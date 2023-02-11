# coding: utf-8
#
# Copyright (c) 2023 Trevor Taylor
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
# json schema represented as a python type var, e.g.:
#
# @dataclasses.dataclass
# class Address:
#  number: str|int
#  street: str
#
# Codec[dict[str,int|Address]].decode({'fred':3, 'jock':{'number':32, 'street': 'asler'}})
#
# For examples see json_codec.py.test
#

from xju.xn import Xn,in_context,in_function_context,readable_repr
from typing import TypeVar, Generic, Type, cast, Any, Protocol
from typing import _LiteralGenericAlias  # type: ignore
from types import GenericAlias, UnionType, NoneType
import xju.newtype

T=TypeVar('T')

JsonType = list|dict|str|float|None


class Codec(Generic[T]):
    t:Type[T]
    def __init__(self, t: Type[T]):
        'initialse json decoder for type %(t)r'
        self.t=t
        self.codec:CodecProto=_explodeSchema(self.t)
        pass

    def __repr__(self):
        return f'{self.t!r} json codec'

    def encode(self,x:T) -> JsonType:
        'encode {self.t} {x} to json'
        return self.codec.encode(x)

    def decode(self,x:JsonType) -> T:
        'decode {x} to a {self.t}'
        try:
            return cast(T, self.codec.decode(x))
        except Exception:
            raise in_function_context(Codec.decode,vars()) from None
        pass

class CodecProto(Protocol):
    def encode(self, x:Any)->JsonType:
        pass
    def decode(self, x:JsonType)->Any:
        pass
    pass

Atom=TypeVar('Atom',int,str,bool,float,None)

NewInt=TypeVar('NewInt',bound=xju.newtype.Int)
NewFloat=TypeVar('NewFloat',bound=xju.newtype.Float)
NewStr=TypeVar('NewStr',bound=xju.newtype.Str)

class NoopCodec(Generic[Atom]):
    t:Type[Atom]
    def __init__(self, t:Type[Atom]):
        self.t=t
    def encode(self, x:Atom)->Atom:
        if not isinstance(x,self.t) and not (
                self.t is float and isinstance(x, int)):
            raise Exception(f'{x!r} is not a {self.t}')
        return x
    def decode(self, x:Atom)->Atom:
        if not isinstance(x, self.t) and not (
                self.t is float and isinstance(x, int)):
            t=type(x)
            raise Exception(f'{x!r} (of type {t}) is not a {self.t}')
        return x
    pass

class NoneCodec:
    def encode(self, x:None)->None:
        if x is not None:
            raise Exception(f'{x!r} is not None')
        return x
    def decode(self, x:None)->None:
        if x is not None:
            raise Exception(f'{x!r} is not None')
        return x
    pass

class ListCodec:
    def __init__(self, value_codec):
        self.value_codec=value_codec
    def encode(self,x):
        return [self.value_codec.encode(_) for _ in x]
    def decode(self,x):
        return [self.value_codec.decode(_) for _ in x]
    pass

class AnyTupleCodec:
    def __init__(self):
        self.value_codec=AnyJsonCodec()
    def encode(self,x) -> list:
        return [self.value_codec.encode(_) for _ in x]
    def decode(self,x) -> tuple:
        return tuple([self.value_codec.decode(_) for _ in x])
    pass

class TupleCodec:
    def __init__(self,value_codecs):
        self.value_codecs=value_codecs
        self.number_of_codecs=len(value_codecs)
        pass
    def encode(self,x) -> list:
        'encode tuple {x} as a {self.number_of_codecs}-element list'
        try:
            if len(x) != self.number_of_codecs:
                l=len(x)
                raise Exception(f'{x} does not have {self.number_of_codecs} items (it has {l} items)')
            return [c.encode(v) for c,v in zip(self.value_codecs,x)]
        except Exception:
            raise in_function_context(TupleCodec.encode,vars()) from None
        pass
    def decode(self,x) -> tuple:
        try:
            if len(x) != self.number_of_codecs:
                l=len(x)
                raise Exception(f'{x} does not have {self.number_of_codecs} items (it has {l} items)')
            result=[c.decode(v) for c,v in zip(self.value_codecs,x)]
            return tuple(result)
        except Exception:
            raise in_function_context(TupleCodec.decode,vars()) from None
        pass
    pass

class UnionCodec:
    def __init__(self,allowed_types):
        self.allowed_types=allowed_types
        self.value_codecs={t:_explodeSchema(t) for t in allowed_types}
        pass
    def encode(self,x) -> JsonType:
        'encode {x!r} as one of {self.allowed_types}'
        try:
            exceptions=[]
            for t, c in self.value_codecs.items():
                try:
                    return c.encode(x)
                except Exception as e:
                    exceptions.append( (t, e) )
                    pass
                pass
            raise Exception(' and '.join([f'failed to encode as {t} because {e}' for t,e in exceptions]))
                
        except Exception:
            raise in_function_context(UnionCodec.encode,vars()) from None
        pass
    def decode(self,x) -> tuple:
        try:
            exceptions=[]
            for t, c in self.value_codecs.items():
                try:
                    return c.decode(x)
                except Exception as e:
                    exceptions.append( (t, e) )
                    pass
                pass
            raise Exception(' and '.join([f'failed to decode as {t} because {e}' for t,e in exceptions]))
        except Exception:
            raise in_function_context(UnionCodec.decode,vars()) from None
        pass
    pass

class DictCodec:
    def __init__(self, key_codec, value_codec):
        self.key_codec=key_codec
        self.value_codec=value_codec
    def encode(self,x:dict)->dict:
        result:dict={}
        for k,v in x.items():
            ek,ev=self.key_codec.encode(k),self.value_codec.encode(v)
            if not isinstance(ek, str):
                raise Exception(f'key encoder {self.key_codec} produced non-str {ek} from dict key {k}')
            result[ek]=ev
            pass
        return result
    def decode(self,x)->dict:
        assert isinstance(x,dict), type(x)
        return {self.key_codec.decode(k):self.value_codec.decode(v) for k,v in x.items()}
    pass

class AnyJsonCodec:
    def encode(self,x):
        # we assume x will be subsequently json.dumps()ed so defer validation to that
        return x
    def decode(self,x):
        # we assume x was json.loads()ed so assume it is appropriate
        return x
    pass

class NewIntCodec(Generic[NewInt]):
    t:Type[NewInt]
    def __init__(self,t:Type[NewInt]):
        self.t=t
        self.base_codec=NoopCodec[int](int)
    def encode(self, x:NewInt)->int:
        if not isinstance(x, xju.newtype.Int):
            raise Exception(f'{x!r} is not a {self.t}')
        return self.base_codec.encode(x.value())
    def decode(self, x:JsonType)->NewInt:
        if not isinstance(x, int):
            t=type(x)
            raise Exception(f'{x!r} (of type {t}) is not an int')
        return self.t(self.base_codec.decode(x))
    pass

class NewFloatCodec(Generic[NewFloat]):
    t:Type[NewFloat]
    def __init__(self,t:Type[NewFloat]):
        self.t=t
        self.base_codec=NoopCodec[float](float)
    def encode(self, x:NewFloat)->float:
        if not isinstance(x, xju.newtype.Float):
            raise Exception(f'{x!r} is not a {self.t}')
        return self.base_codec.encode(x.value())
    def decode(self, x:JsonType)->NewFloat:
        if not isinstance(x, float) and not isinstance(x, int):
            t=type(x)
            raise Exception(f'{x!r} (of type {t}) is not an float (or int)')
        return self.t(self.base_codec.decode(x))
    pass

class NewStrCodec(Generic[NewStr]):
    t:Type[NewStr]
    def __init__(self,t:Type[NewStr]):
        self.t=t
        self.base_codec=NoopCodec[str](str)
    def encode(self, x:NewStr)->str:
        if not isinstance(x, xju.newtype.Str):
            raise Exception(f'{x!r} is not a {self.t}')
        return self.base_codec.encode(x.value())
    def decode(self, x:JsonType)->NewStr:
        if not isinstance(x, str):
            t=type(x)
            raise Exception(f'{x!r} (of type {t}) is not an str')
        return self.t(self.base_codec.decode(x))
    pass

class LiteralStrCodec:
    value:str
    def __init__(self,value:str):
        self.value=value
    def encode(self, x:str)->str:
        if x != self.value:
            raise Exception(f'{x!r} is not {self.value!r}')
        return x
    def decode(self, x:JsonType)->str:
        if not isinstance(x,str):
            raise Exception(f'{x!r} is not a string')
        if x != self.value:
            raise Exception(f'{x!r} is not {self.value!r}')
        return x
    pass

class ClassCodec:
    t:type
    attr_codecs:dict[str,Any]  # codec
    def __init__(self, t:type, attr_codecs:dict[str,Any]):
        self.t=t
        self.attr_codecs=attr_codecs
        pass
    def encode(self,x) -> dict:
        'encode {x} as a {self.t}'
        try:
            if not isinstance(x,self.t):
                xt=type(x)
                raise Exception(f'{x!r} (of type {xt}) is not a {self.t}')
            result={}
            for n, attr_codec in self.attr_codecs.items():
                try:
                    result[n]=attr_codec.encode(getattr(x,n))
                except Exception:
                    raise in_context(f'encode attribute {n}') from None
                pass
            return result
        except Exception:
            raise in_function_context(ClassCodec.encode,vars()) from None
        pass
    def decode(self,x) -> object:
        'deocde {x} as a {self.t}'
        try:
            attr_values=[]
            for n, attr_codec in self.attr_codecs.items():
                try:
                    if n not in x:
                        raise Exception(f'{x!r} has no {n!r} attribute')
                    value=attr_codec.decode(x[n])
                    if not isinstance(attr_codec,LiteralStrCodec):
                        attr_values.append(value)
                        pass
                except Exception:
                    raise in_context(f'decode attribute {n}') from None
                pass
            return self.t(*attr_values)
        except Exception:
            raise in_function_context(ClassCodec.decode,vars()) from None
        pass
    pass
    
def _explodeSchema(t:type):
    '''explode type {t} into a tree of codecs'''
    try:
        if t is float:
            return NoopCodec[float](float)
        if t is int:
            return NoopCodec[int](int)
        if t is str:
            return NoopCodec[str](str)
        if t is bool:
            return NoopCodec[bool](bool)
        if t is None or t is NoneType:
            return NoneCodec()
        if t is list:
            return ListCodec(AnyJsonCodec())
        if type(t) is GenericAlias and getattr(t, '__origin__') is list:
            return ListCodec(_explodeSchema(getattr(t,'__args__')[0]))
        if t is tuple:
            return AnyTupleCodec()
        if type(t) is _LiteralGenericAlias:
            value = t.__args__[0]
            if not isinstance(value,str):
                raise Exception(f'{t!r} literal type is not implemented (only string is implemented)')
            return LiteralStrCodec(value)
        if type(t) is GenericAlias and getattr(t, '__origin__') is tuple:
            return TupleCodec([_explodeSchema(_) for _ in getattr(t,'__args__')])
        if t is dict:
            return DictCodec(AnyJsonCodec(),AnyJsonCodec())
        if type(t) is GenericAlias and getattr(t, '__origin__') is dict:
            return DictCodec(*[_explodeSchema(_) for _ in getattr(t,'__args__')])
        if type(t) is UnionType:
            return UnionCodec(getattr(t,'__args__'))
        if issubclass(t,xju.newtype.Int):
            return NewIntCodec(t)
        if issubclass(t,xju.newtype.Float):
            return NewFloatCodec(t)
        if issubclass(t,xju.newtype.Str):
            return NewStrCodec(t)
        return ClassCodec(t, {n: _explodeSchema(t) for n,t in t.__annotations__.items()})
    except Exception:
        raise in_function_context(_explodeSchema,vars()) from None
    pass
