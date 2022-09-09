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
from xju.xn import in_context,readableRepr,AllFailed,in_function_context
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
        Assert(e.readableRepr())=='''\
Failed to jock because
fred.\
'''
        Assert(str(e)).matches('''\
failed to jock because
[^:]*:33: failed to c[(][)] because
[^:]*:30: failed to b[(][)] because
[^:]*:27: failed to a[(][)] because
[^:]*:25: fred'''.format(**vars()))
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
        Assert(readableRepr(e))=='''\
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
        Assert(readableRepr(in_context('get content')))=='''\
Failed to get content because
- failed to await message because
  deadline reached; and
- failed to read default file because
  open failed.'''
        pass
    pass

from inspect import currentframe, getframeinfo

class Scope():
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
        with Scope('b'):
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
[^:]*:121: failed to c[(][)] because
[^:]*:118: failed to b[(][)] because
[^:]*:115: failed to pass because
[^:]*:105: failed to raise in_context[(]self.s,[(]t,e,b[)],self.fl[)] from None because
[^:]*:112: failed to b because
[^:]*:114: failed to a[(][)] because
[^:]*:110: fred''')
    Assert(e.readableRepr())=='''\
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
        Assert(e.readableRepr())=='Failed to jump 10m in the air because\nI have lead feet.'
    else:
        assert False, 'should not be here'
        pass
    pass

tests=[var for name,var in sorted(vars().items())
       if name.startswith('test') and callable(var)]
for t in tests: t()
n=len(tests)
print('PASS - {n} steps'.format(**vars()))
pass
