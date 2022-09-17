#!/usr/bin/env python3
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

import sys
from xju.xn import in_context,readable_repr,AllFailed,in_function_context,Scope,Xn
from xju.assert_ import Assert
from typing import cast

import io
import sys

from xju.assert_ import Assert

def test1(prog=sys.argv[0]):
    def a():
        raise Exception('fred')
    def b():
        a()
        pass
    def c():
        b()
        pass
    try:
        c()
    except:
        e=in_context('jock')
        Assert(e.readable_repr())=='''\
Failed to jock because
fred.\
'''
        Assert(str(e)).matches('''\
failed to jock because
[^:]*:39: failed to c[(][)] because
[^:]*:36: failed to b[(][)] because
[^:]*:33: failed to a[(][)] because
[^:]*:31: fred'''.format(**vars()))
        pass
    pass

class E(Exception):
    def __str__(self):
        return 'deadline reached'
    pass

def test2():
    try:
        try:
            raise E()
        except:
            raise in_context('await message') from None
        pass
    except E as e:
        Assert(readable_repr(e))=='''\
Failed to await message because
deadline reached.'''
        pass
    pass

def test3():
    try:
        try:
            try:
                raise E()
            except:
                raise in_context('await message') from None
            pass
        except E as e:
            try:
                raise Exception('open failed')
            except:
                raise AllFailed([e,in_context('read default file')]) from None
            pass
        pass
    except:
        Assert(readable_repr(in_context('get content')))=='''\
Failed to get content because
- failed to await message because
  deadline reached; and
- failed to read default file because
  open failed.'''
        pass
    pass

from inspect import currentframe, getframeinfo

class _Scope():
    def __init__(self,s):
        self.s=s
        frameinfo = getframeinfo(currentframe().f_back)
        self.fl=(frameinfo.filename, frameinfo.lineno)
        pass
    def __enter__(self):
        pass
    def __exit__(self,t,e,b):
        if e:
            raise in_context(self.s,(t,e,b),self.fl) from None
        pass

def test4(prog=sys.argv[0]):
    def a():
        raise Exception('fred')
    def b():
        with _Scope('b'):
            True
            a()
            pass
        pass
    def c():
        b()
        pass
    try:
        c()
    except:
        e=in_context('jock')
    else:
        assert False, 'c should have raised?'
        pass
    Assert(str(e)).matches('''\
failed to jock because
[^:]*:127: failed to c[(][)] because
[^:]*:124: failed to b[(][)] because
[^:]*:118: failed to with _Scope[(]'b'[)]: because
[^:]*:111: failed to raise in_context[(]self.s,[(]t,e,b[)],self.fl[)] from None because
[^:]*:118: failed to b because
[^:]*:120: failed to a[(][)] because
[^:]*:116: fred''')
    Assert(e.readable_repr())=='''\
Failed to jock because
failed to b because
fred.\
'''
    pass

def test6():
    class X:
        def jump(self, height:int):
            '''jump {height}m in the air
               - from a standing start'''
            try:
                raise Exception('I have lead feet')
            except Exception as e:
                raise in_function_context(X.jump,vars()) from None
            pass
        pass
    try:
        X().jump(10)
    except Exception as e:
        Assert(e.readable_repr())=='Failed to jump 10m in the air because\nI have lead feet.'
    else:
        assert False, 'should not be here'
        pass
    pass

sys.stdout=io.StringIO()
with Scope('do some stuff') as scope:
    pass
Assert(sys.stdout.getvalue())=='''\
INFO: + do some stuff
INFO: - do some stuff = None
'''
sys.stdout=io.StringIO()
def f():
    with Scope('do some stuff') as scope:
        return scope.result(77)
    pass

Assert(f())==77
Assert(sys.stdout.getvalue())=='''\
INFO: + do some stuff
INFO: - do some stuff = 77
'''
def f2():
    with Scope('do some stuff') as scope:
        raise Exception('did not work')
    pass
try:
    f2()
except Exception as e:
    ee:Xn=cast(Xn,e)
    Assert(ee.readable_repr())=='''\
Failed to do some stuff because
did not work.'''
else:
    assert False,'should not reach'
    pass

tests=[var for name,var in sorted(vars().items())
       if name.startswith('test') and callable(var)]
for t in tests: t()
n=len(tests)
print('PASS - {n} steps'.format(**vars()))
pass

