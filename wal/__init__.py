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

# so we can have our unit tests inside contained modules without them
# having to assume they are part of some package
# i.e. python 2 "submodules" not python3 "packages"
class PushPath(object):
    def __init__(self,path):
        self.path=path
        pass
    def __enter__(self):
        sys.path.insert(0,self.path)
        pass
    def __exit__(self,eType,eVal,eTrc):
        assert sys.path[0]==self.path,sys.path[0:2]
        pass
    pass

with PushPath(os.path.dirname(os.path.realpath(__file__))):
    import jsonschema
    import pq
    import xn
    import dispatcher
    import cookies
    import etc
    
    # convenience aliases
    from xn import inContext,firstLineOf as l1,Xn
    from jsonschema import Schema as JsonSchema
    from dispatcher import Response,Dispatcher,ClientError
    from etc import fromJson,toJson,Scope
    from pq import loadFile as loadHTML,parse as parseHTML
    from assert_ import Assert
    
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
    
    def redirect(location,**etc):
        '''HTTP redirect to %(location)s, with extras %(etc)r'''
        cookies=etc.get('cookies',{})
        result=Response()
        result.cookies=cookies
        result.location=location
        return result
    pass

