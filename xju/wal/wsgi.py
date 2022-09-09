# coding: utf-8

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

import urllib
from xju.xn import in_context
from xju.xn import firstLineOf as l1
from xju.assert_ import Assert
from typing import Tuple,Dict,Mapping,Union,List,Any
from urllib.parse import unquote as urlunquote

def parseHeaders(mimePart:bytes)->Tuple[Dict[str,str],bytes]:
    try:
        Assert(mimePart).isInstanceOf(bytes)
        h,rest=mimePart.split(b'\r\n\r\n', 1)
        headers=h.split(b'\r\n',1)
        headers2=[_.split(b':',1) for _ in headers]
        headers3=[(_[0].decode('utf-8'), _[1].strip().decode('utf-8'))
                 for _ in headers2] #REVISIT: not utf-8 but what?
        return dict(headers3), rest
    except:
        mps=mimePart[0:256]
        raise in_context('parse headers from mime part %(mps)s...'%vars()) from None
    pass

def parseQuoted(x:str):
    assert x.startswith('"'), x
    assert x.endswith('"'), x
    return x[1:-1].replace(r'\"','"')

def parseDisposition(dispositionValue:str):
    '''Parse Content-Disposition value {dispositionValue}'''
    try:
        assert dispositionValue.strip().startswith('form-data;'), dispositionValue
        v=dispositionValue.split(';')[1:]
        v2=[_.strip().split('=',1) for _ in v]
        v3=[(_[0],parseQuoted(_[1])) for _ in v2]
        return dict(v3)
    except:
        raise in_context(l1(parseDisposition.__doc__).format(**vars())) from None
    pass

class FileVar(object):
    def __init__(self, filename:str,contentType:str, content:bytes):
        Assert(content).isInstanceOf(bytes)
        self.filename:str=filename
        self.contentType:str=contentType
        self.content:bytes=content
        pass
    pass

StrOrFileVar=Union[str,
                   FileVar, # for each part of multi-part/form-data
                   Tuple[int, Any]]     #  (length, wsgi.input) for application/octet-stream

def getVariablesFromWSGIenviron(wsgiEnv:Mapping[str,Any])->Dict[str,Union[str,FileVar,Tuple[int,Any],List[StrOrFileVar]]]:
    '''parse query string and wsgi.input into a dictionary from WSGI environ {wsgiEnv}'''
    '''like { varName : str or FileVar or list of str or FileVar }, e.g.:'''
    '''  { "id":"883999", "colours":["red","blue"] }'''
    '''... note list is only used where var has multiple values'''
    '''- where Content-Type is application/octet-stream, result dictiony'''
    '''  includes "body", a tuple ( content_length, file )'''
    '''  where content length is from Content-Length header (or -1) and'''
    '''  file is a file-like readable that presents the body of the request'''
    try:
        result:Dict[str,Union[str,FileVar,Tuple[int,Any],List[StrOrFileVar]]]={}
        d:List[Tuple[str,StrOrFileVar]]=[]
        if wsgiEnv.get('QUERY_STRING',''):
            nvs=[_.split('=',1) for _ in wsgiEnv['QUERY_STRING'].split('&')]
            d.extend(
                [ (_[0], urlunquote(_[1].replace('+',' ')))
                  for _ in nvs])
            pass
        if wsgiEnv['REQUEST_METHOD']!='GET' and 'CONTENT_TYPE' in wsgiEnv:
            #application/javascript; charset=utf-8
            ct:str=wsgiEnv['CONTENT_TYPE']
            encoding='iso-8859-1'
            if 'charset=' in ct:
                encoding=ct.split('charset=')[1].split(';')[0]
                pass
            if 'application/x-www-form-urlencoded' in ct:
                l=int(wsgiEnv.get('CONTENT_LENGTH',-1))
                y=wsgiEnv['wsgi.input'].read(l).decode(encoding)
                nvs=[_.split('=') for _ in y.split('&')]
                d.extend(
                    [ (_[0], urllib.parse.unquote(_[1].replace('+',' ')))
                      for _ in nvs])
            elif 'multipart/form-data' in ct:
                assert 'boundary=' in ct,ct
                # standard adds extra '--' in front of all occurrances
                # of bondary (and after last too)
                boundary:bytes=('\r\n--'+
                                ct.split('boundary=')[1]).encode('utf-8')
                body:bytes=b'\r\n'+wsgiEnv['wsgi.input'].read(
                    int(wsgiEnv.get('CONTENT_LENGTH',-1)))
                all:List[bytes]=body.split(boundary)[1:-1]
                for i,x in enumerate(all):
                    try:
                        assert x.startswith(b'\r\n'), x[0:256]
                        x=x[2:]
                        headers,rest=parseHeaders(x)
                        disposition=parseDisposition(
                            headers['Content-Disposition'])
                        if 'filename' in disposition:
                            fileType=headers.get('Content-Type','')
                            d.append( (disposition['name'], 
                                       FileVar(disposition['filename'],
                                               fileType,
                                               rest)))
                        else:
                            d.append( (disposition['name'],rest.decode('utf-8')) )
                            pass
                    except:
                        raise in_context('parse part %(i)s' % vars()) from None
                    pass
                pass
            elif 'application/octet-stream' in ct:
                length=int(wsgiEnv.get('CONTENT_LENGTH',-1))
                print(f'application/octet-stream, length {length}')
                
                d.append( ('body', (length, wsgiEnv['wsgi.input'])) )
            else:
                if int(wsgiEnv.get('CONTENT_LENGTH',0)):
                    raise Exception(
                        'unimplemented CONTENT_TYPE: '+wsgiEnv['CONTENT_TYPE']+' for POST with non-zero CONTENT_LENGTH')
            pass
        for key, value in d:
            if key in result:
                v=result[key]
                if isinstance(v,list):
                    v.append(value)
                else:
                    result[key]=[v,value]
                    pass
                pass
            else:
                result[key]=value
                pass
            pass
        return result
    except:
        raise in_context(l1(getVariablesFromWSGIenviron.__doc__).format(**vars())) from None
    pass

def getCookiesFromWSGIenviron(environ:Mapping[str,str])->Dict[str,str]:
    '''get cookies from WSGI environ {environ} HTTP_COOKIE item as dictionary'''
    '''like { cookieName : str ] }, e.g.:'''
    '''  { "emailAddress" : "fred@dot.com" } '''
    try:
        result={}
        e=environ.get('HTTP_COOKIE',None)
        if e: result=dict([_.strip().split('=') for _ in e.split(';')])
        return result
    except:
        raise in_context(l1(getCookiesFromWSGIenviron.__doc__).format(**vars())) from None
    pass


def getHTTPHeadersFromWSGIenviron(environ)->Dict[str,str]: #name,value
    '''get HTTP_x headers from WSGI environ {environ!r} as dictionary'''
    '''like { headerName : str }, e.g.:'''
    try:
        return dict([(name,value) for name,value in environ.items()
                     if name.startswith('HTTP_')])
    except:
        raise in_context(l1(getHTTPHeadersFromWSGIenviron.__doc__).format(**vars())) from None
    pass
