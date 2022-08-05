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
from xju.assert_ import Assert
from xju.jsonschema import Schema,OneOf
from xju.xn import readableRepr

def test():
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
        Assert(readableRepr(e))=='''Failed to verify {'a': 1} conforms to json schema '''+repr(x)+''' because\nfailed to validate dictionary item 'c' because\n'c' is not in ['a'] and 'c' is not optional.'''
    else:
        assert None
        pass
    x={'a':int,'c':OneOf(int,None)}
    Schema(x).validate({'a':1})
    try:
        Schema(False).validate(True)
    except Exception as e:
        assert readableRepr(e)=='''Failed to verify True conforms to json schema False because\nTrue is not False.''',repr(readableRepr(e))
    else:
        assert None
        pass
    pass

test()
