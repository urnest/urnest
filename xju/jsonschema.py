#!/usr/bin/env python3
# coding: utf-8
#
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

from .xn import Xn,inContext,firstLineOf,readableRepr

l1=firstLineOf

class Schema:
    def __init__(self,x):
        'initialse schema %(x)r'
        validateSchemaElement(x)
        self.x=x
        pass
    def __repr__(self):
        return repr(self.x)
    def validate(self,x):
        'verify that %(x)r conforms to jsonschema.Schema %(self)r'%vars()
        return validate(self.x,x)
        return
    pass

class OneOf:
    def __init__(self,*choices):
        self.choices=choices
        pass
    def __str__(self):
        return 'one of %(choices)s'%self.__dict__
    def __repr__(self):
        return 'one of %(choices)r'%self.__dict__
    pass

def validateSchemaElement(x):
    'verify that %(x)r is a valid json schema element'
    try:
        if x is None: return
        if x in [int,str,float,bool]: return
        if type(x) in [int,str]: return #literal
        if type(x) is dict:
            if len(x) == 1 and list(x.keys())[0] in (int,str):
                validateSchemaElement(list(x.values())[0])
                return
            for name,y in x.items():
                try:
                    assert type(name) is str, type(name)
                    validateSchemaElement(y)
                except:
                    raise inContext('validate dict schema item %(name)r'%vars())
                pass
            return
        if type(x) is list:
            if len(x) == 0:
                raise Exception(
                    'list schema must contain at least one element'%vars())
            for xx in x: validateSchemaElement(xx)
            return
        if type(x) is tuple:
            for i,y in enumerate(x):
                try:
                    validateSchemaElement(y)
                except:
                    raise inContext('validate tuple schema item %(i)r'%vars())
                pass
            return
        if isinstance(x,OneOf):
            for c in x.choices:
                validateSchemaElement(c)
                pass
            return
        if type(x) is bool:
            #fixed literal value True or False
            return
        if isinstance(x,Schema):
            return
        t=type(x)
        if t is object: t=x.__class__
        raise Exception('jsonschema element may not be a %(t)s, it must be a list, a dictionary or int, str, float, bool, tuple or None'%vars())
    except:
        raise inContext(l1(validateSchemaElement.__doc__)%vars())
    pass

def validate(schema,x):
    'verify %(x)r conforms to json schema %(schema)r'
    try:
        if schema is None and not x is None:
            raise Exception('%(x)r is not None'%vars())
        if type(schema) is int:
            if not type(x) is int:
                raise Exception('%(x)r is not an integer'%vars())
            if not x==schema:
                raise Exception('%(x)r is not %(schema)r'%vars())
            pass
        if type(schema) in [str]:
            if not type(x) in [str]:
                raise Exception('%(x)r is not a string'%vars())
            if not x==schema:
                raise Exception('%(x)r is not %(schema)r'%vars())
            pass
        if schema is int and not type(x) is int:
            raise Exception('%(x)r is not an Int'%vars())
        if schema is bool and not type(x) is bool:
            raise Exception('%(x)r is not a Boolean'%vars())
        if schema is float and not type(x) in [int,float]:
            raise Exception('%(x)r is not a Float'%vars())
        if schema is str and not type(x) in [str]:
            raise Exception('%(x)r is not a String'%vars())
        if type(schema) is dict:
            if not type(x) is dict:
                raise Exception('%(x)r is not a Dictionary'%vars())
            if len(schema)==1 and list(schema.keys())[0] in (int,str):
                for key,y in x.items():
                    try:
                        validate(list(schema.keys())[0],key)
                        validate(list(schema.values())[0],y)
                    except:
                        raise inContext('validate dictionary item %(key)r'%vars())
                    pass
                return x
            for name, y in schema.items():
                try:
                    if not name in x:
                        keys=list(x.keys())
                        raise Exception('%(name)r is not in %(keys)r'%vars())
                    validate(y,x[name])
                except:
                    raise inContext('validate dictionary item %(name)r'%vars())
                pass
            return x
        if type(schema) is list:
            if not type(x) is list:
                raise Exception('%(x)r is not a List'%vars())
            if len(schema)==1:
                for i,y in enumerate(x):
                    try:
                        validate(schema[0],y)
                    except:
                        raise inContext('validate list item %(i)r'%vars())
                    pass
                pass
            else:
                if len(schema)!=len(x):
                    i=len(schema)
                    j=len(x)
                    raise Exception('list {x} does not have {i} elements, it has {j} elements'.format(**vars()))
                for i,v in enumerate(zip(schema,x)):
                    try:
                        validate(v[0],v[1])
                    except:
                        raise inContext('validate list element {i}'.format(**vars()))
                pass
            pass
        if type(schema) is tuple:
            if len(schema) != len(x):
                sl=len(schema)
                xl=len(x)
                raise Exception('tuple has %(xl)s items not %(sl)s'%vars())
            for i,y in enumerate(x):
                try:
                    validate(schema[i],y)
                except:
                    raise inContext('validate tuple schema element %(i)s'%vars())
                pass
            pass
        if isinstance(schema,OneOf):
            choices=schema.choices[:]
            failures=[]
            while len(choices):
                try:
                    validate(choices[0],x)
                    return x
                except Exception as e:
                    failures.append(e)
                    pass
                choices=choices[1:]
                pass
            raise Exception(' and '.join([readableRepr(_) for _ in failures]))
        if type(schema) is bool:
            if x==schema: return x
            raise Exception('{x!r} is not {schema!r}'.format(**vars()))
        if isinstance(schema,Schema):
            schema.validate(x)
            pass
        return x
    except:
        raise inContext(l1(validate.__doc__)%vars())
    pass
                    

