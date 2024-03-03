# Copyright (c) 2018 Trevor Taylor
# 
# Permission to use, copy, modify, and/or distribute this software for
# any purpose with or without fee is hereby granted, provided that all
# copyright notices and this permission notice appear in all copies.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

import os
import mimetypes

from xju.xn import first_line_of as l1,in_context
from xju import pq
from xju.misc import fromJson,toJson
from wal.wsgi import getVariablesFromWSGIenviron, getCookiesFromWSGIenviron, getHTTPHeadersFromWSGIenviron
from typing import Set,Callable,Dict,Union,Tuple,List
from wal import public_functions, restricted_functions,ClientError,Forbidden,NotFound,Response,promoteContent

def getParam(param_name,
             json_params,
             request_params,
             request_attrs,
             param_defaults):
    try:
        try:
            if param_name=='json_params':
                return json_params
            if param_name in json_params:
                return json_params.get(param_name)
            if param_name in request_params:
                return request_params.get(param_name)
            if param_name in request_attrs:
                return request_attrs.get(param_name)
            return param_defaults[param_name]
        except KeyError as e:
            raise Exception(f'unknown parameter {param_name}')
        pass
    except:
        json_param_names=json_params.keys()
        request_param_names=request_params.keys()
        request_attr_names=request_attrs.keys()
        param_default_names=param_defaults.keys()
        raise in_context('get value of %(param_name)s from params supplied as json-encoded "json_params" HTTP param (%(json_param_names)s), HTTP params (%(request_param_names)s), webapp2 request attributes (%(request_attr_names)s) or function paramter defaults (%(param_default_names)s)'%vars()) from None
    pass

def makeParams(remote_addr,method,headers,params,url,referer,cookies,f):
    'make dictionary of params for calling function %(f)s, getting them from remote_addr, method, headers, GET/POST params, url, cookies'''
    try:
        json_params=fromJson(params.get('json_params','{}'))
        param_names=f.__code__.co_varnames[0:f.__code__.co_argcount]
        func_defaults=f.__defaults__ or []
        no_default_params=len(param_names)-len(func_defaults)
        param_defaults=dict([ (param_name,default_value) for param_name,default_value in zip(param_names[no_default_params:],func_defaults)])
        #request.__dict__ does not give us anything useful
        request_attrs={
            'params':params,
            'cookies':cookies,
            'headers':headers,
            'method':method,
            'referer':referer,
            'url':url,
            'remote_addr':remote_addr,
            }
        result=dict([ (_,getParam(_,
                                  json_params,
                                  params,
                                  request_attrs,
                                  param_defaults)) for _ in param_names ])
        return result
    except:
        raise in_context(l1(makeParams.__doc__)%vars()) from None
    pass

class Dispatcher:
    def __init__(self,app,dir,
                 log=lambda s: print('INFO: {s}'.format(**vars())),
                 logError=lambda s: print('ERROR: {s}'.format(**vars()))):
        '''serve pages from {app}'s functions and {dir}'s files'''
        '''(see self.main)'''
        self.app=app
        self.dir=dir
        self.log=log
        self.logError=logError
        pass
    def main(self,environ,start_response):
        '''dispatch request descibed by WSGI environ
             - satisfies request by corresponding names function in self.app
             - fallback is static file from self.dir
           (WSGI application function)'''
        try:
            if len(environ['PATH_INFO'])>1:
                assert environ['PATH_INFO'].startswith('/') #wsgi always does
                path=environ['PATH_INFO'][1:]
            else:
                path='index.html'
                pass
            return \
                self.dispatchToFunction(path,environ,start_response,self.app) or \
                self.dispatchToFile(path,start_response) or \
                self.notFound(path,start_response)
        except Forbidden as e:
            self.log('INFO: '+str(
                in_context(l1(Dispatcher.main.__doc__).format(**vars()))))
            return self.forbidden(start_response)
        except NotFound as e:
            self.log(str(
                in_context(l1(Dispatcher.main.__doc__).format(**vars()))))
            return self.notFound(path,start_response)
        except ClientError as e:
            return self.clientError(start_response,e)
        except Exception as e:
            self.logError('ERROR: '+str(
                in_context(l1(Dispatcher.main.__doc__).format(**vars()))))
            return self.serverError(start_response)
        pass
    def dispatchToFunction(self,name,environ,start_response,mod):
        'dispatch {name} to function in {mod} a name->function dictionary'
        'e.g. app can be a module'
        try:
            if '/' in name:
                m,n=name.split('/',1)
                if m in mod.__dict__:
                    return self.dispatchToFunction(n,environ,start_response,mod.__dict__[m])
                else:
                    return None
                pass
            fname=name.replace('-','_').replace(' ','_').replace('.','_')
            f=mod.__dict__.get(fname,None)
            if f is None: return None
            if not callable(f):
                raise Forbidden('%(fname)s is not callable ie not a function like "def %(fname)s():"'%vars())
            if not f in public_functions and not f in restricted_functions:
                raise Forbidden('%(fname)s is not annotated with @wal.public (or @public) or @wal.restricted (or @restricted)'%vars())
            result=None
            url=(environ.get('wsgi.url_scheme')+'://'+
                 environ.get('HTTP_HOST',
                             environ.get('SERVER_NAME')+':'+
                             environ.get('SERVER_PORT'))+
                 environ.get('SCRIPT_NAME','')+
                 environ.get('PATH_INFO',''))
            cookies=getCookiesFromWSGIenviron(environ)
            if f in restricted_functions:
                result=restricted_functions[f](url,cookies)
                pass
            if not result:
                params=getVariablesFromWSGIenviron(environ)
                in_headers=getHTTPHeadersFromWSGIenviron(environ)
                method=environ['REQUEST_METHOD'] #e.g. 'GET', 'POST'
                referer=environ.get('HTTP_REFERER',None)
                remote_addr=environ['REMOTE_ADDR']
                result=f(**makeParams(remote_addr,
                                      method,
                                      in_headers,
                                      params,
                                      url,
                                      referer,
                                      cookies,
                                      f))
                pass
            self.log('INFO: {name} used {mod.__name__}.{fname}()'.format(**vars()))
            result=promoteContent(result)
            headers=result.cookieHeaders()
            if result.location:
                headers.append( ('Location',result.location) )
                start_response('303 See Other',headers)
                return [''.encode('utf-8')]
            headers.extend([(n,v) for n,v in result.headers
                            if not n in ('Content-Type','Content-Encoding')])
            c=dict([ (n,v) for n,v in result.headers
                     if n in ('Content-Type','Content-Encoding')])
            if result.contentType: c['Content-Type']=result.contentType
            if result.contentEncoding: c['Content-Encoding']=result.contentEncoding
            headers.extend(c.items())
            start_response('200 OK',headers)
            return [result.content]
        except:
            raise in_context(l1(Dispatcher.dispatchToFunction.__doc__).format(
                **vars())) from None
        pass
    
    def dispatchToFile(self,path,start_response):
        'dispatch {self.dir}/{path} if it exists as a plain file'
        x=os.path.join(self.dir,path)
        if os.path.exists(x):
            t=mimetypes.guess_type(path,False)
            headers=[('Content-Type',t[0] or 'text/plain')]
            if t[1]:
                headers.append(('Content-Encoding',t[1]))
                pass
            with open(x,'rb') as f:
                start_response('200 OK', headers)
                return [f.read()]
            pass
        return None
    
    def notFound(self,path,start_response):
        self.log('INFO: {path} not found'.format(**vars()))
        start_response('404 Not Found', 
                       [('Content-Type', 'text/plain; charset=UTF-8')])
        return ['Not Found'.encode('utf-8')]
    
    def forbidden(self,start_response):
        start_response('403 Forbidden', 
                       [('Content-Type', 'text/plain; charset=UTF-8')])
        return ['Forbidden'.encode('utf-8')]
    
    def serverError(self,start_response):
        start_response('500 Internal Server Error', 
                       [('Content-Type', 'text/plain; charset=UTF-8')])
        return ['Server Error'.encode('utf-8')]
    
    def clientError(self,start_response,e):
        start_response('400 Bad Request', 
                       [('Content-Type', 'text/plain; charset=UTF-8')])
        return [Exception.__str__(e).encode('utf-8')]
    
    pass


