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

from xju.xn import firstLineOf as l1,inContext
from xju import pq
from .etc import fromJson,toJson
from .wsgi import getVariablesFromWSGIenviron, getCookiesFromWSGIenviron, getHTTPHeadersFromWSGIenviron
from typing import Set,Callable,Dict,Union,Tuple,List
    
class Response:
    def __init__(self,
                 content:Union[bytes, #pre-encoded
                               None]=None, #REVISIT: rules
                 contentType:Union[str,None]=None, # e.g. text/html; charset=UTF-8
                 contentEncoding:Union[str,None]=None):
        assert content is None or isinstance(content,bytes),type(content)
        assert contentType is None or content is not None, (contentType,type(content))
        assert contentEncoding is None or content is not None, (contentEncoding,type(content))
        self.content:Union[bytes,None]=content
        self.contentType:Union[str,None]=contentType
        self.contentEncoding:Union[str,None]=contentEncoding
        self.cookies:Dict[str,Tuple[str,Dict[str,str]]]={} # name : (value, {attrName:attrValue} )
        self.headers:List[Tuple[str,str]]=[] # (name,value)
        self.location:Union[None,str]=None # for redirect
        pass
    def __str__(self):
        c=(self.content or '')[0:150]
        if len(self.content or '')>150: c=c+'...'
        h=self.headers
        k=self.cookies
        return 'content {c!r}, headers {h!r}, cookies {k!r}'.format(**vars())
    def __add__(self,rhs_):
        'add {rhs_} to Response {self}'
        try:
            rhs=promoteContent(rhs_)
            assert isinstance(rhs,Response),type(rhs)
            lhs=self
            assert lhs.content is None or rhs.content is None, 'content specified more than once'
            result=Response()
            result.content=rhs.content or lhs.content
            result.contentType=rhs.contentType or lhs.contentType
            result.contentEncoding=rhs.contentEncoding or lhs.contentEncoding
            result.cookies=lhs.cookies.copy()
            result.cookies.update(rhs.cookies)
            result.headers=lhs.headers+rhs.headers
            result.location=rhs.location or lhs.location
            return result
        except:
            raise inContext(l1(Response.__add__.__doc__).format(**vars())) from None
        pass
    def __radd__(self,lhs_):
        'add Response {self} to {lhs_}'
        try:
            return promoteContent(lhs_).__add__(self)
        except:
            raise inContext(l1(Response.__radd__.__doc__).format(**vars())) from None
        pass
    def cookieHeaders(self):
        '''return headers for {self.cookies}'''
        try:
            result=[] # ('Set-Cookie', xxxx }
            for name,va in self.cookies.items():
                value=va[0]+''.join(['; {an}={av}'.format(**vars())
                                     for an,av in va[1].items()])
                result.append( ('Set-Cookie',f'''{name}={value}''') )
                pass
            return result
        except:
            raise inContext(
                l1(Response.cookieHeaders.__doc__).format(**vars())) from None
        pass
    pass

public_functions:Set[str]=set()

restricted_functions:Dict[str,Callable]={}

class ClientError(Exception):
    'error to pass back to client'
    def __init__(self,s):
        Exception.__init__(self,s)
        self.error=s
        pass
    pass

class Forbidden(Exception):
    def __init__(self,description):
        Exception.__init__(self,description)
        pass
    pass

class NotFound(Exception):
    def __init__(self,description):
        Exception.__init__(self,description)
        pass
    pass

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
        raise inContext('get value of %(param_name)s from params supplied as json-encoded "json_params" HTTP param (%(json_param_names)s), HTTP params (%(request_param_names)s), webapp2 request attributes (%(request_attr_names)s) or function paramter defaults (%(param_default_names)s)'%vars()) from None
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
        raise inContext(l1(makeParams.__doc__)%vars()) from None
    pass

def promoteContent(content:Union[Response, #already good
                                 pq.Selection, #text/html
                                 dict,bool,list,float]): #text/json REVISIT: rules
    '''promote content object to a valid Response'''
    contentType=type(content)
    try:
        if isinstance(content,Response):
            return content
        if isinstance(content,pq.Selection):
            return Response(content.utf8(),
                            'text/html; charset=UTF-8')
        if isinstance(content,dict) and (
            'result' in content or
            'error' in content):
            return Response(toJson(content).encode('utf-8'),
                            'text/json; charset=UTF-8')
        if content is None or \
           isinstance(content,dict) or isinstance(content,list) or \
           isinstance(content,int) or isinstance(content,float):
            return Response(toJson({'result':content}).encode('utf-8'),
                            'text/json; charset=UTF-8')
        raise Exception('do not know what HTTP HTTP CONTENT-TYPE to use for a {contentType} object - return an explicit wal.Response to set CONTENT-TYPE'.format(**vars()))
    except:
        raise inContext(l1(promoteContent.__doc__).format(**vars())) from None
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
                self.dispatchToFunction(path,environ,start_response) or \
                self.dispatchToFile(path,start_response) or \
                self.notFound(path,start_response)
        except Forbidden as e:
            self.log('INFO: '+str(
                inContext(l1(Dispatcher.main.__doc__).format(**vars()))))
            return self.forbidden(start_response)
        except NotFound as e:
            self.log(str(
                inContext(l1(Dispatcher.main.__doc__).format(**vars()))))
            return self.notFound(path,start_response)
        except ClientError as e:
            return self.clientError(start_response,e)
        except Exception as e:
            self.logError('ERROR: '+str(
                inContext(l1(Dispatcher.main.__doc__).format(**vars()))))
            return self.serverError(start_response)
        pass
    def dispatchToFunction(self,name,environ,start_response):
        'dispatch {name} to function in {self.app} a name->function dictionary'
        'e.g. app can be a module'
        try:
            fname=name.replace('-','_').replace(' ','_').replace('.','_')
            f=self.app.__dict__.get(fname,None)
            if not f: return None
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
                headers=getHTTPHeadersFromWSGIenviron(environ)
                method=environ['REQUEST_METHOD'] #e.g. 'GET', 'POST'
                referer=environ.get('HTTP_REFERER',None)
                remote_addr=environ['REMOTE_ADDR']
                result=f(**makeParams(remote_addr,
                                      method,
                                      headers,
                                      params,
                                      url,
                                      referer,
                                      cookies,
                                      f))
                pass
            self.log('INFO: {name} used app.{fname}()'.format(**vars()))
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
            raise inContext(l1(Dispatcher.dispatchToFunction.__doc__).format(
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


