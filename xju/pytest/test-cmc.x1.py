#!/usr/bin/python3

# multiple inheritance should be disallowed

from xju import cmc

from xju.assert_ import Assert
from typing import Optional

step = 1
class Resource:
    name: str
    entered_at: Optional[int] = None
    exited_at: Optional[int] = None
    ee: Optional[Exception]
    xe: Optional[Exception]

    def __init__(self, name:str, ee:Optional[Exception], xe:Optional[Exception]):
        self.name = name
        self.ee = ee
        self.xe = xe
        pass

    def __enter__(self):
        global step
        print(f'+ enter {self.name} @ {step}')
        Assert(self.entered_at)==None
        self.entered_at = step
        step = step + 1
        if self.ee:
            print(f'E enter {self.name} @{step-1}')
            raise self.ee
        print(f'- enter {self.name} @{step-1}')
        return self
    
    def __exit__(self, t, e, b):
        global step
        print(f'+ exit {self.name} @ {step}')
        Assert(self.entered_at)!=None
        Assert(self.exited_at)==None
        self.exited_at = step
        step = step + 1
        if self.xe:
            print(f'E exit {self.name} @{step-1}')
            raise self.xe
        print(f'- exit {self.name} @{step-1}')
        pass

@cmc.cmclass
class B1(cmc.CM):
    a: Resource
    b: int
    def __init__(self, a:Resource, b:int):
        self.a=a
        self.b=b
        pass
    pass

@cmc.cmclass
class B2(cmc.CM, B1):
    c: str
    d: Resource
    e: Resource

    def __init__(self, c:str, d:Resource, e:Resource, a:Resource, b:int):
        B1.__init__(self,a,b)
        self.c=c
        self.d=d
        self.e=e
        pass
    pass

@cmc.cmclass
class DD(B2, B1):  # multiple inherited B1, dissallow
    f: Resource

    def __init__(self,f:Resource,c:str,d:Resource,e:Resource,a:Resource,b:int):
        B2.__init__(self,c,d,e,a,b)
        B1.__init__(self,a,b)
        self.f=f
        pass
    pass

