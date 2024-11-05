from types import NoneType
from xju.json_codec import codec #, _xxx
#_xxx()
c1=codec(int)
c2=codec(bool)
c3=codec(float)
c4=codec(NoneType)
c5=codec(list)
c6=codec(tuple)
c7=codec(dict)
c8=codec(set)
c9=codec(str)
x = (c1.encode(7),
     c1.encode("fred"),
     c2.encode(True),
     c2.encode("fred"),
     c3.encode(5.6),
     c3.encode("fred"),
     c4.encode(None),
     c4.encode(7),
     c5.encode([1,2]),
     c5.encode(7),
     c7.encode({"1":2}),
     c7.encode(7),
     c8.encode({1}),
     c8.encode(7),
     c9.encode("fred"),
     c9.encode(7))
r1:int=c1.decode(None)
r1x:str=c1.decode(None)  # ensure return type not inferred as Any
r2:bool=c2.decode(None)
r2x:str=c2.decode(None)  # ensure return type not inferred as Any
r3:float=c3.decode(None)
r3x:str=c3.decode(None)  # ensure return type not inferred as Any
r4:None=c4.decode(None)
r4x:str=c4.decode(None)  # ensure return type not inferred as Any
r5:list=c5.decode(None)
r5x:str=c5.decode(None)  # ensure return type not inferred as Any
r6:tuple=c6.decode(None)
r6x:str=c6.decode(None)  # ensure return type not inferred as Any
r7:dict=c7.decode(None)
r7x:str=c7.decode(None)  # ensure return type not inferred as Any
r8:set=c8.decode(None)
r8x:str=c8.decode(None)  # ensure return type not inferred as Any
r9:str=c9.decode(None)
r9x:int=c9.decode(None)  # ensure return type not inferred as Any
codec(dict[int,str]).encode({"1":"b"})  # bad key value type
codec(dict[float,str]).encode({'a':'b'})  # bad key value type
codec(dict[bool,str]).encode({1:'b'})  # bad key value type
codec(dict[None,str]).encode({1:'b'})  # bad key value type
codec(dict[tuple[int,str],str]).encode({(1,'a'):'b'})  # key type not allowed
codec(dict[int|str,str]).encode({(1,'a'):'b'})  # key type not allowed (mixes str with other key types)
codec(dict[int|bool,str]).encode({1:'b', False: 'c'})  # ok
codec(dict[int|bool,str]).encode({1:'b', False: 'c'})  # ok
from xju.newtype import Str,Int,Bool,Float
class NS(Str[Str]):pass
class NI(Int[Int]):pass
class NF(Float[Float]):pass
class NB(Bool[Bool]):pass
codec(dict[NS,int]).encode({NS('a'):2})  # ok
codec(dict[NI,int]).encode({NI(7):2})  # ok
codec(dict[NF,int]).encode({NF(7.2):2})  # ok
codec(dict[NB,int]).encode({NB(True):2})  # ok
codec(dict[NI|NF|NB|None,int]).encode({NI(2):3})  # ok
codec(dict[NS|NI|NF|NB|None,int]).encode({NI(2):3})  # can't mix string and non-string dict keys
X=int|bool
codec(dict[X|None,int])  # ok
codec(dict[X|str,int]) # can't mix...

from enum import Enum
class Pnum(Enum):
    a=7
    b='fred'
    pass

codec(dict[Pnum,int])  # can't mix

from typing import Literal
codec(dict[Literal[Pnum.a]|int,int])  # ok
codec(dict[Literal[Pnum.b]|int,int])  # can't mix
codec(dict[Literal[Pnum.b,Pnum.a],int])  # can't mix
