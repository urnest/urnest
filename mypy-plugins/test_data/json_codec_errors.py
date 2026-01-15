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
codec(dict[list[int],str]) # invalid key type
codec(dict[list,str]) # invalid key type
codec(dict[set[str],str]) # invalid key type
codec(dict[set,str]) # invalid key type
codec(dict[tuple[int,str],str]) # invalid key type
codec(dict[dict[int,str],str]) # invalid key type
codec(dict[dict,str]) # invalid key type
codec(dict[bytes,str]) # invalid key type
from dataclasses import dataclass
@dataclass
class FullName:
    first_name: str
    middle_names: list[str]
    last_name: str
    _class: Literal["Upper", "Middle", "Lower"]
    pass
codec(dict[FullName,str]) # invalid key type

from typing import NewType
FullerName = NewType("FullerName", FullName)
codec(dict[FullerName,int])  # invalid dict key

from xju.json_codec import JsonType,TypeScriptSourceCode,TypeScriptNamespace,CustomClassCodec
from xju.json_codec import CustomStringKeyClassCodec,CustomNonStringKeyClassCodec
from xju.assert_ import Assert

import ipaddress
@dataclass
class IpAddrWithPrefix(ipaddress.IPv4Interface):
    """An IP host address in a network, e.g. 10.1.1.49/24"""
    if_str: str
    __codec=codec(str)
    def __post_init__(self):
        super().__init__(self.if_str)
    @staticmethod
    def xju_json_codec_decode(x:JsonType) -> object:
        return IpAddrWithPrefix(IpAddrWithPrefix.__codec.decode(x))
    @staticmethod
    def xju_json_codec_encode(x:object) -> JsonType:
        assert isinstance(x,IpAddrWithPrefix)
        return IpAddrWithPrefix.__codec.encode(x.if_str)
    @staticmethod
    def xju_json_codec_get_json_schema(definitions:dict[str,dict]) -> dict:
        '''return json schema for T'''
        '''- may add any supporting definitions to definitions'''
        return IpAddrWithPrefix.__codec.get_json_schema()
    @staticmethod
    def xju_json_codec_get_typescript_type() -> TypeScriptSourceCode:
        return TypeScriptSourceCode('string /* IpV4AddrWithPrefix */')
    @staticmethod
    def xju_json_codec_5_get_typescript_isa(
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''return typescript source that evaluates to a xju.json_codec.IsInstance
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        return TypeScriptSourceCode('''xju.json_codec.isInstanceOfString''')
    @staticmethod
    def xju_json_codec_5_get_typescript_asa(
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''return typescript source code that evaluates to a xju.json_codec.AsInstance
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        return TypeScriptSourceCode('''xju.json_codec.asInstanceOfString("IpAddrWithPrefix")''')
    pass

Assert(IpAddrWithPrefix).isSubclassOf(CustomClassCodec)
assert not issubclass(IpAddrWithPrefix, CustomStringKeyClassCodec)
assert not issubclass(IpAddrWithPrefix, CustomNonStringKeyClassCodec)

codec(dict[IpAddrWithPrefix,int])  # invalid dict key

type PL[T,U] = list[tuple[T,U]]
codec(PL[int,str])  # ok
codec(PL)  # unresolved type vars
