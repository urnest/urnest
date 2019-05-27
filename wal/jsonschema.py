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

from xn import Xn,inContext,firstLineOf,readableRepr

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
                    raise 'list {x} does not have {i} elements, it has {j} elements'.format(**vars())
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
                    

def test():
    from assert_ import Assert
    Schema(None).validate(None)
    Schema(2).validate(2)
    Schema('fred').validate('fred')
    Schema(int).validate(4)
    Schema(bool).validate(True)
    Schema(float).validate(4)
    Schema(float).validate(7.6)
    Schema(str).validate('fred')
    Schema(str).validate(u'fred')
    Schema([int]).validate([])
    Schema([int]).validate([5,6])
    Schema({'a':int,'b':str}).validate({'a':8,'b':'fred'})
    Schema([{'a':int,'b':str}]).validate([{'a':8,'b':'fred'},{'a':7,'b':'jock'}])
    Schema((int,str)).validate([8,'fred'])
    Schema({int:str}).validate({1:'fred',2:'jock'})
    Schema({str:int}).validate({'fred':1,'jock':2})
    Schema({str:int}).validate({u'fred':1,'jock':2})
    Schema(OneOf(int,str)).validate(1)
    Schema(OneOf(int,str)).validate('fred')
    Schema([Schema(int)]).validate([1,2,3])
    Schema(False).validate(False)
    try:
        Schema(int).validate('fred')
        assert None
    except Exception as e:
        assert readableRepr(e)=="Failed to verify 'fred' conforms to json schema <class 'int'> because\n'fred' is not an Int.",repr(readableRepr(e))
    try:
        Schema(float).validate('fred')
        assert None
    except Exception as e:
        assert readableRepr(e)=="Failed to verify 'fred' conforms to json schema <class 'float'> because\n'fred' is not a Float.",repr(readableRepr(e))
    try:
        Schema(str).validate(4)
        assert None
    except Exception as e:
        assert readableRepr(e)=="Failed to verify 4 conforms to json schema <class 'str'> because\n4 is not a String.",repr(readableRepr(e))
    try:
        Schema([str]).validate([4])
        assert None
    except Exception as e:
        assert readableRepr(e)=="Failed to verify [4] conforms to json schema [<class 'str'>] because\nfailed to validate list item 0 because\nfailed to verify 4 conforms to json schema <class 'str'> because\n4 is not a String.",repr(readableRepr(e))
    try:
        Schema([str]).validate(['fred',4])
        assert None
    except Exception as e:
        assert readableRepr(e)=="Failed to verify ['fred', 4] conforms to json schema [<class 'str'>] because\nfailed to validate list item 1 because\nfailed to verify 4 conforms to json schema <class 'str'> because\n4 is not a String.",repr(readableRepr(e))
    try:
        Schema({'a':int,'b':str}).validate({'a':8,'b':6})
        assert None
    except Exception as e:
        assert readableRepr(e)=="Failed to verify {'a': 8, 'b': 6} conforms to json schema {'a': <class 'int'>, 'b': <class 'str'>} because\nfailed to validate dictionary item 'b' because\nfailed to verify 6 conforms to json schema <class 'str'> because\n6 is not a String."or readableRepr(e)=="Failed to verify {'b': 6, 'a': 8} conforms to json schema {'b': <class 'str'>, 'a': <class 'int'>} because\nfailed to validate dictionary item 'b' because\nfailed to verify 6 conforms to json schema <class 'str'> because\n6 is not a String.",repr(readableRepr(e))
        pass
    try:
        Schema({int:str}).validate({'sal':'fred',2:'jock'})
        assert None
    except Exception as e:
        assert readableRepr(e)=="Failed to verify "+repr({'sal':'fred',2:'jock'})+" conforms to json schema {<class 'int'>: <class 'str'>} because\nfailed to validate dictionary item 'sal' because\nfailed to verify 'sal' conforms to json schema <class 'int'> because\n'sal' is not an Int.",repr(readableRepr(e))
        pass
    try:
        Schema(OneOf(int,str)).validate(None)
        assert None
    except Exception as e:
        assert readableRepr(e)=="Failed to verify None conforms to json schema one of (<class 'int'>, <class 'str'>) because\nFailed to verify None conforms to json schema <class 'int'> because\nNone is not an Int. and Failed to verify None conforms to json schema <class 'str'> because\nNone is not a String..",repr(readableRepr(e))
        
        pass
    try:
        Schema(None).validate(True)
        assert None
    except Exception as e:
        assert readableRepr(e)=='Failed to verify True conforms to json schema None because\nTrue is not None.',repr(readableRepr(e))
        pass
    try:
        Schema(2).validate(3)
        assert None
    except Exception as e:
        assert readableRepr(e)=='Failed to verify 3 conforms to json schema 2 because\n3 is not 2.',repr(readableRepr(e))
        pass
    try:
        Schema(2).validate('jock')
        assert None
    except Exception as e:
        assert readableRepr(e)=='''Failed to verify 'jock' conforms to json schema 2 because\n'jock' is not an integer.''',repr(readableRepr(e))
        pass
    try:
        Schema('fred').validate(1)
        assert None
    except Exception as e:
        assert readableRepr(e)=='''Failed to verify 1 conforms to json schema 'fred' because\n1 is not a string.''',repr(readableRepr(e))
        pass
    try:
        Schema('fred').validate('jock')
        assert None
    except Exception as e:
        assert readableRepr(e)=='''Failed to verify 'jock' conforms to json schema 'fred' because\n'jock' is not 'fred'.''',repr(readableRepr(e))
        pass
    try:
        x={'a':int,'c':int}
        Schema(x).validate({'a':1})
    except Exception as e:
        Assert(readableRepr(e))=='''Failed to verify {'a': 1} conforms to json schema '''+repr(x)+''' because\nfailed to validate dictionary item 'c' because\n'c' is not in ['a'].'''
    else:
        assert None
        pass
    try:
        Schema(False).validate(True)
    except Exception as e:
        assert readableRepr(e)=='''Failed to verify True conforms to json schema False because\nTrue is not False.''',repr(readableRepr(e))
    else:
        assert None
        pass

if __name__=='__main__':
    test()
