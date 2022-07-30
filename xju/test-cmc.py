# Copyright (c) 2022 Trevor Taylor
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
from xju.cmc import namedtuple
from typing import Optional, Any, NamedTuple
import collections

step =1
class A:
    entered_at_step:Optional[int] = None
    exited_at_step:Optional[int] = None

    def __init__(self,
                 value:Any,
                 ee:Optional[Exception]=None,
                 xe:Optional[Exception]=None):
        self.value = value
        self.entered_at_step:Optional[int] = None
        self.exited_at_step:Optional[int] = None
        self.ee=ee
        self.xe=xe
    def __str__(self):
        return f'entered value: {self.value}, entered step: {self.entered_at_step}, exited step: {self.exited_at_step}'
    def __repr__(self):
        return str(self)
    def __enter__(self):
        global step
        self.entered_at_step = step
        step = step + 1
        if self.ee: raise self.ee
        return self.value

    def __exit__(self, t, e, b):
        global step
        self.exited_at_step = step
        step = step + 1
        if self.xe: raise self.xe

X = namedtuple(NamedTuple('X', [('a',A), ('b',A), ('c',A)]))

with X(A(7), A(8), A(9)) as x:
    assert x.a.value == 7, x.a.value
    assert x.b.value == 8, x.a.value
    assert x.b.value == 9, x.a.value
    assert x.a.entered_at_step == 1, (x.a, x.b, x.c)
    assert x.b.entered_at_step == 2, (x.a, x.b, x.c)
    assert x.c.entered_at_step == 3, (x.a, x.b, x.c)
    assert x.a.exited_at_step == None, (x.a, x.b, x.c)
    assert x.b.exited_at_step == None, (x.a, x.b, x.c)
    assert x.c.exited_at_step == None, (x.a, x.b, x.c)

assert x.a.entered_at_step == 1, (x.a, x.b, x.c)
assert x.b.entered_at_step == 2, (x.a, x.b, x.c)
assert x.c.entered_at_step == 3, (x.a, x.b, x.c)
assert x.a.exited_at_step == 6, (x.a, x.b, x.c)
assert x.b.exited_at_step == 5, (x.a, x.b, x.c)
assert x.c.exited_at_step == 4, (x.a, x.b, x.c)
    
step=11

try:
    with X(a=A(17), b=A(18,ee=Exception('fred')), c=A(19)) as x:
        pass
except Exception as e:
    assert str(e) == 'fred', str(e)
    assert x.a.entered_at_step == 11, (x.a, x.b, x.c)
    assert x.b.entered_at_step == 12, (x.a, x.b, x.c)
    assert x.c.entered_at_step == None, (x.a, x.b, x.c)
    assert x.a.exited_at_step == 14, (x.a, x.b, x.c)
    assert x.b.exited_at_step == 13, (x.a, x.b, x.c)
    assert x.c.exited_at_step == None, (x.a, x.b, x.c)
else:
    assert False, 'should not get to here'
