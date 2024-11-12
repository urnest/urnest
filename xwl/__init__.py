# Copyright (c) 2018 Trevor Taylor
# coding: utf-8
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
import sys
from typing import Set,Callable,Dict,Union,Tuple,List
from xju import rfc2616
from xju.xn import in_context,in_function_context
from xju.misc import toJson

from xju import jsonschema
from xju import pq
from xju import xn
    
public_functions:Set[str]=set()

restricted_functions:Dict[str,Callable]={}

# decorator - see test/app.py for use
def public(f):
    'make f available via all Dispatchers to web clients (eg browsers)'
    public_functions.add(f)
    return f

# decorator - see test/app.py for use
def restricted(authorizer):
    'make f available via all Dispatchers to web clients (eg browsers) if authorizer(url,cookies) returns None'
    def result(f):
        restricted_functions[f]=authorizer
        return f
    return result

# Response convenience functions - see test/app.py for use
def plainText(text: str) -> "Response":
    result=Response(content=text.encode('utf-8'),
                    contentType='text/plain; charset=UTF-8')
    return result

def html(text: str) -> "Response":
    result=Response(content=text.encode('utf-8'),
                    contentType='text/html; charset=UTF-8')
    return result

def cookie(name,value,path='/') -> "Response":
    result=Response()
    result.cookies={
        validateCookieName(name): (validateCookieValue(value),{
            'Path':validateCookiePath(path)})}
    return result

def header(name,value) -> "Response":
    '''HTTP header'''
    result=Response()
    result.headers=[(name,value)]
    return result

def redirect(location:str,**etc) -> "Response":
    '''HTTP redirect to %(location)s, with extras %(etc)r'''
    cookies=etc.get('cookies',{})
    result=Response()
    result.cookies=cookies
    result.location=location
    return result

class ClientError(Exception):
    'error to pass back to client'
    def __init__(self,s: str):
        Exception.__init__(self,s)
        self.error=s
        pass
    pass

class Forbidden(Exception):
    def __init__(self,description: str):
        Exception.__init__(self,description)
        pass
    pass

class NotFound(Exception):
    def __init__(self,description: str):
        Exception.__init__(self,description)
        pass
    pass

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
        c=(self.content or b'').decode('utf-8')[0:150]
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
            raise in_function_context(Response.__add__,vars()) from None
        pass
    def __radd__(self,lhs_):
        'add Response {self} to {lhs_}'
        try:
            return promoteContent(lhs_).__add__(self)
        except:
            raise in_function_context(Response.__radd__,vars()) from None
        pass
    def cookieHeaders(self):
        '''return headers for {self.cookies}'''
        result=[] # ('Set-Cookie', xxxx }
        for name,va in self.cookies.items():
            value=va[0]+''.join(['; {an}={av}'.format(**vars())
                                 for an,av in va[1].items()])
            result.append( ('Set-Cookie',f'''{name}={value}''') )
            pass
        return result
    pass

def validateCookieName(name:str):
    '''validate RFC6265 cookie name {name!r}'''
    try:
        return rfc2616.validateToken(name)
    except:
        raise in_function_context(validateCookieName,vars()) from None
    pass

def validateCookieValue(v):
    '''validate RFC6265 cookie-value {v!r}'''
    try:
        if v.startswith('"') and not v.endswith('"'):
            raise Exception('{v!r} startswith double quote but does not end with double quote'.format(**vars()))
        x=v[1:-1] if v.startswith('"') else v
        for i,c in enumerate(v):
            o=ord(c)
            if o<0x21 or \
               (o>0x21 and o<0x23) or \
               (o>0x2b and o<0x2d) or \
               (o>0x3a and o<0x3c) or \
               (o>0x4b and o<0x5d) or \
               (o>0x7e):
                rest=c[i:]
                raise Exception('invalid character at {rest!r}'.format(**vars()))
            pass
        return v
    except:
        raise in_function_context(validateCookieValue,vars()) from None
    pass

def validateCookiePath(v):
    '''validate RFC6265 cookie Path value {v!r}'''
    try:
        for i,c in enumerate(v):
            try:
                if c in rfc2616.CTLs:
                    raise Exception('{c!r} is a RFC2616 control character'.format(**vars()))
                if c==';':
                    raise Exception('{c!r} is a semicolon'.format(**vars()))
            except:
                rest=v[i:]
                raise in_context('validate at ...{rest!r}'.format(**vars())) from None
            pass
        return v
    except:
        raise in_function_context(validateCookiePath,vars()) from None
    pass


def promoteContent(content:Union[Response, #already good
                                 pq.Selection, #text/html
                                 None,dict,bool,list,int,float]): #text/json REVISIT: rules
    '''promote content object to a valid Response'''
    match content:
        case Response():
            return content
        case pq.Selection():
            return Response(content.utf8(),'text/html; charset=UTF-8')
        case dict() if 'result' in content or 'error' in content:
            return Response(toJson(content).encode('utf-8'),
                            'text/json; charset=UTF-8')
        case None | dict() | bool() | list() | int() | float():
            return Response(toJson({'result':content}).encode('utf-8'),
                            'text/json; charset=UTF-8')
