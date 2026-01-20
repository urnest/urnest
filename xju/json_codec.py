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
#   @dataclasses.dataclass
#   class Address:
#     number: str|int
#     street: str
#
#   codec(Address).decode({'number':32, 'street': 'asler'})==Address(32,'asler')
#   codec(Address).decode({'number':'42 biz', 'street': 'asler'})==Address('42 biz','asler')
#   codec(Address).encode(Address(32,'asler'))=={'number':32, 'street': 'asler'}
#
# For examples see json_codec.py.test
#
import builtins
import sys
from dataclasses import dataclass, Field, MISSING
from inspect import getmro
from typing import NewType,NamedTuple
from xju.time import Timestamp
from xju.xn import Xn,in_context,in_function_context,readable_repr, AllFailed
from typing import TypeVar, Generic, Type, cast, Any, Protocol, Self, Callable, get_type_hints
from typing import Sequence, Literal, NewType, Final, Mapping
from typing import _LiteralGenericAlias  # type: ignore  # mypy 1.1.1
from typing import _UnionGenericAlias  # type: ignore  # mypy 1.1.1
from typing import _GenericAlias,_SpecialForm  # type: ignore  # mypy 1.2.0
from typing import get_origin,get_args, runtime_checkable, TypeAliasType
from types import GenericAlias, UnionType, NoneType
import xju.newtype
from enum import Enum, EnumType, EnumMeta
import json
from xju import misc
from xju.assert_ import Assert

# for development of mypy plugin, to see what return type should be for a case
# see json_codec_mypy_plugin.show_return_type
# def _xxx(x: Type[tuple[str, ...]]) -> tuple[str, ...] | None:
#     return None

#_xxx(tuple[str, ...])

# (Pdb) p x.args[0][0].base.node.type_vars
# ['Payload']   # apparently this is "fullname"? it is verbatim, not "resolved"
#               # e.g. if I used Generic[xju.misc.Payload] I get ['xju.misc.Payload'] here!
#               # another example shows it is the "original" name passed to Generic and
#               # that it will match the type's "name" when used:
#                  (Pdb) p x.args[0][0].base.node.type_vars
#                  ['Payload', 'misc.Payload']
#                              ^ we use this one as type of "new_value" attr
#                  (Pdb) p x.args[0][0].base.node.names['new_value'].node.type.name
#                  'misc.Payload'
#
# (Pdb) p type(x.args[0][0].base.node.type_vars[0])
# <class 'str'>
# (Pdb) p x.args[0][0].base.node.names['new_value'].node.type
# Payload`1
# (Pdb) p type(x.args[0][0].base.node.names['new_value'].node.type)
# <class 'mypy.types.TypeVarType'>


T=TypeVar('T')

JsonType = None|bool|dict|list|float|str

class TypeScriptUQNTag:pass
class TypeScriptUQN(xju.newtype.Str[TypeScriptUQNTag]):pass

class TypeScriptSourceCodeTag:pass
class TypeScriptSourceCode(xju.newtype.Str[TypeScriptSourceCodeTag]):pass

@dataclass
class TypeScriptNamespace:
    defs: dict[TypeScriptUQN, TypeScriptSourceCode | Self]

    def get_namespace_of(self,fqn:Sequence[TypeScriptUQN]) -> Self:
        match len(fqn):
            case 1:
                return self
            case _:
                m=self.defs.setdefault(fqn[0],self.__class__({}))
                assert isinstance(m,self.__class__)
                return m.get_namespace_of(fqn[1:])
        pass

    def get_formatted_defs(self, export:Literal['']|Literal['export ']='')->TypeScriptSourceCode:
        '''get defs of this namespace as (possibly multi-level) namespace content
           - top level defs are exported or not per {export}
           - sub-level defs are always exported'''
        result:list[TypeScriptSourceCode]=[]
        for name, d in self.defs.items():
            match d:
                case TypeScriptSourceCode():
                    result.append(TypeScriptSourceCode(f'{export}{d}'))
                case TypeScriptNamespace:
                    result.append(TypeScriptSourceCode(
                        f"{export}namespace {name} {{\n"
                        f"    {indent(4, d.get_formatted_defs(export='export '))}\n"
                        f"}}\n"))
            pass
        return TypeScriptSourceCode('\n'.join([_.value() for _ in result]))
    pass

class CodecProto(Generic[T], Protocol):
    def encode(self,x:T) -> JsonType:
        'encode {self.t} {x} to json'
        pass
    def decode(self,x:JsonType) -> T:
        'decode {x} to a {self.t}'
        pass
    def get_json_schema(self) -> dict:
        "get jsonschema matching T's encoded form"
        pass
    def ensure_typescript_defs(self, namespace:TypeScriptNamespace) -> None:
        '''ensure namespace {namespace} contains {self} typescript defs'''
        pass
    def add_typescript_alias(self, namespace:TypeScriptNamespace, fqn:Sequence[TypeScriptUQN]) -> None:
        '''add {fqn} as typescript alias for {self.t}'''
        
def codec(t: Type[T]) -> CodecProto[T]:
    '''build codec to encode/decode a "t" to/from json

       - note t may be a Union (which is not a Type; use json_codec_mypy_plugin for
         proper typing and no errors with mypy
    '''
    return Codec[T](t)


@runtime_checkable
class CodecImplProto(Protocol):
    def encode(self,x:Any)->JsonType:
        pass
    def decode(self,x:JsonType)->Any:
        pass
    def get_type_fqn(self) -> str:
        """get (python) type fully qualified name, e.g. 'a.b.C'"""
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        pass
    def typescript_type(self) -> TypeScriptSourceCode:
        pass
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self, namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''returns an expression that evaluates to a xju.json_codec.IsInstance'''
        pass
    def get_typescript_asa(self, namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''returns an expression that evaluates to a xju.json_codec.AsInstance'''
        pass
    pass

@runtime_checkable
class UsableAsKeyProto(CodecImplProto,Protocol):
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        pass
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        pass
    def get_typescript_isa_key(self, namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''returns an expression that evaluates to a xju.json_codec.IsKey'''
        pass
    def get_typescript_asa_key(self, namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''returns an expression that evaluates to a xju.json_codec.AsKey'''
        pass
    pass

def usable_as_key(codec: CodecImplProto) -> UsableAsKeyProto:
    if isinstance(codec, UsableAsKeyProto):
        return codec
    raise Exception(f"{codec.get_type_fqn()} is not valid as a json object key")

# implementation of CodecProto - use codec above with json_codec_mypy_plugin
# rather than using this class directly
class Codec(Generic[T]):
    t:Type[T]
    codec:CodecImplProto
    def __init__(self, t: Type[T]):
        'initialse json decoder for type %(t)r'
        self.t=t
        self.codec=_explodeSchema(self.t, {}, {})
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

    def get_json_schema(self) -> dict:
        definitions: dict[str,dict] = {}
        result = self.codec.get_json_schema(definitions)
        result.update({
            "$id": "https://example.com/address.schema.json",
            "$schema": "https://json-schema.org/draft/2020-12/schema"})
        result.update({
            'definitions':definitions
        })
        return result
    def ensure_typescript_defs(self, namespace:TypeScriptNamespace) -> None:
        return self.codec.ensure_typescript_defs(namespace)
    def add_typescript_alias(self, namespace:TypeScriptNamespace, fqn:Sequence[TypeScriptUQN]) -> None:
        '''add {fqn} as typescript alias for {self.t}'''
        assert len(fqn)>0
        try:
            target_namespace=namespace.get_namespace_of(fqn)
            if fqn[-1] in target_namespace.defs:
                m='.'.join([str(_) for _ in fqn])
                raise Exception(
                    f'{m} is already defined as {target_namespace.defs[fqn[-1]]}')
            tt=self.codec.typescript_type()
            target_namespace.defs[fqn[-1]]=TypeScriptSourceCode(
                f'type {fqn[-1]} = {tt};')
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{fqn[-1]}")] = TypeScriptSourceCode(
                f"function asInstanceOf{fqn[-1]}(v:any):{fqn[-1]} {{\n"
                f"    {indent(8, self.codec.get_typescript_asa(namespace))}.f(v).applyDefaults();\n"
                f"    return v as {fqn[-1]};\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f"isInstanceOf{fqn[-1]}")] = TypeScriptSourceCode(
                f"function isInstanceOf{fqn[-1]}(v:any):v is {fqn[-1]} {{\n"
                f"    const r={indent(8, self.codec.get_typescript_isa(namespace))}(v);\n"
                f"    return r&&r.applyDefaults();\n"
                f"}}")
        except:
            raise in_function_context(Codec.add_typescript_alias,vars())
        pass
    pass

Atom=TypeVar('Atom',int,str,bool,float)

NewInt=TypeVar('NewInt',bound=xju.newtype.Int)
NewFloat=TypeVar('NewFloat',bound=xju.newtype.Float)
NewStr=TypeVar('NewStr',bound=xju.newtype.Str)

class NoopCodecImpl(Generic[Atom]):
    t:Type[Atom]
    def __init__(self, t:Type[Atom]):
        self.t=t
    def encode(self, x:Atom)->Atom:
        if type(x) is not self.t and not (
                self.t is float and type(x) is int):
            raise Exception(f'{x!r} is not a {self.t}')
        return x
    def decode(self, x:JsonType)->Atom:
        if type(x) is not self.t and not (
                self.t is float and type(x) is int):
            t=type(x)
            raise Exception(f'{x!r} (of type {t}) is not a {self.t}')
        return self.t(x)
    def get_type_fqn(self) -> str:
        return get_type_fqn(self.t)
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        st: str
        if self.t is bool: st='boolean'
        if self.t is int: st='integer'
        if self.t is float: st='number'
        if self.t is str: st='string'
        return { 'type': st }
    def get_object_key_json_schema(
            self, definitions:dict[str,dict]) -> dict:
        if issubclass(self.t, bool):
            return {
                'description': f'bool object key',
                'type': 'string',
                'enum': [ 'true', 'false' ]
            }
        if issubclass(self.t, int):
            return {
                'description': f'int object key',
                'type': 'string',
                'pattern': r'^-?(0|[1-9][0-9]*)$'
            }
        if issubclass(self.t, float):
            return {
                'description': f'float object key',
                'type': 'string',
                'pattern': r'^-?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)?$'
            }
        if issubclass(self.t, str):
            return {
                'description': f'string object key',
                'type': 'string'
            }
    def typescript_type(self) -> TypeScriptSourceCode:
        '''return typescript equivalent type for T'''
        if self.t is int: return TypeScriptSourceCode('number')
        if self.t is str: return TypeScriptSourceCode('string')
        if self.t is float: return TypeScriptSourceCode('number')
        assert self.t is bool, self.t
        return TypeScriptSourceCode('boolean')
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        if issubclass(self.t,bool):
            return TypeScriptSourceCode('string /* bool */')
        elif issubclass(self.t,str):
            return TypeScriptSourceCode('string')
        elif issubclass(self.t,int):
            return TypeScriptSourceCode('string /* int */')
        elif issubclass(self.t,float):
            return TypeScriptSourceCode('string /* float */')

    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        if issubclass(self.t,bool):
            return TypeScriptSourceCode("xju.json_codec.isInstanceOfBool")
        if issubclass(self.t, int):
            return TypeScriptSourceCode("xju.json_codec.isInstanceOfInt")
        if issubclass(self.t,float):
            return TypeScriptSourceCode("xju.json_codec.isInstanceOfFloat")
        if issubclass(self.t,str):
            return TypeScriptSourceCode("xju.json_codec.isInstanceOfString")
    def get_typescript_isa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        if issubclass(self.t,bool):
            return TypeScriptSourceCode("xju.json_codec.isKeyOfBool")
        if issubclass(self.t,int):
            return TypeScriptSourceCode("xju.json_codec.isKeyOfInt")
        if issubclass(self.t,float):
            return TypeScriptSourceCode("xju.json_codec.isKeyOfFloat")
        if issubclass(self.t,str):
            return TypeScriptSourceCode("xju.json_codec.isKeyOfString")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        if issubclass(self.t, bool):
            return TypeScriptSourceCode(f"xju.json_codec.asInstanceOfBool('bool')")
        if issubclass(self.t, int):
            return TypeScriptSourceCode(f"xju.json_codec.asInstanceOfInt('int')")
        if issubclass(self.t, float):
            return TypeScriptSourceCode(f"xju.json_codec.asInstanceOfFloat('float')")
        if issubclass(self.t, str):
            return TypeScriptSourceCode("xju.json_codec.asInstanceOfString('string')")
    def get_typescript_asa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        tt=self.typescript_as_object_key_type()
        if issubclass(self.t,bool):
            return TypeScriptSourceCode("xju.json_codec.asKeyOfBool('bool')")
        if issubclass(self.t,int):
            return TypeScriptSourceCode("xju.json_codec.asKeyOfInt('int')")
        if issubclass(self.t,float):
            return TypeScriptSourceCode("xju.json_codec.asKeyOfFloat('float')")
        if issubclass(self.t,str):
            return TypeScriptSourceCode("xju.json_codec.asKeyOfString('string')")
    pass

assert issubclass(NoopCodecImpl, UsableAsKeyProto)

class NoneCodecImpl:
    def encode(self, x:None)->None:
        if x is not None:
            raise Exception(f'{x!r} is not None')
        return x
    def decode(self, x:JsonType)->None:
        if x is not None:
            raise Exception(f'{x!r} is not None')
        return x
    def get_type_fqn(self) -> str:
        return 'None'
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        return { 'type': 'null' }
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        return {
            'description': f'null object key',
            'type': 'string',
            'enum': [ 'null' ]
        }
    def typescript_type(self) -> TypeScriptSourceCode:
        '''return typescript equivalent type for None'''
        return TypeScriptSourceCode('null')
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        # python's json.dumps turns None into "null"
        return TypeScriptSourceCode('string /* null */')
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("xju.json_codec.isInstanceOfNull")
    def get_typescript_isa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("xju.json_codec.isKeyOfNull")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("xju.json_codec.asInstanceOfNull")
    def get_typescript_asa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("xju.json_codec.asKeyOfNull")
    pass

assert issubclass(NoneCodecImpl, UsableAsKeyProto)

class ListCodecImpl:
    def __init__(self, value_codec:CodecImplProto):
        self.value_codec=value_codec
        pass
    def encode(self,x):
        if type(x) is not list:
            raise Exception(f'{x!r} is not a list')
        return [self.value_codec.encode(_) for _ in x]
    def decode(self,x):
        if type(x) is not list:
            raise Exception(f'{x!r} is not a list')
        return [self.value_codec.decode(_) for _ in x]
    def get_type_fqn(self) -> str:
        return f'list[{self.value_codec.get_type_fqn()}]'
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
                "type": "array",
                "items": self.value_codec.get_json_schema(definitions)
            }
            pass
        return {
            '$ref': self_ref
        }
    def typescript_type(self)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"Array<{self.value_codec.typescript_type()}>")
    def ensure_typescript_defs(self, namespace) -> None:
        self.value_codec.ensure_typescript_defs(namespace)
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"xju.json_codec.isInstanceOfList({self.value_codec.get_typescript_isa(namespace)})")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"xju.json_codec.asInstanceOfList({self.value_codec.get_typescript_asa(namespace)})")
    pass

assert not isinstance(ListCodecImpl, UsableAsKeyProto)

class AnyListCodecImpl:
    def __init__(self):
        self.value_codec=AnyJsonCodecImpl()
        pass
    def encode(self,x):
        if type(x) is not list:
            raise Exception(f'{x!r} is not a list')
        return [self.value_codec.encode(_) for _ in x]
    def decode(self,x):
        if type(x) is not list:
            raise Exception(f'{x!r} is not a list')
        return [self.value_codec.decode(_) for _ in x]
    def get_type_fqn(self) -> str:
        return 'list'
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        return {
            "type": "array"
        }
    def typescript_type(self)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"Array<any>")
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("xju.json_codec.isInstanceOfAnyList")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("xju.json_codec.asInstanceOfAnyList")
    pass

assert not isinstance(AnyListCodecImpl, UsableAsKeyProto)

class SetCodecImpl:
    def __init__(self, value_codec:CodecImplProto):
        self.value_codec=value_codec
        pass
    def encode(self,x):
        if type(x) is not set:
            raise Exception(f'{x!r} is not a set')
        return [self.value_codec.encode(_) for _ in x]
    def decode(self,x):
        if type(x) is not list:
            raise Exception(f'{x!r} is not a set')
        result = set([self.value_codec.decode(_) for _ in x])
        if len(result) < len(x):
            raise Exception(f'{x!r} contains at least one duplicate element')
        return result
    def get_type_fqn(self) -> str:
        return f'set[{self.value_codec.get_type_fqn()}]'
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
                "type": "array",
                "uniqueItems": True,
                "items": self.value_codec.get_json_schema(definitions)
            }
            pass
        return {
            '$ref': self_ref
        }
    def typescript_type(self)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"Array<{self.value_codec.typescript_type()}> /* with unique elements */")
    def ensure_typescript_defs(self, namespace) -> None:
        self.value_codec.ensure_typescript_defs(namespace)
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f"xju.json_codec.isInstanceOfList({self.value_codec.get_typescript_isa(namespace)})")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        tt=self.typescript_type()
        return TypeScriptSourceCode(f"xju.json_codec.asInstanceOfList({self.value_codec.get_typescript_asa(namespace)})")
    pass

assert not isinstance(SetCodecImpl, UsableAsKeyProto)

class AnySetCodecImpl:
    def __init__(self):
        self.value_codec=AnyJsonCodecImpl()
        pass
    def encode(self,x):
        if type(x) is not set:
            raise Exception(f'{x!r} is not a set')
        return [self.value_codec.encode(_) for _ in x]
    def decode(self,x):
        if type(x) is not list:
            raise Exception(f'{x!r} is not a list')
        result = set([self.value_codec.decode(_) for _ in x])
        if len(result) < len(x):
            raise Exception(f'{x!r} contains at least one duplicate element')
        return result
    def get_type_fqn(self) -> str:
        return 'set'
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
                "type": "array",
                "uniqueItems": True
            }
            pass
        return {
            '$ref': self_ref
        }
    def typescript_type(self)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"Array<any> /* with unique elements */")
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("xju.json_codec.isInstanceOfAnyList")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("xju.json_codec.asInstanceOfAnyList")
    pass

assert not isinstance(AnySetCodecImpl, UsableAsKeyProto)

class FrozenSetCodecImpl:
    def __init__(self, value_codec:CodecImplProto):
        self.value_codec=value_codec
        pass
    def encode(self,x):
        if type(x) is not frozenset:
            raise Exception(f'{x!r} is not a frozenset')
        return [self.value_codec.encode(_) for _ in x]
    def decode(self,x):
        if type(x) is not list:
            raise Exception(f'{x!r} is not a frozenset')
        result = frozenset([self.value_codec.decode(_) for _ in x])
        if len(result) < len(x):
            raise Exception(f'{x!r} contains at least one duplicate element')
        return result
    def get_type_fqn(self) -> str:
        return f'frozenset[{self.value_codec.get_type_fqn()}]'
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
                "type": "array",
                "uniqueItems": True,
                "items": self.value_codec.get_json_schema(definitions)
            }
            pass
        return {
            '$ref': self_ref
        }
    def typescript_type(self)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"Array<{self.value_codec.typescript_type()}> /* with unique elements */")
    def ensure_typescript_defs(self, namespace) -> None:
        self.value_codec.ensure_typescript_defs(namespace)
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f"xju.json_codec.isInstanceOfList({self.value_codec.get_typescript_isa(namespace)})")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f"xju.json_codec.asInstanceOfList({self.value_codec.get_typescript_asa(namespace)})")
    pass

assert not isinstance(FrozenSetCodecImpl, UsableAsKeyProto)

class AnyFrozenSetCodecImpl:
    def __init__(self):
        self.value_codec=AnyJsonCodecImpl()
        pass
    def encode(self,x):
        if type(x) is not frozenset:
            raise Exception(f'{x!r} is not a frozenset')
        return [self.value_codec.encode(_) for _ in x]
    def decode(self,x):
        if type(x) is not list:
            raise Exception(f'{x!r} is not a list')
        result = frozenset([self.value_codec.decode(_) for _ in x])
        if len(result) < len(x):
            raise Exception(f'{x!r} contains at least one duplicate element')
        return result
    def get_type_fqn(self) -> str:
        return 'frozenset'
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
                "type": "array",
                "uniqueItems": True
            }
            pass
        return {
            '$ref': self_ref
        }
    def typescript_type(self)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"Array<any> /* with unique elements */")
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("xju.json_codec.isInstanceOfAnyList")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("xju.json_codec.asInstanceOfAnyList")
    pass

assert not isinstance(AnyFrozenSetCodecImpl, UsableAsKeyProto)

class TupleCodecImpl:
    def __init__(self,value_codecs):
        self.value_codecs=value_codecs
        self.number_of_codecs=len(value_codecs)
        pass
    def encode(self,x) -> list:
        'encode tuple {x} as a {self.number_of_codecs}-element list'
        try:
            if type(x) is not tuple:
                raise Exception(f'{x!r} is not a tuple')
            if len(x) != self.number_of_codecs:
                l=len(x)
                raise Exception(f'{x} does not have {self.number_of_codecs} items (it has {l} items)')
            return [c.encode(v) for c,v in zip(self.value_codecs,x)]
        except Exception:
            raise in_function_context(TupleCodecImpl.encode,vars()) from None
        pass
    def decode(self,x) -> tuple:
        "decode {x} as tuple"
        try:
            if type(x) is not list:
                raise Exception(f'{x!r} is not a list')
            if len(x) != self.number_of_codecs:
                l=len(x)
                raise Exception(f'{x} does not have {self.number_of_codecs} items (it has {l} items)')
            result=[c.decode(v) for c,v in zip(self.value_codecs,x)]
            return tuple(result)
        except Exception:
            raise in_function_context(TupleCodecImpl.decode,vars()) from None
        pass
    def get_type_fqn(self) -> str:
        return f'tuple[{",".join([c.get_type_fqn() for c in self.value_codecs])}]'
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
                "type": "array",
                "prefixItems": [
                    codec.get_json_schema(definitions) for codec in self.value_codecs
                ]
            }
            pass
        return {
            '$ref': self_ref
        }
    def typescript_type(self)->TypeScriptSourceCode:
        value_types=','.join([str(_.typescript_type()) for _ in self.value_codecs])
        return TypeScriptSourceCode(f"[{value_types}]")
    def ensure_typescript_defs(self, namespace) -> None:
        for c in self.value_codecs: c.ensure_typescript_defs(namespace)
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return TypeScriptSourceCode(
            "xju.json_codec.isInstanceOfTuple([\n"+
            ",\n".join(["    "+indent(4,c.get_typescript_isa(namespace)) for c in self.value_codecs])+
            "\n])")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        
        self.ensure_typescript_defs(namespace)
        return TypeScriptSourceCode(
            "xju.json_codec.asInstanceOfTuple([\n"+
            ",\n".join(["    "+indent(4,c.get_typescript_asa(namespace)) for c in self.value_codecs])+
            "\n])")
    pass

assert not isinstance(TupleCodecImpl, UsableAsKeyProto)

class UnionCodecImpl:
    def __init__(self,allowed_types, value_codecs:dict[Any,CodecImplProto]):
        self.allowed_types=allowed_types
        self.value_codecs=value_codecs
        pass
    def encode(self,x) -> JsonType:
        'encode {x!r} as one of {self.allowed_types}'
        try:
            exceptions=list[BaseException]()
            for t, c in self.value_codecs.items():
                try:
                    return c.encode(x)
                except Exception as e:
                    exceptions.append(in_context(f'decode as {t!r}'))
                    pass
                pass
            raise AllFailed(exceptions)
        except Exception:
            raise in_function_context(UnionCodecImpl.encode,vars()) from None
        pass
    def decode(self,x) -> tuple:
        '''decode {x!r} as one of {self.allowed_types}'''
        try:
            exceptions=list[BaseException]()
            for t, c in self.value_codecs.items():
                try:
                    return c.decode(x)
                except Exception as e:
                    exceptions.append(in_context(f'decode as {t!r}'))
                    pass
                pass
            raise AllFailed(exceptions)
        except Exception:
            raise in_function_context(UnionCodecImpl.decode,vars()) from None
        pass
    def get_type_fqn(self) -> str:
        return f'{"|".join([c.get_type_fqn() for c in self.value_codecs.values()])}'
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
                "oneOf": [ codec.get_json_schema(definitions) for t,codec in self.value_codecs.items() ]
            }
            pass
        return {
            '$ref': self_ref
        }
            
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        return {
            "oneOf": [ usable_as_key(codec).get_object_key_json_schema(definitions)
                       for t,codec in self.value_codecs.items() ]
        }
    def typescript_type(self)->TypeScriptSourceCode:
        return TypeScriptSourceCode(
            '|'.join([str(c.typescript_type()) for _t,c in self.value_codecs.items()]))
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        '''get typescript type of dictionary key that is any of {self.allowed_types}'''
        return TypeScriptSourceCode(
            '|'.join([str(usable_as_key(c).typescript_as_object_key_type())
                      for _t,c in self.value_codecs.items()]))
    def ensure_typescript_defs(self, namespace) -> None:
        for _,c in self.value_codecs.items():
            c.ensure_typescript_defs(namespace)
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return TypeScriptSourceCode(
            f"xju.json_codec.isInstanceOfUnion(["+
            ",".join(['\n    '+indent(4,c.get_typescript_isa(namespace))
                      for c in self.value_codecs.values()])+
            "\n])")
    def get_typescript_isa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return TypeScriptSourceCode(
            f"xju.json_codec.isKeyOfUnion(["+
            ",".join(['\n    '+indent(4,usable_as_key(c).get_typescript_isa_key(namespace))
                      for c in self.value_codecs.values()])+
            "\n])")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return TypeScriptSourceCode(
            f"xju.json_codec.asInstanceOfUnion(["+
            ",".join(['\n    '+indent(4,c.get_typescript_asa(namespace))
                      for c in self.value_codecs.values()])+
            "\n])")
    def get_typescript_asa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return TypeScriptSourceCode(
            f"xju.json_codec.asKeyOfUnion(["+
            ",".join(['\n    '+indent(4,usable_as_key(c).get_typescript_asa_key(namespace))
                      for c in self.value_codecs.values()])+
            "\n])")
    pass

assert isinstance(UnionCodecImpl, UsableAsKeyProto)

class DictCodecImpl:
    decode_key:Callable[[JsonType],object]
    def __init__(self, key_codec: UsableAsKeyProto, value_codec:CodecImplProto):
        self.key_codec=key_codec
        self.value_codec=value_codec
        match key_codec:
            case NoopCodecImpl() if key_codec.t is str:
                self.decode_key=self.key_codec.decode
            case NewStrCodecImpl():
                self.decode_key=self.key_codec.decode
            case _:
                self.decode_key=self.decode_non_str_key_type
                pass
            
    def encode(self,x:dict)->dict:
        if type(x) is not dict:
            raise Exception(f'{x!r} is not a dict')
        result:dict={}
        for k,v in x.items():
            ek,ev=self.key_codec.encode(k),self.value_codec.encode(v)
            result[ek]=ev
            pass
        return result
                            
    def decode_non_str_key_type(self,k:JsonType)->object:
        if type(k) is str:
            exceptions=list[BaseException]()
            try:
                return self.key_codec.decode(json.loads(k))
            except Exception:
                exceptions.append(in_context(
                    f"decode key {k!r} via json.loads"))
                pass
            try:
                return self.key_codec.decode(k)
            except Exception:
                exceptions.append(in_context(
                    f"decode key {k!r} directly"))
            raise AllFailed(exceptions)
        return self.key_codec.decode(k)

    def decode(self,x)->dict:
        """decode x to dict[{self.key_codec}:{self.value_codec}]"""
        if type(x) is not dict:
            raise Exception(f'{x!r} is not a dict')
        return {self.decode_key(k):self.value_codec.decode(v)
                for k,v in x.items()}
    def get_type_fqn(self) -> str:
        return f'dict[{self.key_codec.get_type_fqn()},{self.value_codec.get_type_fqn()}]'
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        """get dict[{self.key_codec}:{self.value_codec}] json schema"""
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            # check key type is allowed
            self.key_codec.get_object_key_json_schema(definitions)
            definitions[fqn]={
                'type': 'object',
                'additionalProperties': self.value_codec.get_json_schema(definitions)
            }
            pass
        return {
            '$ref': self_ref
        }
    def typescript_type(self)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"{{ [key: {self.key_codec.typescript_as_object_key_type()}]: {self.value_codec.typescript_type()} }}")
    def ensure_typescript_defs(self, namespace) -> None:
        self.key_codec.ensure_typescript_defs(namespace)
        self.value_codec.ensure_typescript_defs(namespace)
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return TypeScriptSourceCode(
            f"xju.json_codec.isInstanceOfDict(\n"
            f"    {indent(4,self.key_codec.get_typescript_isa_key(namespace))},\n"
            f"    {indent(4,self.value_codec.get_typescript_isa(namespace))}\n"
            f")")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return TypeScriptSourceCode(
            f"xju.json_codec.asInstanceOfDict(\n"
            f"    {indent(4,self.key_codec.get_typescript_asa_key(namespace))},\n"
            f"    {indent(4,self.value_codec.get_typescript_asa(namespace))}\n"
            f")")
    pass

assert not isinstance(DictCodecImpl, UsableAsKeyProto)

class AnyDictCodecImpl:
    def __init__(self):
        self.key_codec=NoopCodecImpl[str](str)
        self.value_codec=AnyJsonCodecImpl()
    def encode(self,x:dict)->dict:
        if type(x) is not dict:
            raise Exception(f'{x!r} is not a dict')
        result:dict={}
        for k,v in x.items():
            ek,ev=self.key_codec.encode(k),self.value_codec.encode(v)
            result[ek]=ev
            pass
        return result
    def decode(self,x)->dict:
        if type(x) is not dict:
            raise Exception(f'{x!r} is not a dict')
        return {self.key_codec.decode(k):self.value_codec.decode(v) for k,v in x.items()}
    def get_type_fqn(self) -> str:
        return 'dict'
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
                'type': 'object'
            }
            pass
        return {
            '$ref': self_ref
        }
    def typescript_type(self)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"{{ [key: {self.key_codec.typescript_as_object_key_type()}]: {self.value_codec.typescript_type()} }}")
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("xju.json_codec.isInstanceOfAnyDict")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("xju.json_codec.asInstanceOfAnyDict")
    pass

assert not isinstance(AnyDictCodecImpl, UsableAsKeyProto)

class AnyJsonCodecImpl:
    def encode(self,x):
        # we assume x will be subsequently json.dumps()ed so defer validation to that
        return x
    def decode(self,x):
        # we assume x was json.loads()ed so assume it is appropriate
        return x
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn='Any'
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
                "oneOf": [
                    { 'type': t } for t in ['null','boolean','object','array','number','string']
                ]
            }
            pass
        return {
            '$ref': self_ref
        }
        return {
        }
    def typescript_type(self)->TypeScriptSourceCode:
        return TypeScriptSourceCode('any')
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("xju.json_codec.isInstanceOfAny")  #pragma NO COVER
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("xju.json_codec.asInstanceOfAny")  #pragma NO COVER
    pass

assert not isinstance(AnyJsonCodecImpl, UsableAsKeyProto)

class NewIntCodecImpl(Generic[NewInt]):
    t:Type[NewInt]
    def __init__(self,t:Type[NewInt]):
        self.t=t
        self.base_codec=NoopCodecImpl[int](int)
    def encode(self, x:NewInt)->int:
        if not isinstance(x, xju.newtype.Int):
            raise Exception(f'{x!r} is not a {self.t}')
        return self.base_codec.encode(x.value())
    def decode(self, x:JsonType)->NewInt:
        if not isinstance(x, int):
            t=type(x)
            raise Exception(f'{x!r} (of type {t}) is not an int')
        return self.t(self.base_codec.decode(x))
    def get_type_fqn(self) -> str:
        '''get the fully qualified name of {self.t}'''
        return get_type_fqn(self.t)
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
                'type': 'integer'
            }
            pass
        return {
            '$ref': self_ref
        }
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        return {
            'description': self.get_type_fqn(),
            'type': 'string',
            'pattern': r'^-?(0|[1-9][0-9]*)$'
        }
    def typescript_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(self.get_type_fqn())
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        x=self.get_type_fqn().replace('/*','**').replace('*/','**')
        return TypeScriptSourceCode(f"string /* {x} */")
    def ensure_typescript_defs(self, namespace) -> None:
        typescript_fqn=[TypeScriptUQN(_) for _ in self.typescript_type().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        tt=typescript_fqn[-1]
        if tt not in target_namespace.defs:
            target_namespace.defs[tt]=TypeScriptSourceCode(
                f"type {tt} = number;")
            target_namespace.defs[TypeScriptUQN(f'isInstanceOf{tt}')]=TypeScriptSourceCode(
                f"function isInstanceOf{tt}(v:any): v is {tt}\n"
                f"{{\n"
                f"    const r=xju.json_codec.isInstanceOfInt(v);\n"
                f"    return r&&r.applyDefaults();\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{tt}")]=TypeScriptSourceCode(
                f"function asInstanceOf{tt}(v: any): {tt} {{\n"
                f"    _asInstanceOf{tt}.f(v).applyDefaults();\n"
                f"    return v as {tt};\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f"_asInstanceOf{tt}")]=TypeScriptSourceCode(
                f"const _asInstanceOf{tt} = xju.json_codec.asInstanceOfInt('{tt}');")
            target_namespace.defs[TypeScriptUQN(f"_asKeyOf{tt}")]=TypeScriptSourceCode(
                f"const _asKeyOf{tt} = xju.json_codec.asKeyOfInt('{tt}');")
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return TypeScriptSourceCode("xju.json_codec.isInstanceOfInt")
    def get_typescript_isa_key(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return TypeScriptSourceCode("xju.json_codec.isKeyOfInt")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return as_instance_of_expression(self.get_type_fqn())
    def get_typescript_asa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return as_key_of_expression(self.get_type_fqn())
    pass

assert isinstance(NewIntCodecImpl, UsableAsKeyProto)

class NewFloatCodecImpl(Generic[NewFloat]):
    t:Type[NewFloat]
    def __init__(self,t:Type[NewFloat]):
        self.t=t
        self.base_codec=NoopCodecImpl[float](float)
    def encode(self, x:NewFloat)->float:
        if not isinstance(x, xju.newtype.Float):
            raise Exception(f'{x!r} is not a {self.t}')
        return self.base_codec.encode(x.value())
    def decode(self, x:JsonType)->NewFloat:
        if type(x) is not float and type(x) is not int:
            t=type(x)
            raise Exception(f'{x!r} (of type {t}) is not a float (or an int)')
        return self.t(self.base_codec.decode(x))
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        return get_type_fqn(self.t)
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
                'type': 'number'
            }
            pass
        return {
            '$ref': self_ref
        }
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        return {
            'description': self.get_type_fqn(),
            'type': 'string',
            'pattern': r'^-?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)?$'
        }
    def typescript_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(self.get_type_fqn())
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        x=f"string /* {self.get_type_fqn().replace('/*','**').replace('*/','**')} */"
        return TypeScriptSourceCode(x)
    def ensure_typescript_defs(self, namespace) -> None:
        typescript_fqn=[TypeScriptUQN(_) for _ in self.typescript_type().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        tt=typescript_fqn[-1]
        if tt not in target_namespace.defs:
            target_namespace.defs[tt]=TypeScriptSourceCode(
                f"type {tt} = number;")
            target_namespace.defs[TypeScriptUQN(f'isInstanceOf{tt}')]=TypeScriptSourceCode(
                f"function isInstanceOf{tt}(v:any): v is {tt}\n"
                f"{{\n"
                f"    const r=xju.json_codec.isInstanceOfFloat(v);\n"
                f"    return r&&r.applyDefaults();\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{tt}")]=TypeScriptSourceCode(
                f"function asInstanceOf{tt}(v: any): {tt} {{\n"
                f"    _asInstanceOf{tt}.f(v).applyDefaults();\n"
                f"    return v as {tt};\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f"_asInstanceOf{tt}")]=TypeScriptSourceCode(
                f"const _asInstanceOf{tt} = xju.json_codec.asInstanceOfFloat('{tt}');")
            target_namespace.defs[TypeScriptUQN(f"_asKeyOf{tt}")]=TypeScriptSourceCode(
                f"const _asKeyOf{tt} = xju.json_codec.asKeyOfFloat('{tt}');")
            pass
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return TypeScriptSourceCode("xju.json_codec.isInstanceOfFloat")
    def get_typescript_isa_key(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return TypeScriptSourceCode("xju.json_codec.isKeyOfFloat")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return as_instance_of_expression(self.get_type_fqn())
    def get_typescript_asa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return as_key_of_expression(self.get_type_fqn())
    pass

assert isinstance(NewFloatCodecImpl, UsableAsKeyProto)

class NewStrCodecImpl(Generic[NewStr]):
    t:Type[NewStr]
    def __init__(self,t:Type[NewStr]):
        self.t=t
        self.base_codec=NoopCodecImpl[str](str)
    def encode(self, x:NewStr)->str:
        if not isinstance(x, xju.newtype.Str):
            raise Exception(f'{x!r} is not a {self.t}')
        return self.base_codec.encode(x.value())
    def decode(self, x:JsonType)->NewStr:
        if type(x) is not str:
            t=type(x)
            raise Exception(f'{x!r} (of type {t}) is not an str')
        return self.t(self.base_codec.decode(x))
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        return get_type_fqn(self.t)
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            if self.t.pattern is not None:
                definitions[fqn]={
                    'type': 'string',
                    'pattern': self.t.pattern.pattern
                }
                pass
            definitions[fqn]={
                'type': 'string'
            }
            pass
        return {
            '$ref': self_ref
        }
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        if self.t.pattern is not None:
            return {
                'description': self.get_type_fqn(),
                'type': 'string',
                'pattern': self.t.pattern.pattern
            }
        return {
            'description': self.get_type_fqn(),
            'type': 'string'
        }
    def typescript_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(self.get_type_fqn())
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        x=f"string /* {self.get_type_fqn().replace('/*','**').replace('*/','**')} */"
        return TypeScriptSourceCode(x)
    def ensure_typescript_defs(self, namespace) -> None:
        typescript_fqn=[TypeScriptUQN(_) for _ in self.typescript_type().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        tt=typescript_fqn[-1]
        if tt not in target_namespace.defs:
            target_namespace.defs[tt]=TypeScriptSourceCode(
                f"type {tt} = string;")
            target_namespace.defs[TypeScriptUQN(f'isInstanceOf{tt}')]=TypeScriptSourceCode(
                f"function isInstanceOf{tt}(v:any): v is {tt}\n"
                f"{{\n"
                f"    const r=_isInstanceOf{tt}(v);\n"
                f"    return r&&r.applyDefaults();\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{tt}")]=TypeScriptSourceCode(
                f"function asInstanceOf{tt}(v: any): {tt} {{\n"
                f"    _asInstanceOf{tt}.f(v).applyDefaults();\n"
                f"    return v as {tt};\n"
                f"}}")
            if self.t.pattern:
                target_namespace.defs[TypeScriptUQN(f"_isInstanceOf{tt}")]=TypeScriptSourceCode(
                    f"const _isInstanceOf{tt} = xju.json_codec.isInstanceOfStringPattern(new RegExp({json.dumps(self.t.pattern.pattern)}));")
                target_namespace.defs[TypeScriptUQN(f"_isKeyOf{tt}")]=TypeScriptSourceCode(
                    f"const _isKeyOf{tt} = xju.json_codec.isKeyOfStringPattern(new RegExp({json.dumps(self.t.pattern.pattern)}));")
                target_namespace.defs[TypeScriptUQN(f"_asInstanceOf{tt}")]=TypeScriptSourceCode(
                    f"const _asInstanceOf{tt} = xju.json_codec.asInstanceOfStringPattern('{tt}', new RegExp({json.dumps(self.t.pattern.pattern)}));")
                target_namespace.defs[TypeScriptUQN(f"_asKeyOf{tt}")]=TypeScriptSourceCode(
                    f"const _asKeyOf{tt} = xju.json_codec.asKeyOfStringPattern('{tt}', new RegExp({json.dumps(self.t.pattern.pattern)}));")
            else:
                target_namespace.defs[TypeScriptUQN(f"_isInstanceOf{tt}")]=TypeScriptSourceCode(
                    f"const _isInstanceOf{tt} = xju.json_codec.isInstanceOfString;")
                target_namespace.defs[TypeScriptUQN(f"_isKeyOf{tt}")]=TypeScriptSourceCode(
                    f"const _isKeyOf{tt} = xju.json_codec.isKeyOfString;")
                target_namespace.defs[TypeScriptUQN(f"_asInstanceOf{tt}")]=TypeScriptSourceCode(
                    f"const _asInstanceOf{tt} = xju.json_codec.asInstanceOfString('{tt}');")
                target_namespace.defs[TypeScriptUQN(f"_asKeyOf{tt}")]=TypeScriptSourceCode(
                    f"const _asKeyOf{tt} = xju.json_codec.asKeyOfString('{tt}');")
            pass
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return is_instance_of_expression(self.get_type_fqn())
    def get_typescript_isa_key(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return is_key_of_expression(self.get_type_fqn())
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return as_instance_of_expression(self.get_type_fqn())
    def get_typescript_asa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return as_key_of_expression(self.get_type_fqn())
    pass

assert isinstance(NewStrCodecImpl, UsableAsKeyProto)

class TimestampCodecImpl:
    def __init__(self):
        self.base_codec=NoopCodecImpl[float](float)
    def encode(self, x:Timestamp)->float:
        if not isinstance(x, Timestamp):
            raise Exception(f'{x!r} is not a {Timestamp}')
        return self.base_codec.encode(x.value())
    def decode(self, x:JsonType)->Timestamp:
        if type(x) is not float and type(x) is not int:
            t=type(x)
            raise Exception(f'{x!r} (of type {t}) is not a float (or an int)')
        return Timestamp(self.base_codec.decode(x))
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        return get_type_fqn(Timestamp)
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
                'type': 'number'
            }
            pass
        return {
            '$ref': self_ref
        }
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        return {
            'description': self.get_type_fqn(),
            'type': 'string',
            'pattern': r'^-?(0|[1-9][0-9]*)(\.[0-9]+)?([eE][+-]?[0-9]+)?$'
        }
    def typescript_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(self.get_type_fqn())
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode('string /* xju.time.Timestamp */')
    def ensure_typescript_defs(self, namespace) -> None:
        typescript_fqn=[TypeScriptUQN(_) for _ in self.typescript_type().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        tt=typescript_fqn[-1]
        if tt not in target_namespace.defs:
            target_namespace.defs[tt]=TypeScriptSourceCode(
                f"type {tt} = number;")
            target_namespace.defs[TypeScriptUQN(f'isInstanceOf{tt}')]=TypeScriptSourceCode(
                f"function isInstanceOf{tt}(v:any): v is {tt}\n"
                f"{{\n"
                f"    const r=xju.json_codec.isInstanceOfFloat(v);\n"
                f"    return r&&r.applyDefaults();\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{tt}")]=TypeScriptSourceCode(
                f"function asInstanceOf{tt}(v: any): {tt} {{\n"
                f"    const r=_asInstanceOf{tt}.f(v);\n"
                f"    r.applyDefaults();\n"
                f"    return v as {tt};\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f"_isInstanceOf{tt}")]=TypeScriptSourceCode(
                f"const _isInstanceOf{tt} = xju.json_codec.isInstanceOfFloat;")
            target_namespace.defs[TypeScriptUQN(f"_isKeyOf{tt}")]=TypeScriptSourceCode(
                f"const _isKeyOf{tt} = xju.json_codec.isKeyOfFloat;")
            target_namespace.defs[TypeScriptUQN(f"_asInstanceOf{tt}")]=TypeScriptSourceCode(
                f"const _asInstanceOf{tt} = xju.json_codec.asInstanceOfFloat('{tt}');")
            target_namespace.defs[TypeScriptUQN(f"_asKeyOf{tt}")]=TypeScriptSourceCode(
                f"const _asKeyOf{tt} = xju.json_codec.asKeyOfFloat('{tt}');")
            pass
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return is_instance_of_expression(self.get_type_fqn())
    def get_typescript_isa_key(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return is_key_of_expression(self.get_type_fqn())
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return as_instance_of_expression(self.get_type_fqn())
    def get_typescript_asa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return as_key_of_expression(self.get_type_fqn())
    pass

assert isinstance(TimestampCodecImpl, UsableAsKeyProto)

class LiteralStrCodecImpl:
    value:str
    def __init__(self,value:str):
        self.value=value
    def encode(self, x:str)->str:
        if type(x) is not str or x != self.value:
            raise Exception(f'{x!r} is not {self.value!r}')
        return x
    def decode(self, x:JsonType)->str:
        if type(x) is not str:
            raise Exception(f'{x!r} is not a string')
        if x != self.value:
            raise Exception(f'{x!r} is not {self.value!r}')
        return x
    def get_type_fqn(self) -> str:
        return f'Literal[{self.value!r}]'
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
            'type': 'string',
            'enum': [ self.value ]
            }
            pass
        return {
            '$ref': self_ref
        }
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        return {
            'type': 'string',
            'enum': [ self.value ]
        }
    def typescript_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(json.dumps(self.value))
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(json.dumps(self.value))
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
            return TypeScriptSourceCode(f"xju.json_codec.isInstanceOfLiteral({self.typescript_type()})")
    def get_typescript_isa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
            return TypeScriptSourceCode(f"xju.json_codec.isKeyOfLiteral({self.typescript_type()})")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
            return TypeScriptSourceCode(f"xju.json_codec.asInstanceOfLiteral({self.typescript_type()})")
    def get_typescript_asa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
            return TypeScriptSourceCode(f"xju.json_codec.asKeyOfLiteral({self.typescript_type()})")
    pass

assert isinstance(LiteralStrCodecImpl, UsableAsKeyProto)

class LiteralIntCodecImpl:
    value:int
    def __init__(self,value:int):
        self.value=value
    def encode(self, x:int)->int:
        if type(x) is not int or x != self.value:
            raise Exception(f'{x!r} is not {self.value!r}')
        return x
    def decode(self, x:JsonType)->int:
        if type(x) is not int:
            raise Exception(f'{x!r} is not a int')
        if x != self.value:
            raise Exception(f'{x!r} is not {self.value!r}')
        return x
    def get_type_fqn(self) -> str:
        return f'Literal[{self.value}]'
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
            'type': 'number',
            'enum': [ self.value ]
            }
            pass
        return {
            '$ref': self_ref
        }
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        return {
            'type': 'string',
            'enum': [ f"{self.value}" ]
        }
    def typescript_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(json.dumps(self.value))
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("string")  # typescript does not allow json.dumps(str(self.typescript_type())) here
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
            return TypeScriptSourceCode(f"xju.json_codec.isInstanceOfLiteral({self.typescript_type()})")
    def get_typescript_isa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
            return TypeScriptSourceCode(f"xju.json_codec.isKeyOfLiteral({self.typescript_type()})")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
            return TypeScriptSourceCode(f"xju.json_codec.asInstanceOfLiteral({self.typescript_type()})")
    def get_typescript_asa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
            return TypeScriptSourceCode(f"xju.json_codec.asKeyOfLiteral({self.typescript_type()})")
    pass

assert isinstance(LiteralIntCodecImpl, UsableAsKeyProto)

class LiteralBoolCodecImpl:
    value:bool
    def __init__(self,value:bool):
        self.value=value
    def encode(self, x:bool)->bool:
        if type(x) is not bool or x != self.value:
            raise Exception(f'{x!r} is not {self.value!r}')
        return x
    def decode(self, x:JsonType)->bool:
        if type(x) is not bool:
            raise Exception(f'{x!r} is not a boolean')
        if x != self.value:
            raise Exception(f'{x!r} is not {self.value!r}')
        return x
    def get_type_fqn(self) -> str:
        return f'Literal[{self.value}]'
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
            'type': 'boolean',
            'enum': [ self.value ]
            }
            pass
        return {
            '$ref': self_ref
        }
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        return {
            'type': 'string',
            'enum': [ 'true' if self.value else 'false' ]
        }
    def typescript_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(json.dumps(self.value))
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("string")  # typescript does not allow json.dumps(str(self.typescript_type())) here
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
            return TypeScriptSourceCode(f"xju.json_codec.isInstanceOfLiteral({self.typescript_type()})")
    def get_typescript_isa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
            return TypeScriptSourceCode(f"xju.json_codec.isKeyOfLiteral({self.typescript_type()})")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
            return TypeScriptSourceCode(f"xju.json_codec.asInstanceOfLiteral({self.typescript_type()})")
    def get_typescript_asa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
            return TypeScriptSourceCode(f"xju.json_codec.asKeyOfLiteral({self.typescript_type()})")
    pass

assert isinstance(LiteralBoolCodecImpl, UsableAsKeyProto)

@dataclass
class BytesCodecImpl:
    value_codec: CodecImplProto
    def encode(self,x):
        if type(x) is not bytes:
            raise Exception(f'{x!r} is not of type bytes')
        return self.value_codec.encode([_ for _ in x])
    def decode(self,x):
        'decode {x} as "bytes"'
        try:
            return bytes(self.value_codec.decode(x))
        except Exception as e:
            raise in_function_context(BytesCodecImpl.decode,vars())
    def get_type_fqn(self) -> str:
        return 'bytes'
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
                '$ref': f'#/definitions/{self.value_codec.get_type_fqn()}'
            }
            pass
        return {
            '$ref': self_ref
        }

    def typescript_type(self)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"Array<number> /* bytes */")
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f"xju.json_codec.isInstanceOfBytes")
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f"xju.json_codec.asInstanceOfBytes")
    pass

assert not isinstance(BytesCodecImpl, UsableAsKeyProto)

@runtime_checkable
class CustomClassCodec(Protocol):
    '''implement these methods on class T to use custom encoding of instances of T'''
    @staticmethod
    def xju_json_codec_encode(
            x:object  # x is a T
    ) -> JsonType:
        assert False  #pragma NO COVER
        pass
    @staticmethod
    def xju_json_codec_decode(x:JsonType) -> object:  # must return a T
        assert False  #pragma NO COVER
        pass
    @staticmethod
    def xju_json_codec_get_json_schema(definitions:dict[str,dict]) -> dict:
        '''return json schema for T
           - may add any supporting definitions to definitions'''
        assert False  #pragma NO COVER
        return {}
    @staticmethod
    def xju_json_codec_get_typescript_type() -> TypeScriptSourceCode:
        '''return the typescript type that this class encodes to
           e.g. number
        '''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    @staticmethod
    def xju_json_codec_5_get_typescript_isa(
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''return typescript source that evaluates to a xju.json_codec.IsInstance
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    @staticmethod
    def xju_json_codec_5_get_typescript_asa(
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''return typescript source code that evaluates to a xju.json_codec.AsInstance
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    pass

def VerifyTypeIsCustomClassCodec(t: type):
    """use this function to let json_codec_mypy_plugin verify
    that t implements the CustomClassCodec protocol correctly
    (issubclass(t, CustomClassCodec) does not catch all incorrect
    implementations)"""
    pass

@runtime_checkable
class CustomStringKeyClassCodec(Protocol):
    '''
    implement these methods as well as CustomClassCodec method on class T to use custom encoding
    of instances of T as dict keys where encoded value when used as a dict key is a string
    '''
    @staticmethod
    def xju_json_codec_get_object_key_json_schema(definitions:dict[str,dict]) -> dict:
        '''return json schema for T when used as an object key
           - may add any supporting definitions to definitions'''
        assert False  #pragma NO COVER
        return {}
    @staticmethod
    def xju_json_codec_typescript_key_type()-> Literal['String']:
        assert False #pragma NO COVER
        return 'String'

    @staticmethod
    def xju_json_codec_5_get_typescript_isa_key(
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''return typescript source code that evaluates to a xju.json_codec.IsKey
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    @staticmethod
    def xju_json_codec_5_get_typescript_asa_key(
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''return typescript source code that evaluates to a xju.json_codec.AsKey
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    pass

def VerifyTypeIsCustomStringKeyClassCodec(t: type):
    """use this function to let json_codec_mypy_plugin verify
    that t implements the CustomStringKeyClassCodec protocol correctly
    (issubclass(t, CustomStringKeyClassCodec) does not catch all incorrect
    implementations)"""
    pass


@runtime_checkable
class CustomNonStringKeyClassCodec(Protocol):
    '''
    implement these methods as well as CustomClassCodec method on class T to use custom encoding
    of instances of T as dict keys where encoded value when used as a dict key not a string
    i.e. is int/float/bool or None
    '''
    @staticmethod
    def xju_json_codec_get_object_key_json_schema(definitions:dict[str,dict]) -> dict:
        '''return json schema for T when used as an object key
           - may add any supporting definitions to definitions'''
        assert False  #pragma NO COVER
        return {}
    @staticmethod
    def xju_json_codec_typescript_key_type()-> Literal['NonString']:
        assert False #pragma NO COVER
        return 'NonString'

    @staticmethod
    def xju_json_codec_5_get_typescript_isa_key(
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''return typescript source code that evaluates to a xju.json_codec.IsKey
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    @staticmethod
    def xju_json_codec_5_get_typescript_asa_key(
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''return typescript source code that evaluates to a xju.json_codec.AsKey
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    pass

def VerifyTypeIsCustomNonStringKeyClassCodec(t: type):
    """use this function to let json_codec_mypy_plugin verify
    that t implements the CustomNonStringKeyClassCodec protocol correctly
    (issubclass(t, CustomNonStringKeyClassCodec) does not catch all incorrect
    implementations)"""
    pass


@runtime_checkable
class CustomGenericClassCodec(Protocol):
    '''implement these methods on class T to use custom encoding of instances of T
       where T is a Generic class and custom encoding needs the concreate type parameters'''
    @staticmethod
    def xju_json_codec_encode_generic(
            x:object,  # x is a T
            type_var_map:dict[TypeVar,type]
    ) -> JsonType:
        assert False  #pragma NO COVER
        pass
    @staticmethod
    def xju_json_codec_decode_generic(
            x:JsonType,
            type_var_map:dict[TypeVar,type]) -> object:  # must return a T
        assert False  #pragma NO COVER
        pass
    @staticmethod
    def xju_json_codec_get_json_schema_generic(
            definitions:dict[str,dict],
            type_var_map:dict[TypeVar,type]) -> dict:
        '''return json schema for T
           - may add any supporting definitions to definitions'''
        assert False  #pragma NO COVER
        return {}
    @staticmethod
    def xju_json_codec_get_typescript_type_generic(
            type_var_map:dict[TypeVar,type]) -> TypeScriptSourceCode:
        '''return the typescript type that this class encodes to
           e.g. number
        '''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    @staticmethod
    def xju_json_codec_5_get_typescript_isa_generic(
            namespace: TypeScriptNamespace,
            type_var_map:dict[TypeVar,type]) -> TypeScriptSourceCode:
        '''return typescript source that evaluates to a xju.json_codec.IsInstance
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    @staticmethod
    def xju_json_codec_5_get_typescript_asa_generic(
            namespace: TypeScriptNamespace,
            type_var_map:dict[TypeVar,type]) -> TypeScriptSourceCode:
        '''return typescript source code that evaluates to a xju.json_codec.AsInstance
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    pass

def VerifyTypeIsCustomGenericClassCodec(t: type):
    """use this function to let json_codec_mypy_plugin verify
    that t implements the CustomGenericClassCodec protocol correctly
    (issubclass(t, CustomGenericClassCodec) does not catch all incorrect
    implementations)"""
    pass

@runtime_checkable
class CustomStringKeyGenericClassCodec(Protocol):
    '''
    implement these methods as well as CustomClassCodec method on class T to use custom encoding
    of instances of T as dict keys where encoded value when used as a dict key is a string
    where T is a Generic class and custom encoding needs the concreate type parameters
    '''
    @staticmethod
    def xju_json_codec_get_object_key_json_schema_generic(
            definitions:dict[str,dict],
            type_var_map:dict[TypeVar,type]) -> dict:
        '''return json schema for T when used as an object key
           - may add any supporting definitions to definitions'''
        assert False  #pragma NO COVER
        return {}

    @staticmethod
    def xju_json_codec_typescript_key_type_generic(type_var_map:dict[TypeVar,type])-> Literal['String']:
        assert False #pragma NO COVER
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

def VerifyTypeIsCustomStringKeyGenericClassCodec(t: type):
    """use this function to let json_codec_mypy_plugin verify
    that t implements the CustomStringKeyGenericClassCodec protocol correctly
    (issubclass(t, CustomStringKeyGenericClassCodec) does not catch all incorrect
    implementations)"""
    pass

assert not issubclass(CustomStringKeyClassCodec, CustomStringKeyGenericClassCodec)

@runtime_checkable
class CustomNonStringKeyGenericClassCodec(Protocol):
    '''
    implement these methods as well as CustomClassCodec method on class T to use custom encoding
    of instances of T as dict keys where encoded value when used as a dict key not a string
    i.e. is int/float/bool or None,
    where T is a Generic class and custom encoding needs the concreate type parameters
    '''
    @staticmethod
    def xju_json_codec_get_object_key_json_schema_generic(
            definitions:dict[str,dict],
            type_var_map:dict[TypeVar,type]) -> dict:
        '''return json schema for T when used as an object key
           - may add any supporting definitions to definitions'''
        assert False  #pragma NO COVER
        return {}
    @staticmethod
    def xju_json_codec_typescript_key_type_generic(type_var_map:dict[TypeVar,type])-> Literal['NonString']:
        assert False #pragma NO COVER
        return 'NonString'

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

def VerifyTypeIsCustomNonStringKeyGenericClassCodec(t: type):
    """use this function to let json_codec_mypy_plugin verify
    that t implements the CustomNonStringKeyGenericClassCodec protocol correctly
    (issubclass(t, CustomNonStringKeyGenericClassCodec) does not catch all incorrect
    implementations)"""
    pass

assert not issubclass(CustomNonStringKeyClassCodec, CustomNonStringKeyGenericClassCodec)

class PythonAttrNameTag: pass
class PythonAttrName(xju.newtype.Str[PythonAttrNameTag]): pass
class JsonAttrNameTag: pass
class JsonAttrName(xju.newtype.Str[JsonAttrNameTag]): pass

attr_name_map = dict[type, dict[PythonAttrName, JsonAttrName]]()
dont_encode_attr_map = dict[type, set[PythonAttrName]]()

@dataclass
class AttrCodec:
    encoded_name: JsonAttrName
    codec: CodecImplProto
    pass

@dataclass
class AttrCodecWithDefault(AttrCodec):
    default_value: object
    pass

def get_attr_encoded_name(mro: Sequence[type], n: PythonAttrName) -> JsonAttrName:
    result: None | JsonAttrName = None
    for t in mro:
        result = attr_name_map.get(t, {}).get(n, None)
        if result:
            return result
        pass
    return JsonAttrName(n.value())

def encode_attr_as(t: type, attr: PythonAttrName, encoded_name: JsonAttrName) -> None:
    """encode {t}'s {attr} attribute as json object property {encoded_name}"""
    try:
        assert attr.value() in get_type_hints(t), f"{t} apparently has no declared {attr!r} attribute"
        tt=attr_name_map.setdefault(t, {})
        current=tt.get(attr)
        assert current is None, f"{t}.{attr} is already mapped to encoded name {current}"
        tt[attr] = encoded_name
    except Exception:
        raise in_function_context(encode_attr_as, vars()) from None
    pass

def dont_encode_attr(t: type, attr: PythonAttrName) -> None:
    """don't encode {t}'s {attr} attribute to json"""
    try:
        assert attr.value() in get_type_hints(t), f"{t} apparently has no declared {attr!r} attribute"
        tt=dont_encode_attr_map.setdefault(t, set())
        assert attr not in tt, f"""{t}.{attr} is already marked "don't encode"""
        tt.add(attr)
    except Exception:
        raise in_function_context(dont_encode_attr, vars()) from None
    pass

@dataclass
class CustomGenericClassCodecImpl:
    t:type[CustomGenericClassCodec]
    type_var_map:dict[TypeVar,type]

    def encode(self,x) -> JsonType:
        'encode {x} as a {self.t}'
        try:
            if not isinstance(x, self.t):
                xt=type(x)
                raise Exception(f'{x!r} (of type {xt}) is not a {self.t}')
            return self.t.xju_json_codec_encode_generic(x,self.type_var_map)
        except Exception:
            raise in_function_context(CustomGenericClassCodecImpl.encode,vars()) from None
        pass
    def decode(self,x) -> object:
        'decode {x} as a {self.t}'
        try:
            result=self.t.xju_json_codec_decode_generic(x,self.type_var_map)
            assert isinstance(result,self.t), (repr(result),self.t)
            return result
        except Exception:
            raise in_function_context(CustomGenericClassCodecImpl.decode,vars()) from None
        pass
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        return get_type_fqn(self.t)
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        return self.t.xju_json_codec_get_json_schema_generic(definitions,self.type_var_map)
    def typescript_type(self) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_get_typescript_type_generic(self.type_var_map)
    def ensure_typescript_defs(self,namespace)->None:
        '''ensure {namespace} has a typescript definition for {self.get_type_fqn()}'''
        return None
    def get_typescript_isa(self,namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_5_get_typescript_isa_generic(namespace,self.type_var_map)

    def get_typescript_asa(self,namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_5_get_typescript_asa_generic(namespace,self.type_var_map)
    pass

assert isinstance(CustomGenericClassCodecImpl, CodecImplProto)
assert not isinstance(CustomGenericClassCodecImpl, UsableAsKeyProto)


@dataclass
class CustomClassCodecImpl:
    t:type[CustomClassCodec]

    def encode(self,x) -> JsonType:
        'encode {x} as a {self.t}'
        try:
            return self.t.xju_json_codec_encode(x)
        except Exception:
            raise in_function_context(CustomClassCodecImpl.encode,vars()) from None
        pass
    def decode(self,x) -> object:
        'decode {x} as a {self.t}'
        try:
            result=self.t.xju_json_codec_decode(x)
            assert isinstance(result,self.t), (repr(result),self.t)
            return result
        except Exception:
            raise in_function_context(CustomClassCodecImpl.decode,vars()) from None
        pass
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        return get_type_fqn(self.t)
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        return self.t.xju_json_codec_get_json_schema(definitions)
    def typescript_type(self) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_get_typescript_type()
    def ensure_typescript_defs(self,namespace)->None:
        '''ensure {namespace} has a typescript definition for {self.get_type_fqn()}'''
        return None
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_5_get_typescript_isa(namespace)
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_5_get_typescript_asa(namespace)
    pass
    
assert not isinstance(CustomClassCodecImpl, UsableAsKeyProto)

@dataclass
class ClassCodecImpl:
    t:type
    attr_codecs:dict[str,AttrCodec]

    def encode(self,x) -> JsonType:
        'encode {x} as a {self.t}'
        try:
            if not isinstance(x, self.t):
                xt=type(x)
                raise Exception(f'{x!r} (of type {xt}) is not a {self.t}')
            result={}
            for n, attr_codec in self.attr_codecs.items():
                try:
                    result[attr_codec.encoded_name.value()]=attr_codec.codec.encode(
                        getattr(x,n))
                except Exception:
                    raise in_context(f'encode attribute {n}') from None
                pass
            return result
        except Exception:
            raise in_function_context(ClassCodecImpl.encode,vars()) from None
        pass
    def decode(self,x) -> object:
        'decode {x} as a {self.t}'
        try:
            attr_values={}
            for n, attr_codec in self.attr_codecs.items():
                encoded_name = attr_codec.encoded_name.value()
                if encoded_name in x:
                    try:
                        value=attr_codec.codec.decode(x[encoded_name])
                        attr_values[n]=value
                        pass
                    except Exception:
                        raise in_context(f'decode attribute {n}') from None
                    pass
                pass
            try:
                return self.t(**attr_values)
            except Exception:
                raise in_context(f'init {self.t} with keyword arguments {attr_values}') from None
        except Exception:
            raise in_function_context(ClassCodecImpl.decode,vars()) from None
        pass
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        return get_type_fqn(self.t)
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
                'description': self.t.__name__,
                'type': 'object',
                'properties': {
                    attr_codec.encoded_name.value(): attr_codec.codec.get_json_schema(definitions)
                    for n, attr_codec in self.attr_codecs.items()
                }
            }
            pass
        return {
            '$ref': self_ref
        }
    def typescript_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(self.get_type_fqn())
    def ensure_typescript_defs(self,namespace)->None:
        '''ensure {namespace} has a typescript definition for {self.get_type_fqn()}'''
        typescript_fqn=[TypeScriptUQN(_) for _ in self.get_type_fqn().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        typescript_type_name=typescript_fqn[-1]
        tt=self.typescript_type()
        for attr_codec in self.attr_codecs.values():
            attr_codec.codec.ensure_typescript_defs(namespace)
        if typescript_type_name not in target_namespace.defs:
            target_namespace.defs[typescript_type_name]=TypeScriptSourceCode(
                f"type {typescript_type_name} = {{\n"+
                ''.join([f"    {attr_codec.encoded_name.value()}: {indent(4,attr_codec.codec.typescript_type())};\n"
                         for attr_name, attr_codec in self.attr_codecs.items()])+
                f"}};")
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"function asInstanceOf{typescript_type_name}(v:any): {tt}\n"
                f"{{\n"
                f"    _asInstanceOf{typescript_type_name}.f(v).applyDefaults();\n"
                f"    return v as {tt};\n"
                f"}}\n")
            target_namespace.defs[TypeScriptUQN(f"isInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"function isInstanceOf{typescript_type_name}(v:any): v is {tt}\n"
                f"{{\n"
                f"    const r:false|xju.json_codec.ApplyDefaults=_isInstanceOf{typescript_type_name}(v);\n"
                f"    return r && r.applyDefaults();\n"
                f"}}\n")
            attrs=[indent(4, TypeScriptSourceCode(
                f"{{propertyName: '{attr_codec.encoded_name}', asInstance: {attr_codec.codec.get_typescript_asa(namespace)}, defaultValue: {json.dumps(attr_codec.codec.encode(attr_codec.default_value))}}}"
                if isinstance(attr_codec, AttrCodecWithDefault) else
                f"{{propertyName: '{attr_codec.encoded_name}', asInstance: {attr_codec.codec.get_typescript_asa(namespace)}}}"))
                   for attr_codec in self.attr_codecs.values()]
            target_namespace.defs[TypeScriptUQN(f"_asInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"const _asInstanceOf{typescript_type_name} = xju.json_codec.asInstanceOfClass('{tt}', [\n"
                f"    {',\n    '.join(attrs)}\n"
                f"]);\n")
            attrs=[indent(4, TypeScriptSourceCode(
                f"{{propertyName:'{attr_codec.encoded_name}', isInstance:{attr_codec.codec.get_typescript_isa(namespace)}, defaultValue: {json.dumps(attr_codec.codec.encode(attr_codec.default_value))}}}"
                if isinstance(attr_codec, AttrCodecWithDefault) else
                f"{{propertyName:'{attr_codec.encoded_name}', isInstance:{attr_codec.codec.get_typescript_isa(namespace)}}}" 
            ))
                   for attr_codec in self.attr_codecs.values()]
            target_namespace.defs[TypeScriptUQN(f"_isInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"const _isInstanceOf{typescript_type_name} = xju.json_codec.isInstanceOfClass('{tt}', [\n"
                f"    {',\n    '.join(attrs)}\n"
                f"]);\n")
            pass
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return is_instance_of_expression(self.get_type_fqn())

    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return as_instance_of_expression(self.get_type_fqn())
    pass
    
assert not isinstance(ClassCodecImpl, UsableAsKeyProto)

@dataclass
class GenericClassUsableAsObjectKeyCodecImpl:
    t:type[CustomStringKeyGenericClassCodec] | type[CustomNonStringKeyGenericClassCodec]
    type_var_map:dict[TypeVar,type]

    def encode(self,x) -> JsonType:
        'encode {x} as a {self.t}'
        try:
            if not isinstance(x, self.t):
                xt=type(x)
                raise Exception(f'{x!r} (of type {xt}) is not a {self.t}')
            return self.t.xju_json_codec_encode_generic(x,self.type_var_map)
        except Exception:
            raise in_function_context(GenericClassUsableAsObjectKeyCodecImpl.encode,vars()) from None
        pass
    def decode(self,x) -> object:
        'decode {x} as a {self.t}'
        try:
            result=self.t.xju_json_codec_decode_generic(x,self.type_var_map)
            assert isinstance(result,self.t), (repr(result),self.t)
            return result
        except Exception:
            raise in_function_context(GenericClassUsableAsObjectKeyCodecImpl.decode,vars()) from None
        pass
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        return get_type_fqn(self.t)
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        return self.t.xju_json_codec_get_json_schema_generic(definitions,self.type_var_map)
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        return self.t.xju_json_codec_get_object_key_json_schema_generic(definitions,self.type_var_map)
    def typescript_type(self) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_get_typescript_type_generic(self.type_var_map)
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"string /* {self.t.xju_json_codec_get_typescript_type_generic(self.type_var_map).replace('/*','**').replace('*/','**')} */")
    def ensure_typescript_defs(self,namespace)->None:
        '''ensure {namespace} has a typescript definition for {self.get_type_fqn()}'''
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_5_get_typescript_isa_generic(namespace,self.type_var_map)

    def get_typescript_isa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_5_get_typescript_isa_key_generic(namespace,self.type_var_map)
        
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_5_get_typescript_asa_generic(namespace,self.type_var_map)
    def get_typescript_asa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_5_get_typescript_asa_key_generic(namespace,self.type_var_map)
    pass
    
assert isinstance(GenericClassUsableAsObjectKeyCodecImpl, UsableAsKeyProto)

@dataclass
class ClassUsableAsObjectKeyCodecImpl:
    t:type[CustomStringKeyClassCodec] | type[CustomNonStringKeyClassCodec]

    def encode(self,x) -> JsonType:
        'encode {x} as a {self.t}'
        try:
            if not isinstance(x, self.t):
                xt=type(x)
                raise Exception(f'{x!r} (of type {xt}) is not a {self.t}')
            return self.t.xju_json_codec_encode(x)
        except Exception:
            raise in_function_context(ClassUsableAsObjectKeyCodecImpl.encode,vars()) from None
        pass
    def decode(self,x) -> object:
        'decode {x} as a {self.t}'
        try:
            result=self.t.xju_json_codec_decode(x)
            assert isinstance(result,self.t), (repr(result),self.t)
            return result
        except Exception:
            raise in_function_context(ClassUsableAsObjectKeyCodecImpl.decode,vars()) from None
        pass
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        return get_type_fqn(self.t)
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        return self.t.xju_json_codec_get_json_schema(definitions)
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        return self.t.xju_json_codec_get_object_key_json_schema(definitions)
    def typescript_type(self) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_get_typescript_type()
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"string /* {self.t.xju_json_codec_get_typescript_type().replace('/*','**').replace('*/','**')} */")
    def ensure_typescript_defs(self,namespace)->None:
        '''ensure {namespace} has a typescript definition for {self.get_type_fqn()}'''
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_5_get_typescript_isa(namespace)

    def get_typescript_isa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_5_get_typescript_isa_key(namespace)
        
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_5_get_typescript_asa(namespace)
    def get_typescript_asa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.t.xju_json_codec_5_get_typescript_asa_key(namespace)
    pass
    
assert isinstance(ClassUsableAsObjectKeyCodecImpl, UsableAsKeyProto)

@dataclass
class EnumValueCodecImpl:
    name:str
    t:EnumMeta
    value:Enum
    value_codec:CodecImplProto

    def encode(self,x) -> JsonType:
        'encode {x!r} as a {self.t.__name__}.{self.value.name}'
        try:
            if x != self.value:
                raise Exception(f"{x} != {self.value}")
            return self.value_codec.encode(x.value)
        except Exception:
            raise in_function_context(EnumValueCodecImpl.encode,vars()) from None
        pass
    def decode(self,x) -> Enum:
        'decode {x!r} as {self.t.__name__}.{self.value.name}'
        try:
            value=self.value_codec.decode(x)
            if value != self.value.value:
                raise Exception(f'{value!r} is not {self.value.value!r}')
            return self.value
        except Exception:
            raise in_function_context(EnumValueCodecImpl.decode,vars()) from None
        pass
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=f"{self.get_type_fqn()}.{self.name}"
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            defn = self.value_codec.get_json_schema(definitions)
            defn.update({
                'enum': [ self.value_codec.encode(self.value.value) ]
            })
            definitions[fqn]=defn
            pass
        return {
            '$ref': self_ref
        }
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        result = self.value_codec.get_object_key_json_schema(definitions)
        result.update({
            'enum': [ str(self.value_codec.encode(self.value.value)) ]
        })
        return result
    def get_type_fqn(self)->str:
        '''get the fully qualified name of {self.t}'''
        return get_type_fqn(self.t)
    @property
    def is_typescript_pod(self) -> bool:
        v=self.value_codec.encode(self.value.value)
        return type(v) in (float, int, str)
    def typescript_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f'{self.get_type_fqn()}.{self.value.name}') if self.is_typescript_pod else self.typescript_value()
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f'{self.get_type_fqn()}.{self.value.name}') if self.is_typescript_pod else self.typescript_value()
    def typescript_value(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(json.dumps(self.value_codec.encode(self.value.value)))
    def ensure_typescript_defs(self,namespace: TypeScriptNamespace)->None:
        pass
    def get_typescript_isa(
            self,
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f"xju.json_codec.isInstanceOfLiteral({self.typescript_type()})")
    def get_typescript_isa_key(
            self,
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f"xju.json_codec.isKeyOfLiteral({self.typescript_type()})")
    def get_typescript_asa(
            self,
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f"xju.json_codec.asInstanceOfLiteral({self.typescript_type()})")
    def get_typescript_asa_key(
            self,
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f"xju.json_codec.asKeyOfLiteral({self.typescript_type()})")
    pass
    
assert isinstance(EnumValueCodecImpl, UsableAsKeyProto)

class EnumCodecImpl:
    def __init__(self,t:EnumMeta, value_codecs:dict[str, EnumValueCodecImpl]):
        self.t=t
        self.value_codecs=value_codecs
        pass
    def encode(self,x:Enum) -> JsonType:
        'encode {self.t} value {x!r}'
        try:
            if not isinstance(x, self.t):
                raise Exception(f'{x} (of type {x.__class__.__name__}) is not a {self.t.__name__}')
            return self.value_codecs[x.name].encode(x)
        except Exception:
            raise in_function_context(EnumCodecImpl.encode,vars()) from None
        pass
    def decode(self,x) -> Enum:
        '''decode {x!r} as enum {self.t.__name__} value'''
        try:
            exceptions=list[BaseException]()
            for n, c in self.value_codecs.items():
                try:
                    return c.decode(x)
                except Exception:
                    exceptions.append(in_context(f'decode as {self.t.__name__}.{n}'))
                    pass
                pass
            raise AllFailed(exceptions)
        except Exception:
            raise in_function_context(EnumCodecImpl.decode,vars()) from None
        pass
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
            "oneOf": [
                codec.get_json_schema(definitions)
                for _name,codec in self.value_codecs.items() ]
            }
            pass
        return {
            '$ref': self_ref
        }
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        return {
            "oneOf": [
                codec.get_object_key_json_schema(definitions)
                for _t,codec in self.value_codecs.items() ]
        }
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        return get_type_fqn(self.t)

    def typescript_type(self)->TypeScriptSourceCode:
        return TypeScriptSourceCode(self.get_type_fqn())
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f"string /* {self.typescript_type().replace('/*','**').replace('*/','**')} */")
    def ensure_typescript_defs(self, namespace) -> None:
        for c in self.value_codecs.values():
            c.ensure_typescript_defs(namespace)
            pass
        typescript_fqn=[TypeScriptUQN(_) for _ in self.get_type_fqn().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        tt=typescript_fqn[-1]
        if tt not in target_namespace.defs:
            enum_value_codec:EnumValueCodecImpl
            target_namespace.defs[tt]=TypeScriptSourceCode(
                f"enum {tt} {{\n"+
                ',\n'.join([f'    {name} = {enum_value_codec.typescript_value().value()}'
                            for name, enum_value_codec in self.value_codecs.items()])+
                f'\n}};\n')
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{tt}")]=TypeScriptSourceCode(
                f"function asInstanceOf{tt}(v: any): {tt}\n"
                f"{{\n"
                f"    const r=_asInstanceOf{tt}.f(v);\n"
                f"    r.applyDefaults();\n"
                f"    return v as {tt};\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f'isInstanceOf{tt}')]=TypeScriptSourceCode(
                f"function isInstanceOf{tt}(v:any): v is string\n"
                f"{{\n"
                f"    const r:false|xju.json_codec.ApplyDefaults=_isInstanceOf{tt}(v);\n"
                f"    return r&&r.applyDefaults();\n"
                f"}}")
            asas=[f"    {indent(4,vc.get_typescript_asa(namespace))}"
                  for _t,vc in self.value_codecs.items()]
            target_namespace.defs[TypeScriptUQN(f"_asInstanceOf{tt}")]=TypeScriptSourceCode(
                f"const _asInstanceOf{tt} = xju.json_codec.asInstanceOfUnion([\n"
                f"    {',\n'.join(asas)}\n"
                f"]);")
            
            isas=[f"    {indent(4,vc.get_typescript_isa(namespace))}"
                  for _t,vc in self.value_codecs.items()]
            target_namespace.defs[TypeScriptUQN(f"_isInstanceOf{tt}")]=TypeScriptSourceCode(
                f"const _isInstanceOf{tt} = xju.json_codec.isInstanceOfUnion([\n"
                f"    {',\n'.join(isas)}\n"
                f"]);")

            asas=[f"    {indent(4,vc.get_typescript_asa_key(namespace))}"
                  for _t,vc in self.value_codecs.items()]
            target_namespace.defs[TypeScriptUQN(f"_asKeyOf{tt}")]=TypeScriptSourceCode(
                f"const _asKeyOf{tt} = xju.json_codec.asKeyOfUnion([\n"
                f"    {',\n'.join(asas)}\n"
                f"]);")

            isas=[f"    {indent(4,vc.get_typescript_isa_key(namespace))}"
                  for _t,vc in self.value_codecs.items()]
            target_namespace.defs[TypeScriptUQN(f"_isKeyOf{tt}")]=TypeScriptSourceCode(
                f"const _isKeyOf{tt} = xju.json_codec.isKeyOfUnion([\n"
                f"    {',\n'.join(isas)}\n"
                f"]);")

            pass
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return is_instance_of_expression(self.get_type_fqn())
    def get_typescript_isa_key(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return is_key_of_expression(self.get_type_fqn())
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return as_instance_of_expression(self.get_type_fqn())
    def get_typescript_asa_key(self,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return as_key_of_expression(self.get_type_fqn())
    pass

assert isinstance(EnumCodecImpl, UsableAsKeyProto)

class LiteralEnumCodecImpl:
    def __init__(self,t:EnumMeta, value_codecs:dict[str, EnumValueCodecImpl]):
        self.t=t
        self.value_codecs=value_codecs
        pass
    def encode(self,x:Enum) -> JsonType:
        'encode {self.t} value {x!r}'
        try:
            Assert(x).isInstanceOf(self.t)
            return self.value_codecs[x.name].encode(x)
        except Exception:
            raise in_function_context(LiteralEnumCodecImpl.encode,vars()) from None
        pass
    def decode(self,x) -> Enum:
        '''decode {x!r} as enum {self.t.__name__} value'''
        try:
            exceptions=list[BaseException]()
            for n, c in self.value_codecs.items():
                try:
                    return c.decode(x)
                except Exception:
                    exceptions.append(in_context(f'decode as {self.t.__name__}.{n}'))
                    pass
                pass
            raise AllFailed(exceptions)
        except Exception:
            raise in_function_context(LiteralEnumCodecImpl.decode,vars()) from None
        pass
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]={
            "oneOf": [
                codec.get_json_schema(definitions)
                for _name,codec in self.value_codecs.items() ]
            }
            pass
        return {
            '$ref': self_ref
        }
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        return get_type_fqn(self.t)

    def typescript_type(self)->TypeScriptSourceCode:
        return TypeScriptSourceCode(self.get_type_fqn())
    def ensure_typescript_defs(self, namespace) -> None:
        for c in self.value_codecs.values():
            c.ensure_typescript_defs(namespace)
            pass
        typescript_fqn=[TypeScriptUQN(_) for _ in self.get_type_fqn().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        tt=typescript_fqn[-1]
        if tt not in target_namespace.defs:
            enum_value_codec:EnumValueCodecImpl
            target_namespace.defs[tt]=TypeScriptSourceCode(
                f"type {tt} ="+
                ' |'.join([f'\n    {enum_value_codec.typescript_value().value()}'
                           for enum_value_codec in self.value_codecs.values()])+
                f';\n')
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{tt}")]=TypeScriptSourceCode(
                f"function asInstanceOf{tt}(v: any): {tt}\n"
                f"{{\n"
                f"    const r=_asInstanceOf{tt}.f(v);\n"
                f"    r.applyDefaults();\n"
                f"    return v as {tt};\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f'isInstanceOf{tt}')]=TypeScriptSourceCode(
                f"function isInstanceOf{tt}(v:any): v is string\n"
                f"{{\n"
                f"    const r:false|xju.json_codec.ApplyDefaults=_isInstanceOf{tt}(v);\n"
                f"    return r&&r.applyDefaults();\n"
                f"}}")
            asas=[f"    {indent(4,vc.get_typescript_asa(namespace))}"
                  for _t,vc in self.value_codecs.items()]
            target_namespace.defs[TypeScriptUQN(f"_asInstanceOf{tt}")]=TypeScriptSourceCode(
                f"const _asInstanceOf{tt} = xju.json_codec.asInstanceOfUnion([\n"
                f"    {',\n'.join(asas)}\n"
                f"]);")
            
            isas=[f"    {indent(4,vc.get_typescript_isa(namespace))}"
                  for _t,vc in self.value_codecs.items()]
            target_namespace.defs[TypeScriptUQN(f"_isInstanceOf{tt}")]=TypeScriptSourceCode(
                f"const _isInstanceOf{tt} = xju.json_codec.isInstanceOfUnion([\n"
                f"    {',\n'.join(isas)}\n"
                f"]);")
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return is_instance_of_expression(self.get_type_fqn())
    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return as_instance_of_expression(self.get_type_fqn())
    pass

assert not isinstance(LiteralEnumCodecImpl, UsableAsKeyProto)

@dataclass
class TypeAliasCodecImpl:
    t:type
    value_codec:CodecImplProto

    def encode(self,x) -> JsonType:
        'encode {x} as a {self.t}'
        try:
            return self.value_codec.encode(x)
        except Exception:
            raise in_function_context(TypeAliasCodecImpl.encode,vars()) from None
        pass
    def decode(self,x) -> object:
        'decode {x} as a {self.t}'
        try:
            return self.value_codec.decode(x)
        except Exception:
            raise in_function_context(TypeAliasCodecImpl.decode,vars()) from None
        pass
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        return get_type_fqn(self.t)
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        fqn=self.get_type_fqn()
        self_ref=f'#/definitions/{fqn}'
        if not fqn in definitions:
            definitions[fqn]=self.value_codec.get_json_schema(definitions)
            pass
        return {
            '$ref': self_ref
        }
    def typescript_type(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(self.get_type_fqn())
    def ensure_typescript_defs(self,namespace)->None:
        '''ensure {namespace} has a typescript definition for {self.get_type_fqn()}'''
        typescript_fqn=[TypeScriptUQN(_) for _ in self.get_type_fqn().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        typescript_type_name=typescript_fqn[-1]
        tt=self.typescript_type()
        self.value_codec.ensure_typescript_defs(namespace)
        if typescript_type_name not in target_namespace.defs:
            target_namespace.defs[typescript_type_name]=TypeScriptSourceCode(
                f"type {typescript_type_name} = {indent(4,self.value_codec.typescript_type())};")
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"function asInstanceOf{typescript_type_name}(v:any): {tt}\n"
                f"{{\n"
                f"    _asInstanceOf{typescript_type_name}.f(v).applyDefaults();\n"
                f"    return v as {tt};\n"
                f"}}\n")
            target_namespace.defs[TypeScriptUQN(f"isInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"function isInstanceOf{typescript_type_name}(v:any): v is {tt}\n"
                f"{{\n"
                f"    const r:false|xju.json_codec.ApplyDefaults=_isInstanceOf{typescript_type_name}(v);\n"
                f"    return r && r.applyDefaults();\n"
                f"}}\n")
            target_namespace.defs[TypeScriptUQN(f"_asInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"const _asInstanceOf{typescript_type_name} = {self.value_codec.get_typescript_asa(namespace)};\n")
            target_namespace.defs[TypeScriptUQN(f"_isInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"const _isInstanceOf{typescript_type_name} = {self.value_codec.get_typescript_isa(namespace)};\n")
            pass
        pass
    def get_typescript_isa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return is_instance_of_expression(self.get_type_fqn())

    def get_typescript_asa(self,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        return as_instance_of_expression(self.get_type_fqn())
    pass
    
assert not isinstance(ClassCodecImpl, UsableAsKeyProto)

@dataclass
class BackRefCodecImpl:
    codec: CodecImplProto | None
    def encode(self,x:Any)->JsonType:
        assert self.codec is not None
        return self.codec.encode(x)
    def decode(self,x:JsonType)->Any:
        assert self.codec is not None
        return self.codec.decode(x)
    def get_type_fqn(self) -> str:
        assert self.codec is not None
        return self.codec.get_type_fqn()
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        assert self.codec is not None
        fqn=self.get_type_fqn()
        return  {
            '$ref': f'#/definitions/{fqn}'
        }
    def typescript_type(self) -> TypeScriptSourceCode:
        assert self.codec is not None
        return self.codec.typescript_type()
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        assert self.codec is not None
        return TypeScriptSourceCode(f"(x:any) => {self.codec.get_typescript_isa(namespace)}(x)")
    def get_typescript_asa(self,namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        assert self.codec is not None
        return TypeScriptSourceCode("\n".join([
            f"{{"
            ,f"    typeName: {json.dumps(str(self.typescript_type()))},"
            ,f"    f:(x:any) => {indent(4,self.codec.get_typescript_asa(namespace))}.f(x)"
            ,f"}}"]))

def _explodeSchema(
        t:type|NewType|TypeVar|GenericAlias|UnionType|_LiteralGenericAlias|_GenericAlias|Enum|Type[Enum],
        type_var_map:dict[TypeVar,Any]|None,
        codec_backrefs: dict[type|NewType|TypeVar|GenericAlias|UnionType|_LiteralGenericAlias|_GenericAlias|Enum|Type[Enum], BackRefCodecImpl]
) -> CodecImplProto:
    '''explode type {t!r} into a tree of codecs using map {type_var_map} to resolve any generic type refs i.e. TypeVars'''
    match codec_backrefs.get(t):
        case None:
            backref=BackRefCodecImpl(None)
            codec_backrefs[t] = backref
            try:
                result=_explodeSchemaRec(t, type_var_map, codec_backrefs)
                backref.codec=result
                return result
            finally:
                del codec_backrefs[t]
                pass
        case b:
            return b
    pass

def _explodeSchemaRec(
        t:type|NewType|TypeVar|GenericAlias|UnionType|_LiteralGenericAlias|_GenericAlias|Enum|Type[Enum],
        type_var_map:dict[TypeVar,Any]|None,
        codec_backrefs: dict[type|NewType|TypeVar|GenericAlias|UnionType|_LiteralGenericAlias|_GenericAlias|Enum|Type[Enum], BackRefCodecImpl],
) -> CodecImplProto:
    '''explode type {t!r} into a tree of codecs using map {type_var_map} to resolve any generic type refs i.e. TypeVars'''
    try:
        if type(t) is TypeVar:
            assert type_var_map is not None
            assert t in type_var_map, type_var_map.keys()
            return _explodeSchema(type_var_map[t],type_var_map,codec_backrefs)
        if t is float or (isinstance(t,NewType) and t.__supertype__ is float):
            return NoopCodecImpl[float](float)
        if t is int or (isinstance(t,NewType) and t.__supertype__ is int):
            return NoopCodecImpl[int](int)
        if t is str or (isinstance(t,NewType) and t.__supertype__ is str):
            return NoopCodecImpl[str](str)
        if t is bytes or (isinstance(t,NewType) and t.__supertype__ is bytes):
            return BytesCodecImpl(_explodeSchema(list[int],type_var_map,codec_backrefs))
        if t is bool or (isinstance(t,NewType) and t.__supertype__ is bool):
            return NoopCodecImpl[bool](bool)
        if t is None or t is NoneType:
            return NoneCodecImpl()
        if t is list:
            return AnyListCodecImpl()
        if type(t) is GenericAlias and get_origin(t) is list:
            return ListCodecImpl(_explodeSchema(get_args(t)[0],type_var_map,codec_backrefs))
        if t is frozenset:
            return AnyFrozenSetCodecImpl()
        if t is set:
            return AnySetCodecImpl()
        if type(t) is GenericAlias and get_origin(t) is frozenset:
            return FrozenSetCodecImpl(_explodeSchema(get_args(t)[0],type_var_map,codec_backrefs))
        if type(t) is GenericAlias and get_origin(t) is set:
            return SetCodecImpl(_explodeSchema(get_args(t)[0],type_var_map,codec_backrefs))
        if type(t) is _LiteralGenericAlias and len(get_args(t))==1:
            return _explode_literal(get_args(t)[0],type_var_map,codec_backrefs)
        if type(t) is _LiteralGenericAlias:
            return UnionCodecImpl(get_args(t),
                                  {at:_explode_literal(at,type_var_map,codec_backrefs)
                                   for at in get_args(t)})
        if type(t) is GenericAlias and get_origin(t) is tuple:
            return TupleCodecImpl([_explodeSchema(_,type_var_map,codec_backrefs) for _ in get_args(t)])
        if t is dict:
            return AnyDictCodecImpl()
        if type(t) is GenericAlias and get_origin(t) is dict:
            return DictCodecImpl(
                usable_as_key(_explodeSchema(get_args(t)[0], type_var_map,codec_backrefs)),
                _explodeSchema(get_args(t)[1], type_var_map,codec_backrefs)
            )
        if type(t) is UnionType:
            return UnionCodecImpl(get_args(t),
                                  {at:_explodeSchema(at, type_var_map,codec_backrefs) for at in get_args(t)})
        if type(t) is _UnionGenericAlias:
            return UnionCodecImpl(get_args(t),
                                  {at:_explodeSchema(at, type_var_map,codec_backrefs) for at in get_args(t)})
        # note t Self is handled in _explodeSchema

        if type(t) is _GenericAlias or type(t) is GenericAlias:
            if type(get_origin(t)) is _SpecialForm and str(get_origin(t))=="typing.Final":
                return _explodeSchema(get_args(t)[0],type_var_map,codec_backrefs)
            local_type_var_map={
                type_var: value
                for type_var, value in
                list((type_var_map or {}).items())+list(zip(get_origin(t).__parameters__, get_args(t)))
            }
            return _explodeSchema(
                get_origin(t),
                local_type_var_map,codec_backrefs)
        if type(t) is EnumType and issubclass(t, Enum):
            value_codecs={
                name: EnumValueCodecImpl(
                    name,
                    vv.__class__,
                    vv,
                    _explodeSchema(type(vv.value),type_var_map,codec_backrefs))
                for name,vv in t.__members__.items()
            }
            use_typescript_enum = all([v.is_typescript_pod for v in value_codecs.values()])
            return (
                EnumCodecImpl(t,value_codecs) if use_typescript_enum
                else LiteralEnumCodecImpl(t,value_codecs)
            )
        if type(t) is TypeAliasType:
            return TypeAliasCodecImpl(t, _explodeSchema(t.__value__,type_var_map,codec_backrefs))
        assert isinstance(t,type), (type(t), t)
        if issubclass(t,xju.newtype.Int):
            return NewIntCodecImpl(t)
        if issubclass(t,xju.newtype.Float):
            return NewFloatCodecImpl(t)
        if issubclass(t,xju.newtype.Str):
            return NewStrCodecImpl(t)
        if issubclass(t,Timestamp):
            return TimestampCodecImpl()
        dont_encode=dont_encode_attr_map.get(t)
        old_self=codec_backrefs.get(Self,None)
        self_ref=BackRefCodecImpl(None)
        codec_backrefs[Self]=self_ref
        try:
            if issubclass(t, (CustomStringKeyGenericClassCodec,CustomNonStringKeyGenericClassCodec)):
                result=GenericClassUsableAsObjectKeyCodecImpl(t,type_var_map)
                self_ref.codec=result
                return result
            if issubclass(t, CustomGenericClassCodec):
                result=CustomGenericClassCodecImpl(t,type_var_map)
                self_ref.codec=result
                return result
            if issubclass(t, (CustomStringKeyClassCodec,CustomNonStringKeyClassCodec)):
                result=ClassUsableAsObjectKeyCodecImpl(t)
                self_ref.codec=result
                return result
            if issubclass(t, CustomClassCodec):
                result=CustomClassCodecImpl(t)
                self_ref.codec=result
                return result
            else:
                result=ClassCodecImpl(
                    t,{n: make_attr_codec(t, n, nt, type_var_map, codec_backrefs)
                       for n,nt in get_type_hints(t).items()
                       if not ((type(nt) is _GenericAlias and type(get_origin(nt)) is _SpecialForm and str(get_origin(nt))=="typing.ClassVar") or
                               (dont_encode is not None and PythonAttrName(n) in dont_encode))})
                self_ref.codec=result
                return result
        finally:
            if old_self:
                codec_backrefs[Self]=old_self
            else:
                del codec_backrefs[Self]
                pass
            pass
    except Exception:
        raise in_function_context(_explodeSchemaRec,vars()) from None
    pass

def _explode_literal(
        value: Any,
        type_var_map:dict[TypeVar,Any]|None,
        codec_backrefs: dict[type|NewType|TypeVar|GenericAlias|UnionType|_LiteralGenericAlias|_GenericAlias|Enum|Type[Enum], BackRefCodecImpl]
) -> CodecImplProto:
    '''create codec for literal value {value!r}'''
    try:
        if type(value) is str:
            return LiteralStrCodecImpl(value)
        if type(value) is bool:
            return LiteralBoolCodecImpl(value)
        if type(value) is int:
            return LiteralIntCodecImpl(value)
        if isinstance(value, Enum):
            return EnumValueCodecImpl(
                value.name,
                value.__class__,value,
                _explodeSchema(type(value.value),type_var_map,codec_backrefs))
        t=type(value)
        raise Exception(f'{t} literals are not supported (only support str, int, bool, Enum)')
    except:
        raise in_function_context(_explode_literal, vars())
    pass

def indent(n: int, s:TypeScriptSourceCode)->str:
    lines=s.splitlines()
    if lines:
        return '\n'.join([lines[0].value()]+[(' '*n)+l.value() for l in lines[1:]])
    else:
        return ''

def get_type_fqn(t:type) -> str:
    if str(t.__module__) in ('__main__', 'builtins', 'typing'):
        return t.__name__.removeprefix('builtins.').removeprefix('typing.')
    return f"{t.__module__}.{t.__name__}"

def is_instance_of_expression(type_fqn:str) -> TypeScriptSourceCode:
    """get a.b.c._isInstanceOfX() source code for {type_fqn} like a.b.c.X"""
    parts=type_fqn.split('.')
    return TypeScriptSourceCode('.'.join(parts[0:-1]+[f"_isInstanceOf{parts[-1]}"]))

def is_key_of_expression(type_fqn:str) -> TypeScriptSourceCode:
    """get a.b.c._isKeyOfX() source code for {type_fqn} like a.b.c.X"""
    parts=type_fqn.split('.')
    return TypeScriptSourceCode('.'.join(parts[0:-1]+[f"_isKeyOf{parts[-1]}"]))

def as_instance_of_expression(type_fqn:str) -> TypeScriptSourceCode:
    """get a.b.c._asInstanceOfX() source code for {type_fqn} like a.b.c.X"""
    parts=type_fqn.split('.')
    return TypeScriptSourceCode('.'.join(parts[0:-1]+[f"_asInstanceOf{parts[-1]}"]))

def as_key_of_expression(type_fqn:str) -> TypeScriptSourceCode:
    """get a.b.c.asInstanceOfKeyOfX() source code for {type_fqn} like a.b.c.X"""
    parts=type_fqn.split('.')
    return TypeScriptSourceCode('.'.join(parts[0:-1]+[f"_asKeyOf{parts[-1]}"]))

def get_default_value(cls, attr_name: str) -> None | tuple[Any]:
    if hasattr(cls, '_field_defaults'):  # NamedTuple
        if attr_name in cls._field_defaults:
            return (cls._field_defaults[attr_name],)
        return None
    if hasattr(cls, "__dataclass_fields__") and attr_name in cls.__dataclass_fields__:
        field: Field = cls.__dataclass_fields__[attr_name]
        if field.default_factory is not MISSING:
            return (field.default_factory(),)
        if field.default is not MISSING:
            return (field.default,)
        return None
    elif hasattr(cls, attr_name):
        return (getattr(cls,attr_name),)
    return None

def make_attr_codec(
        t:type,
        attr_name: str, nt:type,
        type_var_map:dict[TypeVar,Any]|None,
        codec_backrefs: dict[
            type|NewType|TypeVar|GenericAlias|UnionType|_LiteralGenericAlias|_GenericAlias|Enum|Type[Enum], BackRefCodecImpl
        ]
) -> AttrCodecWithDefault | AttrCodec:
    match get_default_value(t, attr_name):
        case None:
            return AttrCodec(get_attr_encoded_name(getmro(t), PythonAttrName(attr_name)),
                             _explodeSchema(nt,type_var_map,codec_backrefs))
        case tuple() as v:
            return AttrCodecWithDefault(get_attr_encoded_name(getmro(t), PythonAttrName(attr_name)),
                                        _explodeSchema(nt,type_var_map,codec_backrefs),
                                        v[0])
