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
from typing import TypeVar, Generic, Type, cast, Any, Protocol, Self, Callable, get_type_hints
from typing import _LiteralGenericAlias  # type: ignore  # mypy 1.1.1
from typing import _UnionGenericAlias  # type: ignore  # mypy 1.1.1
from types import GenericAlias, UnionType, NoneType
import xju.newtype

T=TypeVar('T')

JsonType = None|bool|dict|list|float|str


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
        return self.codec.encode(x,None)

    def decode(self,x:JsonType) -> T:
        'decode {x} to a {self.t}'
        try:
            return cast(T, self.codec.decode(x,None))
        except Exception:
            raise in_function_context(Codec.decode,vars()) from None
        pass

    def get_json_schema(self) -> dict:
        definitions: dict[str,dict] = {}
        result = self.codec.get_json_schema(definitions, None)
        result.update({
            "$id": "https://example.com/address.schema.json",
            "$schema": "https://json-schema.org/draft/2020-12/schema"})
        result.update({
            'definitions':definitions
        })
        return result
    pass

class CodecProto(Protocol):
    def encode(self,x:Any,back_ref:None|Callable[[Any],JsonType])->JsonType:
        pass
    def decode(self,x:JsonType,back_ref:None|Callable[[JsonType],Any])->Any:
        pass
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
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
    def encode(self, x:Atom, _:None|Callable[[Any],JsonType])->Atom:
        if type(x) is not self.t and not (
                self.t is float and type(x) is int):
            raise Exception(f'{x!r} is not a {self.t}')
        return x
    def decode(self, x:Atom,_:None|Callable[[JsonType],Any])->Atom:
        if type(x) is not self.t and not (
                self.t is float and type(x) is int):
            t=type(x)
            raise Exception(f'{x!r} (of type {t}) is not a {self.t}')
        return x
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        st: str
        if self.t is int: st='integer'
        if self.t is str: st='string'
        if self.t is float: st='number'
        if self.t is bool: st='boolean'
        if self.t is None: st='null'
        return { 'type': st }
    pass

class NoneCodec:
    def encode(self, x:None,_:None|Callable[[Any],JsonType])->None:
        if x is not None:
            raise Exception(f'{x!r} is not None')
        return x
    def decode(self, x:None,_:None|Callable[[JsonType],Any])->None:
        if x is not None:
            raise Exception(f'{x!r} is not None')
        return x
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return { 'type': 'null' }
    pass

class ListCodec:
    def __init__(self, value_codec:CodecProto):
        self.value_codec=value_codec
        pass
    def encode(self,x,back_ref:None|Callable[[Any],JsonType]):
        if type(x) is not list:
            raise Exception(f'{x!r} is not a list')
        return [self.value_codec.encode(_,back_ref) for _ in x]
    def decode(self,x,back_ref:None|Callable[[JsonType],Any]):
        if type(x) is not list:
            raise Exception(f'{x!r} is not a list')
        return [self.value_codec.decode(_,back_ref) for _ in x]
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            "type": "array",
            "items": self.value_codec.get_json_schema(definitions, self_ref)
        }
    pass

class AnyListCodec:
    def __init__(self):
        self.value_codec=AnyJsonCodec()
        pass
    def encode(self,x,back_ref:None|Callable[[Any],JsonType]):
        if type(x) is not list:
            raise Exception(f'{x!r} is not a list')
        return [self.value_codec.encode(_,back_ref) for _ in x]
    def decode(self,x,back_ref:None|Callable[[JsonType],Any]):
        if type(x) is not list:
            raise Exception(f'{x!r} is not a list')
        return [self.value_codec.decode(_,back_ref) for _ in x]
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            "type": "array"
        }
    pass

class TupleCodec:
    def __init__(self,value_codecs):
        self.value_codecs=value_codecs
        self.number_of_codecs=len(value_codecs)
        pass
    def encode(self,x,back_ref:None|Callable[[Any],JsonType]) -> list:
        'encode tuple {x} as a {self.number_of_codecs}-element list'
        try:
            if type(x) is not tuple:
                raise Exception(f'{x!r} is not a tuple')
            if len(x) != self.number_of_codecs:
                l=len(x)
                raise Exception(f'{x} does not have {self.number_of_codecs} items (it has {l} items)')
            return [c.encode(v,back_ref) for c,v in zip(self.value_codecs,x)]
        except Exception:
            raise in_function_context(TupleCodec.encode,vars()) from None
        pass
    def decode(self,x,back_ref:None|Callable[[JsonType],Any]) -> tuple:
        try:
            if type(x) is not list:
                raise Exception(f'{x!r} is not a list')
            if len(x) != self.number_of_codecs:
                l=len(x)
                raise Exception(f'{x} does not have {self.number_of_codecs} items (it has {l} items)')
            result=[c.decode(v,back_ref) for c,v in zip(self.value_codecs,x)]
            return tuple(result)
        except Exception:
            raise in_function_context(TupleCodec.decode,vars()) from None
        pass
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            "type": "array",
            "prefixItems": [
                codec.get_json_schema(definitions, self_ref) for codec in self.value_codecs
            ]
        }
    pass

class UnionCodec:
    def __init__(self,allowed_types):
        self.allowed_types=allowed_types
        self.value_codecs={t:_explodeSchema(t) for t in allowed_types}
        pass
    def encode(self,x,back_ref:None|Callable[[Any],JsonType]) -> JsonType:
        'encode {x!r} as one of {self.allowed_types}'
        try:
            exceptions=[]
            for t, c in self.value_codecs.items():
                try:
                    return c.encode(x,back_ref)
                except Exception as e:
                    exceptions.append( (t, e) )
                    pass
                pass
            raise Exception(' and '.join([f'failed to encode as {t} because {e}' for t,e in exceptions]))
                
        except Exception:
            raise in_function_context(UnionCodec.encode,vars()) from None
        pass
    def decode(self,x,back_ref:None|Callable[[JsonType],Any]) -> tuple:
        try:
            exceptions=[]
            for t, c in self.value_codecs.items():
                try:
                    return c.decode(x,back_ref)
                except Exception as e:
                    exceptions.append( (t, e) )
                    pass
                pass
            raise Exception(' and '.join([f'failed to decode as {t} because {e}' for t,e in exceptions]))
        except Exception:
            raise in_function_context(UnionCodec.decode,vars()) from None
        pass
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            "oneOf": [ codec.get_json_schema(definitions, self_ref) for codec in self.value_codecs.values() ]
        }
    pass

class DictCodec:
    def __init__(self, key_codec, value_codec):
        self.key_codec=key_codec
        self.value_codec=value_codec
    def encode(self,x:dict,back_ref:None|Callable[[Any],JsonType])->dict:
        if type(x) is not dict:
            raise Exception(f'{x!r} is not a dict')
        result:dict={}
        for k,v in x.items():
            ek,ev=self.key_codec.encode(k,back_ref),self.value_codec.encode(v,back_ref)
            if not isinstance(ek, str):
                raise Exception(f'key encoder {self.key_codec} produced non-str {ek} from dict key {k}')
            result[ek]=ev
            pass
        return result
    def decode(self,x,back_ref:None|Callable[[JsonType],Any])->dict:
        if type(x) is not dict:
            raise Exception(f'{x!r} is not a dict')
        return {self.key_codec.decode(k,back_ref):self.value_codec.decode(v,back_ref)
                for k,v in x.items()}
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            'type': 'object',
            'additionalProperties': self.value_codec.get_json_schema(definitions, self_ref)
        }
    pass

class AnyDictCodec:
    def __init__(self):
        self.key_codec=NoopCodec[str](str)
        self.value_codec=AnyJsonCodec()
    def encode(self,x:dict,back_ref:None|Callable[[Any],JsonType])->dict:
        if type(x) is not dict:
            raise Exception(f'{x!r} is not a dict')
        result:dict={}
        for k,v in x.items():
            ek,ev=self.key_codec.encode(k,back_ref),self.value_codec.encode(v,back_ref)
            result[ek]=ev
            pass
        return result
    def decode(self,x,back_ref:None|Callable[[JsonType],Any])->dict:
        if type(x) is not dict:
            raise Exception(f'{x!r} is not a dict')
        return {self.key_codec.decode(k,back_ref):self.value_codec.decode(v,back_ref) for k,v in x.items()}
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            'type': 'object'
        }
    pass

class AnyJsonCodec:
    def encode(self,x,_:None|Callable[[Any],JsonType]):
        # we assume x will be subsequently json.dumps()ed so defer validation to that
        return x
    def decode(self,x,_:None|Callable[[JsonType],Any]):
        # we assume x was json.loads()ed so assume it is appropriate
        return x
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            "oneOf": [ { 'type': t } for t in ['null','boolean','object','array','number','string'] ]
        }
    pass

class NewIntCodec(Generic[NewInt]):
    t:Type[NewInt]
    def __init__(self,t:Type[NewInt]):
        self.t=t
        self.base_codec=NoopCodec[int](int)
    def encode(self, x:NewInt,back_ref:None|Callable[[Any],JsonType])->int:
        if not isinstance(x, xju.newtype.Int):
            raise Exception(f'{x!r} is not a {self.t}')
        return self.base_codec.encode(x.value(),back_ref)
    def decode(self, x:JsonType,back_ref:None|Callable[[JsonType],Any])->NewInt:
        if not isinstance(x, int):
            t=type(x)
            raise Exception(f'{x!r} (of type {t}) is not an int')
        return self.t(self.base_codec.decode(x,back_ref))
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            'description': self.t.__name__,
            'type': 'integer'
        }
    pass

class NewFloatCodec(Generic[NewFloat]):
    t:Type[NewFloat]
    def __init__(self,t:Type[NewFloat]):
        self.t=t
        self.base_codec=NoopCodec[float](float)
    def encode(self, x:NewFloat,back_ref:None|Callable[[Any],JsonType])->float:
        if not isinstance(x, xju.newtype.Float):
            raise Exception(f'{x!r} is not a {self.t}')
        return self.base_codec.encode(x.value(),back_ref)
    def decode(self, x:JsonType,back_ref:None|Callable[[JsonType],Any])->NewFloat:
        if type(x) is not float and type(x) is not int:
            t=type(x)
            raise Exception(f'{x!r} (of type {t}) is not a float (or an int)')
        return self.t(self.base_codec.decode(x,back_ref))
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            'description': self.t.__name__,
            'type': 'number'
        }
    pass

class NewStrCodec(Generic[NewStr]):
    t:Type[NewStr]
    def __init__(self,t:Type[NewStr]):
        self.t=t
        self.base_codec=NoopCodec[str](str)
    def encode(self, x:NewStr,back_ref:None|Callable[[Any],JsonType])->str:
        if not isinstance(x, xju.newtype.Str):
            raise Exception(f'{x!r} is not a {self.t}')
        return self.base_codec.encode(x.value(),back_ref)
    def decode(self, x:JsonType,back_ref:None|Callable[[JsonType],Any])->NewStr:
        if type(x) is not str:
            t=type(x)
            raise Exception(f'{x!r} (of type {t}) is not an str')
        return self.t(self.base_codec.decode(x,back_ref))
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            'description': self.t.__name__,
            'type': 'string'
        }
    pass

class LiteralStrCodec:
    value:str
    def __init__(self,value:str):
        self.value=value
    def encode(self, x:str, _:None|Callable[[Any],JsonType])->str:
        if type(x) is not str or x != self.value:
            raise Exception(f'{x!r} is not {self.value!r}')
        return x
    def decode(self, x:JsonType,_:None|Callable[[JsonType],Any])->str:
        if type(x) is not str:
            raise Exception(f'{x!r} is not a string')
        if x != self.value:
            raise Exception(f'{x!r} is not {self.value!r}')
        return x
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            'type': 'string',
            'enum': [ self.value ]
        }
    pass

class SelfCodec:
    def encode(self,x,back_ref:None|Callable[[Any],JsonType]) -> JsonType:
        'encode {x} as a Self'
        try:
            assert back_ref is not None
            return back_ref(x)
        except Exception:
            raise in_function_context(SelfCodec.encode,vars()) from None
        pass
    def decode(self,x,back_ref:None|Callable[[JsonType],Any]) -> object:
        'deocde {x} as a Self'
        try:
            assert back_ref is not None
            return back_ref(x)
        except Exception:
            raise in_function_context(SelfCodec.decode,vars()) from None
        pass
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            '$ref': self_ref
        }
    pass
    
class ClassCodec:
    t:type
    attr_codecs:dict[str,Any]  # codec
    def __init__(self, t:type, attr_codecs:dict[str,Any]):
        self.t=t
        self.attr_codecs=attr_codecs
        pass
    def encode(self,x,_:None|Callable[[Any],JsonType]) -> dict:
        'encode {x} as a {self.t}'
        try:
            if type(x) is not self.t:
                xt=type(x)
                raise Exception(f'{x!r} (of type {xt}) is not a {self.t}')
            def back_ref(x:Any) -> JsonType:
                return self.encode(x,None)
            result={}
            for n, attr_codec in self.attr_codecs.items():
                try:
                    result[n]=attr_codec.encode(getattr(x,n),back_ref)
                except Exception:
                    raise in_context(f'encode attribute {n}') from None
                pass
            return result
        except Exception:
            raise in_function_context(ClassCodec.encode,vars()) from None
        pass
    def decode(self,x,_:None|Callable[[JsonType],Any]) -> object:
        'deocde {x} as a {self.t}'
        try:
            def back_ref(x:JsonType) -> object:
                return self.decode(x,None)
            attr_values=[]
            for n, attr_codec in self.attr_codecs.items():
                try:
                    if n not in x:
                        raise Exception(f'{x!r} has no {n!r} attribute')
                    value=attr_codec.decode(x[n],back_ref)
                    attr_values.append(value)
                except Exception:
                    raise in_context(f'decode attribute {n}') from None
                pass
            try:
                return self.t(*attr_values)
            except Exception:
                raise in_context(f'init {self.t} with positional parameters {attr_values}') from None
        except Exception:
            raise in_function_context(ClassCodec.decode,vars()) from None
        pass
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        fqn=f'{self.t.__module__}.{self.t.__name__}'
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
                'description': self.t.__name__,
                'type': 'object',
                'properties': {
                    n: attr_codec.get_json_schema(definitions, self_ref)
                    for n, attr_codec in self.attr_codecs.items()
                }
            }
            pass
        return {
            '$ref': self_ref
        }
    pass

def _explodeSchema(t:type):
    '''explode type {t!r} into a tree of codecs'''
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
            return AnyListCodec()
        if type(t) is GenericAlias and getattr(t, '__origin__') is list:
            return ListCodec(_explodeSchema(getattr(t,'__args__')[0]))
        if type(t) is _LiteralGenericAlias:
            value = t.__args__[0]
            if not isinstance(value,str):
                raise Exception(f'{t!r} literal type is not supported (only string is implemented)')
            return LiteralStrCodec(value)
        if type(t) is GenericAlias and getattr(t, '__origin__') is tuple:
            return TupleCodec([_explodeSchema(_) for _ in getattr(t,'__args__')])
        if t is dict:
            return AnyDictCodec()
        if type(t) is GenericAlias and getattr(t, '__origin__') is dict:
            return DictCodec(*[_explodeSchema(_) for _ in getattr(t,'__args__')])
        if type(t) is UnionType:
            return UnionCodec(getattr(t,'__args__'))
        if type(t) is _UnionGenericAlias:
            return UnionCodec(getattr(t,'__args__'))
        if t is Self:
            return SelfCodec()
        if issubclass(t,xju.newtype.Int):
            return NewIntCodec(t)
        if issubclass(t,xju.newtype.Float):
            return NewFloatCodec(t)
        if issubclass(t,xju.newtype.Str):
            return NewStrCodec(t)
        return ClassCodec(
            t,{n: _explodeSchema(nt) for n,nt in get_type_hints(t).items()})
    except Exception:
        raise in_function_context(_explodeSchema,vars()) from None
    pass
