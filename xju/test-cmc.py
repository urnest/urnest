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
from xju import cmc

from xju.assert_ import Assert
import dataclasses
from typing import Optional
import contextlib

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
Assert(C.__mro__) == (C,B,A,object)

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
        Assert(self.entered_at)==None
        self.entered_at = step
        step = step + 1
        if self.ee: raise self.ee
        return self
    
    def __exit__(self, t, e, b):
        global step
        Assert(self.entered_at)!=None
        Assert(self.exited_at)==None
        self.exited_at = step
        step = step + 1
        if self.xe: raise self.xe
        pass

x = cmc.Dict({'c':Resource(None,None)})
x['b'] = Resource(None,None)
Assert(None)==x['b'].entered_at
Assert(None)==x['c'].entered_at
Assert(x.keys())=={'c':None,'b':None}.keys()
Assert(list(x))==list({'c':None,'b':None})
Assert(len(x))==2
del x['b']
del x['c']

x = cmc.Dict({'c':Resource(None,None)})
x['b'] = Resource(None,None)
Assert(None)==x['b'].entered_at
Assert(None)==x['c'].entered_at
Assert('c').isIn(x)
Assert(x.keys())=={'c':None,'b':None}.keys()
Assert(x.items())=={'c':x['c'],'b':x['b']}.items()
k,bb=x.popitem()
Assert(k)=='b'
Assert(None)==bb.entered_at
cc=x.pop('c')
Assert(None)==cc.entered_at

x = cmc.Dict({'c':Resource(None,None)})
x['b'] = Resource(None,None)
x.clear()

with cmc.Dict() as x:
    x['c'] = Resource(None,None)
    x['b'] = Resource(None,None)
    Assert(1)==x['c'].entered_at
    Assert(2)==x['b'].entered_at
    Assert(None)==x['c'].exited_at
    Assert(None)==x['b'].exited_at
Assert(1)==x['c'].entered_at
Assert(2)==x['b'].entered_at
Assert(3)==x['b'].exited_at
Assert(4)==x['c'].exited_at

step = 1
with cmc.Dict({'c':Resource(None,None)}) as x:
    x['b'] = Resource(None,None)
    Assert(1)==x['c'].entered_at
    Assert(2)==x['b'].entered_at
    Assert(None)==x['c'].exited_at
    Assert(None)==x['b'].exited_at
Assert(1)==x['c'].entered_at
Assert(2)==x['b'].entered_at
Assert(3)==x['b'].exited_at
Assert(4)==x['c'].exited_at

step = 1
with cmc.Dict() as x:
    x['c'] = Resource(None,None)
    x['b'] = Resource(None,None)
    Assert(1)==x['c'].entered_at
    Assert(2)==x['b'].entered_at
    Assert(None)==x['c'].exited_at
    Assert(None)==x['b'].exited_at
    del x['c']
Assert(2)==x['b'].entered_at
Assert(4)==x['b'].exited_at

step = 1
with cmc.Dict() as x:
    x['c'] = Resource(None,None)
    x['b'] = Resource(None,None)
    Assert(1)==x['c'].entered_at
    Assert(2)==x['b'].entered_at
    Assert(None)==x['c'].exited_at
    Assert(None)==x['b'].exited_at
    k,b = x.popitem()
    Assert(k)=='b'
    Assert(b.exited_at)==3
Assert(1)==x['c'].entered_at
Assert(4)==x['c'].exited_at


step = 1
with cmc.Dict() as x:
    x['c'] = Resource(None,None)
    x['b'] = Resource(None,None)
    Assert(1)==x['c'].entered_at
    Assert(2)==x['b'].entered_at
    Assert(None)==x['c'].exited_at
    Assert(None)==x['b'].exited_at
    cc = x.pop('c')
    Assert(cc.entered_at)==1
    Assert(cc.exited_at)==3
Assert(2)==x['b'].entered_at
Assert(4)==x['b'].exited_at

step = 1
with cmc.Dict() as x:
    x['c'] = Resource(None,None)
    x['b'] = Resource(None,None)
    Assert(x.keys())=={'c':None,'b':None}.keys()
    Assert(1)==x['c'].entered_at
    Assert(2)==x['b'].entered_at
    Assert(None)==x['c'].exited_at
    Assert(None)==x['b'].exited_at
    cc = x.get('c')
    Assert(cc.entered_at)==1
    Assert(cc.exited_at)==None
    cc = x.pop('c')
    dd = x.get('d',Resource(None,None))
    Assert(None)==dd.entered_at
    Assert(None)==dd.exited_at
    dd = x.pop('d',Resource(None,None))
    Assert(cc.entered_at)==1
    Assert(cc.exited_at)==3
    Assert(None)==dd.entered_at
    Assert(None)==dd.exited_at
Assert(2)==x['b'].entered_at
Assert(4)==x['b'].exited_at

step = 1
with cmc.Dict() as x:
    x['c'] = Resource(None,None)
    x.setdefault('b',Resource(None,None))
    Assert(1)==x['c'].entered_at
    Assert(2)==x['b'].entered_at
    Assert(None)==x['c'].exited_at
    Assert(None)==x['b'].exited_at
    bb=x['b']
    x['b'] = bb
    Assert(1)==x['c'].entered_at
    Assert(2)==x['b'].entered_at
    Assert(None)==x['c'].exited_at
    Assert(None)==x['b'].exited_at
    d=Resource(None,None)
    x.setdefault('b',d)
    Assert(None)==d.entered_at
    cc=x['c']
    bb=x['b']
    x.clear()
    Assert(1)==cc.entered_at
    Assert(2)==bb.entered_at
    Assert(3)==bb.exited_at
    Assert(4)==cc.exited_at
    pass

step=1
with cmc.Dict() as x:
    x['c'] = Resource(None,None)
    x['b'] = Resource(None,None)
    Assert(1)==x['c'].entered_at
    Assert(2)==x['b'].entered_at
    Assert(None)==x['c'].exited_at
    Assert(None)==x['b'].exited_at
    bb=x['b']
    x.update({'b':bb, 'd':Resource(None,None)})
    pass

Assert(1)==x['c'].entered_at
Assert(2)==x['b'].entered_at
Assert(3)==x['d'].entered_at
Assert(4)==x['d'].exited_at
Assert(5)==x['b'].exited_at
Assert(6)==x['c'].exited_at

step=1
with cmc.Dict() as x:
    x['c'] = Resource(None,None)
    x['b'] = Resource(None,None)
    Assert(1)==x['c'].entered_at
    Assert(2)==x['b'].entered_at
    Assert(None)==x['c'].exited_at
    Assert(None)==x['b'].exited_at
    bb=x['b']
    x.update([('b',bb), ('d',Resource(None,None))])
    pass

Assert(1)==x['c'].entered_at
Assert(2)==x['b'].entered_at
Assert(3)==x['d'].entered_at
Assert(4)==x['d'].exited_at
Assert(5)==x['b'].exited_at
Assert(6)==x['c'].exited_at

step=1
bbb=Resource(None,None)
ccc=Resource(Exception('fred'),None)
try:
    with cmc.Dict({'b':bbb,'c':ccc}) as x:
        pass
    pass
except Exception as e:
    Assert(str(e))=='fred'
    Assert(bbb.entered_at)==1
    Assert(ccc.entered_at)==2
    Assert(ccc.exited_at)==None
    Assert(bbb.exited_at)==3
else:
    assert False, x
    pass

step=1
bbb=Resource(None,None)
ccc=Resource(None,Exception('fred'))
try:
    with cmc.Dict({'b':bbb,'c':ccc}) as x:
        pass
    pass
except Exception as e:
    Assert(str(e))=='fred'
    Assert(bbb.entered_at)==1
    Assert(ccc.entered_at)==2
    Assert(ccc.exited_at)==3
    Assert(bbb.exited_at)==4
else:
    assert False, x
    pass

step=1
bbb=Resource(None,Exception('fred'))
ccc=Resource(None,None)
try:
    with cmc.Dict({'b':bbb,'c':ccc}) as x:
        pass
    pass
except Exception as e:
    Assert(str(e))=='fred'
    Assert(bbb.entered_at)==1
    Assert(ccc.entered_at)==2
    Assert(ccc.exited_at)==3
    Assert(bbb.exited_at)==4
else:
    assert False, x
    pass


@cmc.cmclass
@dataclasses.dataclass
class B1(cmc.CM):
    a: Resource
    b: int
    pass

@cmc.cmclass
@dataclasses.dataclass
class B2(cmc.CM):
    c: str
    d: Resource
    e: Resource
    pass

@cmc.cmclass
@dataclasses.dataclass
class DD(B2, B1):
    f: Resource
    pass

step=1

with DD(Resource(ee=None, xe=None), #a
       1, #b
       'c', #c
       Resource(ee=None, xe=None), #d
       Resource(ee=None, xe=None), #e
       Resource(ee=None, xe=None), #f
       ) as z:
    Assert(z.a.entered_at) == 3
    Assert(z.a.exited_at) == None
    Assert(z.d.entered_at) == 1
    Assert(z.d.exited_at) == None
    Assert(z.e.entered_at) == 2
    Assert(z.e.exited_at) == None
    Assert(z.f.entered_at) == 4
    Assert(z.f.exited_at) == None
    pass

Assert(z.a.entered_at) == 3
Assert(z.a.exited_at) == 6
Assert(z.d.entered_at) == 1
Assert(z.d.exited_at) == 8
Assert(z.e.entered_at) == 2
Assert(z.e.exited_at) == 7
Assert(z.f.entered_at) == 4
Assert(z.f.exited_at) == 5

assert False, 'tests for partial entry; tests for exception on exit including multiple'
assert False, 'tests for [1]+[2]'
assert False, 'tests for [4]'
