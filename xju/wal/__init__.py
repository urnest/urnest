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

from xju import jsonschema
from xju import pq
from xju import xn
from . import dispatcher
from . import cookies
from . import etc
    
# convenience aliases
from xju.xn import inContext,firstLineOf as l1,Xn
from xju.jsonschema import Schema as JsonSchema
from .dispatcher import Response,Dispatcher,ClientError
from .etc import fromJson,toJson,Scope
from xju.pq import loadFile as loadHTML,parse as parseHTML
from xju.assert_ import Assert

# decorator - see test/app.py for use
def public(f):
    'make f available via all Dispatchers to web clients (eg browsers)'
    dispatcher.public_functions.add(f)
    return f

# decorator - see test/app.py for use
def restricted(authorizer):
    'make f available via all Dispatchers to web clients (eg browsers) if authorizer(url,cookies) returns None'
    def result(f):
        dispatcher.restricted_functions[f]=authorizer
        return f
    return result

# Response convenience functions - see test/app.py for use
def plainText(text):
    result=Response(content=text.encode('utf-8'),
                    contentType='text/plain; charset=UTF-8')
    return result

def html(text):
    result=Response(content=text.encode('utf-8'),
                    contentType='text/html; charset=UTF-8')
    return result

def cookie(name,value,path='/'):
    result=Response()
    result.cookies={
        cookies.validateCookieName(name): (cookies.validateCookieValue(value),{
            'Path':cookies.validateCookiePath(path)})}
    return result

def header(name,value):
    '''HTTP header'''
    result=Response()
    result.headers=[(name,value)]
    return result

def redirect(location:str,**etc):
    '''HTTP redirect to %(location)s, with extras %(etc)r'''
    cookies=etc.get('cookies',{})
    result=Response()
    result.cookies=cookies
    result.location=location
    return result
pass

