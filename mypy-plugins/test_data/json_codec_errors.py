from types import NoneType
from xju.json_codec import codec,VerifyTypeIsCustomClassCodec,VerifyTypeIsCustomGenericClassCodec,VerifyTypeIsCustomStringKeyGenericClassCodec #, _xxx
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

from typing import Literal, cast, Any
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
    def xju_json_codec_encode(x:object) -> str:
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

VerifyTypeIsCustomClassCodec(IpAddrWithPrefix)
Assert(IpAddrWithPrefix).isSubclassOf(CustomClassCodec)
assert not issubclass(IpAddrWithPrefix, CustomStringKeyClassCodec)
assert not issubclass(IpAddrWithPrefix, CustomNonStringKeyClassCodec)
codec(dict[IpAddrWithPrefix,int])  # invalid dict key

type PL[T,U] = list[tuple[T,U]]
codec(PL[int,str])  # ok
codec(PL)  # unresolved type vars

# verify json_codec_plugin calculates encode() return type correctly
# (int is not a valid json type so will never be chosen)
def is_int(x: int) -> int: return x
def is_none(x: None) -> None: return x

is_int(codec(None).encode(None))
is_int(codec(int).encode(7))
is_none(codec(bool).encode(True))
is_int(codec(float).encode(7.8))
is_int(codec(str).encode("fred"))
is_int(codec(list).encode([]))
is_int(codec(list[bool]).encode(list[bool]()))
is_int(codec(set).encode(set()))
is_int(codec(set[bool]).encode(set[bool]()))
is_int(codec(dict).encode({}))
is_int(codec(dict[bool,int]).encode(dict[bool,int]()))
is_int(codec(frozenset).encode(frozenset()))
is_int(codec(frozenset[bool]).encode(frozenset[bool]()))

is_int(codec(FullerName).encode(FullerName(FullName("fred",["jock"],"walker","Upper"))))

Milligrams = NewType('Milligrams',float)
is_int(codec(Milligrams).encode(Milligrams(7.6)))

Votes = NewType('Votes',int)
is_int(codec(Votes).encode(Votes(8)))

Enabled = NewType('Enabled',bool)
is_none(codec(Enabled).encode(Enabled(False)))

is_int(codec(bytes).encode(bytes([])))
is_int(codec(tuple[int,str]).encode( (7,'fred') ))

is_int(codec(int|str).encode(8))

is_int(codec(NS).encode(NS("fred")))
is_int(codec(NI).encode(NI(9)))
is_int(codec(NF).encode(NF(9.9)))
is_none(codec(NB).encode(NB(True)))

from xju.time import Timestamp
is_int(codec(Timestamp).encode(Timestamp(3)))

class O(Enum):
    a='fred'
    b=7
    pass

OA = O

is_int(codec(Literal[O.b]).encode(O.b))
is_int(codec(Literal[OA.a]).encode(O.a))
is_int(codec(O).encode(O.a))

@dataclass(init=False)
class MixedEnumValue:
    def __init__(self, *args, **argv):
        match len(args):
            case 0:
                self.k = argv['k']
                self.v = argv['v']
            case 1:
                self.k, self.v = args[0].k, args[0].v
            case _:
                self.k, self.v = args

    k: int
    v: str

class MixedEnum(MixedEnumValue, Enum):
    a = MixedEnumValue(1, 'fred')
    b = MixedEnumValue(2, 'jock')
    pass

is_int(codec(MixedEnum).encode(MixedEnum.a))

is_int(codec(Literal["fred"]).encode("fred"))
is_int(codec(Literal[6,'jock']).encode(6))

@dataclass
class Address:
    street:'Street'
    suburb:'Suburb'
    postcode:'Postcode'
    _class: Literal["first", "second"]
    pass

class StreetTag:pass
class Street(Str[StreetTag]):pass

class SuburbTag:pass
class Suburb(Str[SuburbTag]):pass

class PostcodeTag:pass
class Postcode(Int[PostcodeTag]):pass

address=Address(Street('alba'),Suburb('bocca'),Postcode(3365),'second')
is_int(codec(Address).encode(address)) # dict
ip=IpAddrWithPrefix('10.1.1.49/24')
is_int(codec(IpAddrWithPrefix).encode(ip))  # str (return type of IpAddrWithPrefix.xju_json_codec_encode)

from typing import Protocol,TypeVar
from xju.json_codec import CodecProto

class ShapeOfT(Protocol):
    x: str

@dataclass(kw_only=True)
class G[C, T: ShapeOfT]:
    c: type[C]
    v: T

    @staticmethod
    def get_codec(type_var_map:dict[TypeVar,type]) -> CodecProto:
        assert type_var_map is not None
        t_var=G.__type_params__[1]
        assert isinstance(t_var,TypeVar)
        assert type_var_map[t_var].__annotations__['x'] is str
        return codec(str)

    # xju.json_codec.CustomGenericClassCodec:
    @staticmethod
    def xju_json_codec_encode_generic(
            x:object,
            type_var_map:dict[TypeVar,type]) -> str:
        """only encodes the T.x attribute"""
        xx=cast(G[C,T],x)
        result=G.get_codec(type_var_map).encode(xx.v.x)
        assert isinstance(result,str)
        return result
    @staticmethod
    def xju_json_codec_decode_generic(
            x:JsonType,
            type_var_map:dict[TypeVar,type]) -> "G[C,T]":
        v_x=G.get_codec(type_var_map).decode(x)
        c_var=G.__type_params__[0]
        assert isinstance(c_var,TypeVar)
        t_var=G.__type_params__[1]
        assert isinstance(t_var,TypeVar)
        return G(c=cast(type[C],type_var_map[c_var]),
                 v=type_var_map[t_var](x=v_x))
    @staticmethod
    def xju_json_codec_get_json_schema_generic(
            definitions:dict[str,dict],
            type_var_map:dict[TypeVar,type]) -> dict:
        '''return json schema for T'''
        '''- may add any supporting definitions to definitions'''
        return G.get_codec(type_var_map).get_json_schema()
    @staticmethod
    def xju_json_codec_get_typescript_type_generic(
            type_var_map:dict[TypeVar,type]) -> TypeScriptSourceCode:
        kt=f"typeof {G.__type_params__[0]}"
        vt=f"{G.__type_params__[1]}"
        return TypeScriptSourceCode(f"string /* G<{kt},{vt} */");
    @staticmethod
    def xju_json_codec_5_get_typescript_isa_generic(
            namespace: TypeScriptNamespace,
            type_var_map:dict[TypeVar,type]) -> TypeScriptSourceCode:
        '''return typescript source that evaluates to a xju.json_codec.IsInstance
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        return TypeScriptSourceCode("xju.json_codec.isInstanceOfString;");
    @staticmethod
    def xju_json_codec_5_get_typescript_asa_generic(
            namespace: TypeScriptNamespace,
            type_var_map:dict[TypeVar,type]) -> TypeScriptSourceCode:
        '''return typescript source code that verifies {expression} with defaults applied would be a T and returns xju.json_codec.ApplyDefaults that would apply defaults, throwing an Error if {expression} is not valid as a T'''
        '''- may add any supporting definitions to namespace, e.g. type for T itself'''
        kt=f"typeof {G.__type_params__[0]}"
        vt=f"{G.__type_params__[1]}"
        return TypeScriptSourceCode(f"xju.json_codec.asInstanceOfString('G<{kt},{vt}>'");
    pass

VerifyTypeIsCustomGenericClassCodec(G)

@dataclass(kw_only=True)
class GT:
    x: str
    pass


is_int(codec(G[int,GT]).encode(G(c=int, v=GT(x='fred'))))  # str (return type of G.xju_json_codec_encode_generic)


@dataclass(kw_only=True)
class GK[C, T: ShapeOfT]:
    c: type[C]
    v: T

    @staticmethod
    def get_codec(type_var_map:dict[TypeVar,Any]) -> CodecProto:
        assert type_var_map is not None
        t_var=GK.__type_params__[1]
        assert isinstance(t_var,TypeVar)
        assert type_var_map[t_var].__annotations__['x'] is str
        return codec(str)

    # xju.json_codec.CustomGenericClassCodec:
    @staticmethod
    def xju_json_codec_encode_generic(
            x:object,
            type_var_map:dict[TypeVar,type]) -> str:
        """only encodes the T.x attribute"""
        xx=cast(GK[C,T],x)
        result=GK.get_codec(type_var_map).encode(xx.v.x)
        assert isinstance(result,str)
        return result
    @staticmethod
    def xju_json_codec_decode_generic(
            x:JsonType,
            type_var_map:dict[TypeVar,type]) -> "GK[C,T]":
        v_x=GK.get_codec(type_var_map).decode(x)
        c_var=GK.__type_params__[0]
        assert isinstance(c_var,TypeVar)
        t_var=GK.__type_params__[1]
        assert isinstance(t_var,TypeVar)
        return GK(c=cast(type[C],type_var_map[c_var]),
                  v=type_var_map[t_var](x=v_x))
    @staticmethod
    def xju_json_codec_get_json_schema_generic(
            definitions:dict[str,dict],
            type_var_map:dict[TypeVar,type]) -> dict:
        '''return json schema for T'''
        '''- may add any supporting definitions to definitions'''
        return GK.get_codec(type_var_map).get_json_schema()
    @staticmethod
    def xju_json_codec_get_typescript_type_generic(
            type_var_map:dict[TypeVar,type]) -> TypeScriptSourceCode:
        kt=f"typeof {G.__type_params__[0]}"
        vt=f"{G.__type_params__[1]}"
        return TypeScriptSourceCode(f"string /* GK<{kt},{vt}> */");
    @staticmethod
    def xju_json_codec_5_get_typescript_isa_generic(
            namespace: TypeScriptNamespace,
            type_var_map:dict[TypeVar,type]) -> TypeScriptSourceCode:
        '''return typescript source that evaluates to a xju.json_codec.IsInstance
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        return TypeScriptSourceCode("xju.json_codec.isInstanceOfString;");
    @staticmethod
    def xju_json_codec_5_get_typescript_asa_generic(
            namespace: TypeScriptNamespace,
            type_var_map:dict[TypeVar,type]) -> TypeScriptSourceCode:
        '''return typescript source code that verifies {expression} with defaults applied would be a T and returns xju.json_codec.ApplyDefaults that would apply defaults, throwing an Error if {expression} is not valid as a T'''
        '''- may add any supporting definitions to namespace, e.g. type for T itself'''
        kt=f"typeof {GK.__type_params__[0]}"
        vt=f"{GK.__type_params__[1]}"
        return TypeScriptSourceCode(f"xju.json_codec.asInstanceOfString('GK<{kt},{vt}>'");
    @staticmethod
    def xju_json_codec_get_object_key_json_schema_generic(
            definitions:dict[str,dict],
            type_var_map:dict[TypeVar,type]) -> dict:
        '''return json schema for T when used as an object key
           - may add any supporting definitions to definitions'''
        return GK.get_codec(type_var_map).get_json_schema()
    @staticmethod
    def xju_json_codec_typescript_key_type_generic(type_var_map:dict[TypeVar,type])-> Literal['String']:
        return 'String'

    @staticmethod
    def xju_json_codec_5_get_typescript_isa_key_generic(
            namespace: TypeScriptNamespace,
            type_var_map:dict[TypeVar,type]) -> TypeScriptSourceCode:
        '''return typescript source code that evaluates to a xju.json_codec.IsKey
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    @staticmethod
    def xju_json_codec_5_get_typescript_asa_key_generic(
            namespace: TypeScriptNamespace,
            type_var_map:dict[TypeVar,type]) -> TypeScriptSourceCode:
        '''return typescript source code that evaluates to a xju.json_codec.AsKey
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    pass

VerifyTypeIsCustomStringKeyGenericClassCodec(GK)

is_int(codec(GK[int,GT]).encode(GK(c=int, v=GT(x='fred'))))  # str (return type of G.xju_json_codec_encode_generic)
