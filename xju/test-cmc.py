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
import xju.cmc

from xju.assert_ import Assert
import dataclasses

# python behaviour checks
class A():
    a: int
    pass

class B(A):
    b: str
    def set_b_b(self, s: str) -> None:
        self.b = s
        pass
    pass

Assert(A.__annotations__) == {'a': int}
Assert(B.__annotations__) == {'b': str}
Assert(B.__bases__) == (A, )

class C(B, A):
    b: str
    def set_c_b(self, s: str) -> None:
        self.b = s
        pass
    pass

Assert(B.__annotations__) == {'b': str}
Assert(C.__annotations__) == {'b': str} #[2]

c = C()

c.set_c_b('_c_')
c.set_b_b('_b_')
Assert(c.b) == '_b_' #[1]

Assert(c.__dict__) == { 'b': '_b_'} #[3]
c.a = 5 # [4]
Assert(c.__dict__) == { 'b': '_b_', 'a': 5}
Assert(C.__mro__) == [C,B,A]

# [1] + [2] is interesting: even though there is only one actual b attribute there are
# two annotations; would be better if mypy disallowed, because with multi-level
# inheritance can easily stomp on lower-level attribute, especially as in practice noone
# much uses private attributes; for our cmclass decorator we reject such overriding altogether
# this will naturally disallow multiple inheritance of class that has a resource attribute, which
# is good; but only for attrs that implement context management

# [3] no mention of a because it has not been initialised, disallow at compile time too
# [4] that's dangerous from a resource perspective, might enter one resource and exit some
#     other resource; block __setattr__ and __delattr__ after ___enter__ succeeds

# @cmclass tests
step = 1
class Resource:
    entered_at: Optional[int] = None
    exited_at: Optional[int] = None
    ee: Optional[Exception]
    xe: Optional[Exception]

    def __init__(self, ee:Optional[Exception], xe:Optional[Exception]):
        self.ee = ee
        self.xe = xe
        pass

    def __enter__(self):
        global step
        self.entered_at = step
        step = step + 1
        if self.ee: raise self.ee
        return self
    
    def __exit__(self):
        global step
        self.entered_at = step
        step = step + 1
        if self.xe: raise self.xe
        return self

@cmclass
@dataclasses.dataclass
class B1:
    a: Resource
    b: int
    pass

@cmclass
@dataclasses.dataclass
class B2:
    c: str
    d: Resource
    e: Resource
    pass

@cmclass
@dataclasses.dataclass
class D(B2, B1):
    f: Resource
    pass

with D(Resource(ee=None, xe=None), #f
       Resource(ee=None, xe=None), #a
       1, #b
       'c', #c
       Resource(ee=None, xe=None), #d
       Resource(ee=None, xe=None) #e
       ) as x:
    Assert(x.a.entered_at) == 1
    Assert(x.a.exited_at) == None
    Assert(x.d.entered_at) == 2
    Assert(x.d.exited_at) == None
    Assert(x.e.entered_at) == 3
    Assert(x.e.exited_at) == None
    Assert(x.f.entered_at) == 4
    Assert(x.f.exited_at) == None
    pass

Assert(x.a.entered_at) == 1
Assert(x.a.exited_at) == 8
Assert(x.d.entered_at) == 2
Assert(x.d.exited_at) == 7
Assert(x.e.entered_at) == 3
Assert(x.e.exited_at) == 6
Assert(x.f.entered_at) == 4
Assert(x.f.exited_at) == 5

assert False, 'tests for partial entry; tests for exception on exit including multiple'
assert False, 'tests for [1]+[2]'
assert False, 'tests for [4]'
