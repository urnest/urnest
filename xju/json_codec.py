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
from dataclasses import dataclass
from inspect import getmro
from typing import NewType
from xju.time import Timestamp
from xju.xn import Xn,in_context,in_function_context,readable_repr, AllFailed
from typing import TypeVar, Generic, Type, cast, Any, Protocol, Self, Callable, get_type_hints
from typing import Sequence, Literal, NewType, Final, Mapping
from typing import _LiteralGenericAlias  # type: ignore  # mypy 1.1.1
from typing import _UnionGenericAlias  # type: ignore  # mypy 1.1.1
from typing import _GenericAlias,_SpecialForm  # type: ignore  # mypy 1.2.0
from typing import get_origin,get_args, runtime_checkable
from types import GenericAlias, UnionType, NoneType
import xju.newtype
from enum import Enum, EnumType, EnumMeta
import json
from xju import misc

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

@dataclass
class TypeScriptBackRefs:
    type_back_ref:Callable[[],TypeScriptSourceCode]
    as_object_key_type_back_ref:Callable[[],TypeScriptSourceCode]
    isa_back_ref:Callable[[TypeScriptSourceCode],TypeScriptSourceCode]
    asa_back_ref:Callable[[TypeScriptSourceCode],TypeScriptSourceCode]


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
    def typescript_type(self) -> TypeScriptSourceCode:
        '''return typescript unqualified equivalent type for T'''
        pass
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        '''return typescript unqualified equivalent type for T when used as an object key'''
        pass
    def ensure_typescript_defs(self, namespace:TypeScriptNamespace) -> None:
        '''ensure namespace {namespace} contains {self} typescript defs'''
        pass
    def add_typescript_alias(self, namespace:TypeScriptNamespace, fqn:Sequence[TypeScriptUQN]) -> None:
        '''add {fqn} as typescript alias for {self.t}'''
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''get typescript "{expression} is a T" code, adding any necessary definitions to {namespace}
           result is a type-guard expression, examples of some possible results with namespace "n":
             parameter       possible code                  note
             "x.y"           "(typeof (x.y) == 'number')"   (uses built-in type guard pattern)
             "z"             "n.isInstanceOfFred(z)"        (calls a type-guard function)
           ... note that in javascript all object keys are strings, but that python json module
           also allows int,bool,float and None: its dumps converts these to their javascript
           literal strings equivalent (so None becomes "null", Ture/False become "true"/"false").
        '''
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''get typescript "{expression} is a T" code when T is used an object key, adding any
           necessary definitions to {namespace}
           result is a type-guard expression, examples of some possible results with namespace "n":
             parameter       possible code                  note
             "x.y"           "(typeof (x.y) == 'number')"   (uses built-in type guard pattern)
             "z"             "n.isInstanceOfFred(z)"        (calls a type-guard function)
           ... note that in javascript all object keys are strings, but that python json module
           also allows int,bool,float and None: its dumps converts these to their javascript
           literal strings equivalent (so None becomes "null", Ture/False become "true"/"false").
        '''
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''get typescript "{expression} as a T" code, adding any necessary definitions to {namespace}
        
           result is a cast expression that throws an Error if {expression} is not a T

           ... note that in javascript all object keys are strings, but that python json module
           also allows int,bool,float and None: its dumps converts these to their javascript
           literal strings equivalent (so None becomes "null", Ture/False become "true"/"false").
        '''
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''get typescript "{expression} as a T" code when T is used as an object key, adding any
           necessary definitions to {namespace}
        
           result is a cast expression that throws an Error if {expression} is not a T

           ... note that in javascript all object keys are strings, but that python json module
           also allows int,bool,float and None: its dumps converts these to their javascript
           literal strings equivalent (so None becomes "null", Ture/False become "true"/"false").
        '''
        
def codec(t: Type[T]) -> CodecProto[T]:
    '''build codec to encode/decode a "t" to/from json

       - note t may be a Union (which is not a Type; use json_codec_mypy_plugin for
         proper typing and no errors with mypy
    '''
    return Codec[T](t)


# implementation of CodecProto - use codec above with json_codec_mypy_plugin
# rather than using this class directly
class Codec(Generic[T]):
    t:Type[T]
    def __init__(self, t: Type[T]):
        'initialse json decoder for type %(t)r'
        self.t=t
        self.codec:CodecImplProto=_explodeSchema(self.t, {}, {})
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
    def typescript_type(self) -> TypeScriptSourceCode:
        '''return typescript unqualified equivalent type for T'''
        return self.codec.typescript_type(None)
    def typescript_as_object_key_type(self) -> TypeScriptSourceCode:
        return self.codec.typescript_as_object_key_type(None)
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
            target_namespace.defs[fqn[-1]]=TypeScriptSourceCode(
                f'type {fqn[-1]} = {self.typescript_type()};')
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{fqn[-1]}")] = TypeScriptSourceCode(
                f"function asInstanceOf{fqn[-1]}(v:any):{fqn[-1]} {{ return {self.codec.get_typescript_asa(TypeScriptSourceCode('v'),namespace,None)}; }}")
            target_namespace.defs[TypeScriptUQN(f"isInstanceOf{fqn[-1]}")] = TypeScriptSourceCode(
                f"function isInstanceOf{fqn[-1]}(v:any):v is {fqn[-1]} {{ return {self.codec.get_typescript_isa(TypeScriptSourceCode('v'),namespace,None)}; }}")
        except:
            raise in_function_context(Codec.add_typescript_alias,vars())
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.codec.get_typescript_isa(expression,namespace,None)
    def get_typescript_isa_key(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.codec.get_typescript_isa_key(expression,namespace,None)
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.codec.get_typescript_asa(expression,namespace,None)
    def get_typescript_asa_key(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        return self.codec.get_typescript_asa_key(expression,namespace,None)
    pass

class CodecImplProto(Protocol):
    def encode(self,x:Any)->JsonType:
        pass
    def decode(self,x:JsonType)->Any:
        pass
    def get_type_fqn(self) -> str:
        """get (python) type fully qualified name, e.g. 'a.b.C'"""
    def get_json_schema(self, definitions:dict[str,dict]) -> dict:
        pass
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        pass
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        pass
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        pass
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        pass
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        pass
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        pass
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        '''return typescript equivalent type for T'''
        if self.t is int: return TypeScriptSourceCode('number')
        if self.t is str: return TypeScriptSourceCode('string')
        if self.t is float: return TypeScriptSourceCode('number')
        assert self.t is bool, self.t
        return TypeScriptSourceCode('boolean')
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
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
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(typeof ({expression}) == '{self.typescript_type(back_refs)}')")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        if issubclass(self.t,bool):
            return TypeScriptSourceCode(
                f"(({expression}) === 'true' || ({expression}) === 'false')")
        elif issubclass(self.t,int):
            return TypeScriptSourceCode(
                f"(typeof ({expression}) == 'string' && !isNaN(parseInt({expression})))")
        elif issubclass(self.t,float):
            return TypeScriptSourceCode(
                f"(typeof ({expression}) == 'string' && !isNaN(parseFloat({expression})))")
        elif issubclass(self.t,str):
            return TypeScriptSourceCode(
                f"(typeof ({expression}) == 'string')")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (typeof v !== '{tt}') throw new Error(`${{v}} is not a {tt} it is a ${{typeof v}}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_as_object_key_type(back_refs)
        if issubclass(self.t,bool):
            return TypeScriptSourceCode(
                f"((v: any): {tt} => {{\n"
                f"    if (typeof v !== 'string' /* {self.typescript_type(back_refs)} */) throw new Error(`${{v}} is not a {tt} it is a ${{typeof v}}`);\n"
                f"    if (v !== 'true' && v !== 'false') throw new Error(`${{v}} is not 'true' or 'false'`);\n"
                f"    return v as {tt};\n"
                f"}})({expression})")
        elif issubclass(self.t,int):
            return TypeScriptSourceCode(
                f"((v: any): {tt} => {{\n"
                f"    if (typeof v !== 'string' /* {self.typescript_type(back_refs)} */) throw new Error(`${{v}} is not a {tt} it is a ${{typeof v}}`);\n"
                f"    if (isNaN(parseInt(v))) throw new Error(`${{v}} is not a stringified integer`);\n"
                f"    return v as {tt};\n"
                f"}})({expression})")
        elif issubclass(self.t,float):
            return TypeScriptSourceCode(
                f"((v: any): {tt} => {{\n"
                f"    if (typeof v !== 'string' /* {self.typescript_type(back_refs)} */) throw new Error(`${{v}} is not a {tt} it is a ${{typeof v}}`);\n"
                f"    if (isNaN(parseFloat(v))) throw new Error(`${{v}} is not a stringified number`);\n"
                f"    return v as {tt};\n"
                f"}})({expression})")
        elif issubclass(self.t,str):
            return TypeScriptSourceCode(
                f"((v: any): {tt} => {{\n"
                f"    if (typeof v !== 'string' /* {self.typescript_type(back_refs)} */) throw new Error(`${{v}} is not a string it is a ${{typeof v}}`);\n"
                f"    return v as {tt};\n"
                f"}})({expression})")
    pass

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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        '''return typescript equivalent type for None'''
        return TypeScriptSourceCode('null')
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        # python's json.dumps turns None into "null"
        return TypeScriptSourceCode('string /* null */')
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(({expression}) === {self.typescript_type(back_refs)})")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(({expression}) === 'null')")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"((v: any): null => {{\n"
            f"    if (v !== null) throw new Error(`${{v}} is not null it is a ${{typeof v}}`);\n"
            f"    return v as null;\n"
            f"}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"((v: any): null => {{\n"
            f"    if (v !== 'null') throw new Error(`key ${{v}} is not 'null'`);\n"
            f"    return v as null;\n"
            f"}})({expression})")
    pass

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
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        raise Exception("list is not allowed as json object key type")
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"Array<{self.value_codec.typescript_type(back_refs)}>")
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    def ensure_typescript_defs(self, namespace) -> None:
        self.value_codec.ensure_typescript_defs(namespace)
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"(((v:any):v is {tt}=>(Array.isArray(v) && "
            f"    v.filter((x)=>(\n"
            f"        !{indent(8,self.value_codec.get_typescript_isa(TypeScriptSourceCode('x'),namespace,back_refs))})).length==0))({expression}))")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
        
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{try{{\n"
            f"    if (!Array.isArray(v)) throw new Error(`${{v}} is not an array it is a ${{typeof v}}`);\n"
            f"    v.forEach((x)=>{indent(4,self.value_codec.get_typescript_asa(TypeScriptSourceCode('x'),namespace,back_refs))});\n"
            f"    return v as {tt};\n"
            f"}}catch(e:any){{throw new Error(`${{v}} is not a {tt} because ${{e}}`);}}}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    pass

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
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        raise Exception("list is not allowed as json object key type")
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"Array<any>")
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(Array.isArray({expression}))")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
        
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (!Array.isArray(v)) throw new Error(`${{v}} is not an Array it is a ${{typeof v}}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    pass

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
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        raise Exception("set is not allowed as json object key type")
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"Array<{self.value_codec.typescript_type(back_refs)}> /* with unique elements */")
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    def ensure_typescript_defs(self, namespace) -> None:
        self.value_codec.ensure_typescript_defs(namespace)
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"(((v:any):v is {tt}=>(\n"
            "     Array.isArray(v) && \n"
            f"    v.filter((x)=>(\n"
            f"        !{indent(8,self.value_codec.get_typescript_isa(TypeScriptSourceCode('x'),namespace,back_refs))})).length==0))({expression}))")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
        
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{try{{\n"
            f"    if (!Array.isArray(v)) throw new Error(`${{v}} is not an array it is a ${{typeof v}}`);\n"
            f"    v.forEach((x)=>{indent(4,self.value_codec.get_typescript_asa(TypeScriptSourceCode('x'),namespace,back_refs))});\n"
            f"    return v as {tt};\n"
            f"}}catch(e:any){{throw new Error(`${{v}} is not a {tt} because ${{e}}`);}}}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    pass

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
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        raise Exception("set is not allowed as json object key type")
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"Array<any> /* with unique elements */")
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(Array.isArray({expression}))")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
        
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (!Array.isArray(v)) throw new Error(`${{v}} is not an {tt} it is a ${{typeof v}}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    pass

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
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        raise Exception("frozenset is not allowed as json object key type")
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"Array<{self.value_codec.typescript_type(back_refs)}> /* with unique elements */")
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    def ensure_typescript_defs(self, namespace) -> None:
        self.value_codec.ensure_typescript_defs(namespace)
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"(((v:any):v is {tt}=>(\n"
            "     Array.isArray(v) && \n"
            f"    v.filter((x)=>(\n"
            f"        !{indent(8,self.value_codec.get_typescript_isa(TypeScriptSourceCode('x'),namespace,back_refs))})).length==0))({expression}))")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
        
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{try{{\n"
            f"    if (!Array.isArray(v)) throw new Error(`${{v}} is not an array it is a ${{typeof v}}`);\n"
            f"    v.forEach((x)=>{indent(4,self.value_codec.get_typescript_asa(TypeScriptSourceCode('x'),namespace,back_refs))});\n"
            f"    return v as {tt};\n"
            f"}}catch(e:any){{throw new Error(`${{v}} is not a {tt} because ${{e}}`);}}}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    pass

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
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        raise Exception("frozenset is not allowed as json object key type")
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"Array<any> /* with unique elements */")
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(Array.isArray({expression}))")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
        
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (!Array.isArray(v)) throw new Error(`${{v}} is not an {tt} it is a ${{typeof v}}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    pass

class TupleCodec:
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
            raise in_function_context(TupleCodec.encode,vars()) from None
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
            raise in_function_context(TupleCodec.decode,vars()) from None
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
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        raise Exception("tuple is not allowed as json object key type")
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->TypeScriptSourceCode:
        value_types=','.join([str(_.typescript_type(back_refs)) for _ in self.value_codecs])
        return TypeScriptSourceCode(f"[{value_types}]")
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    def ensure_typescript_defs(self, namespace) -> None:
        for c in self.value_codecs: c.ensure_typescript_defs(namespace)
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        isas=[' &&\n            '+indent(12,self.value_codecs[i].get_typescript_isa(f"v[{i}]",namespace,back_refs))
              for i in range(0, self.number_of_codecs)]
        self.ensure_typescript_defs(namespace)
        return TypeScriptSourceCode(
            f"((v:any): v is {self.typescript_type(back_refs)}=>{{\n"
            f"    if (Array.isArray(v)) {{\n" +
            f"        return v.length == {self.number_of_codecs}" +
            f"".join(isas) + ";\n"+
            f"    }}\n"+
            f"    else return false;\n"+
            f"}})({expression})")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
        
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        asas=['    '+indent(4,self.value_codecs[i].get_typescript_asa(f"v[{i}]",namespace,back_refs))+';\n'
              for i in range(0, self.number_of_codecs)]
        tt=self.typescript_type(back_refs)
        self.ensure_typescript_defs(namespace)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{try{{\n" +
            f"    if (!Array.isArray(v)) throw new Error(`${{v}} is not an array it is a ${{typeof v}}`);\n" +
            f"    if (v.length != {self.number_of_codecs}) throw new Error(`${{v}} does not have {self.number_of_codecs} elements (it has ${{v.length}} elements)`);\n" +
            ''.join(asas)+
            f"    return v as {tt};\n"+
            f"}}catch(e:any){{ throw new Error(`${{v}} is not a {tt} because ${{e}}`);}}}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    pass

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
            "oneOf": [ codec.get_object_key_json_schema(definitions) for t,codec in self.value_codecs.items() ]
        }
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->TypeScriptSourceCode:
        return TypeScriptSourceCode(
            '|'.join([str(c.typescript_type(back_refs)) for _t,c in self.value_codecs.items()]))
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        '''get typescript type of dictionary key that is any of {self.allowed_types}'''
        try:
            item_types=[str(c.typescript_as_object_key_type(back_refs)) for _t,c in self.value_codecs.items()]
            return TypeScriptSourceCode(f"string /* {'|'.join(item_types).replace('/*','**').replace('*/','**')} */")
        except Exception:
            raise in_function_context(UnionCodecImpl.typescript_as_object_key_type,vars()) from None
    def ensure_typescript_defs(self, namespace) -> None:
        for _,c in self.value_codecs.items():
            c.ensure_typescript_defs(namespace)
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        isas=[' ||\n       '+indent(12,value_codec.get_typescript_isa(TypeScriptSourceCode(f"v"),
                                                                      namespace,back_refs))
              for _t,value_codec in self.value_codecs.items()]
        return TypeScriptSourceCode(
            f"((v:any): v is {self.typescript_type(back_refs)}=>{{\n"
            f"    return false" +
            f"".join(isas) + ";\n"+
            f"}})({expression})")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        isas=[' ||\n       '+indent(12,value_codec.get_typescript_isa_key(TypeScriptSourceCode(f"v"),
                                                                          namespace,back_refs))
              for _t,value_codec in self.value_codecs.items()]
        return TypeScriptSourceCode(
            f"((v:any): v is {self.typescript_as_object_key_type(back_refs)}=>{{\n"
            f"    return false" +
            f"".join(isas) + ";\n"+
            f"}})({expression})")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        asas=[f"    try{{\n"+
              f"        {indent(8,value_codec.get_typescript_asa(TypeScriptSourceCode('v'),namespace,back_refs))};\n"+
              f"        return v as {tt};\n"+
              f"    }}\n"+
              f"    catch(e:any){{\n"+
              f"        es.push(e.message);\n"+
              f"    }};\n"
              for _t,value_codec in self.value_codecs.items()]
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{try{{\n" +
            f"    var es = new Array<string>();\n"+
            "".join(asas)+
            f"    throw new Error(es.join(' and '));\n"+
            f"}}catch(e:any)\n"+
            f"{{\n"+
            f"    throw new Error(`${{v}} is not a {tt} because ${{e}}`);\n"+
            f"}}}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_as_object_key_type(back_refs)
        asas=[f"    try{{\n"+
              f"        {indent(8,value_codec.get_typescript_asa_key(TypeScriptSourceCode('v'),namespace,back_refs))};\n"+
              f"        return v as {tt};\n"+
              f"    }}\n"+
              f"    catch(e:any){{\n"+
              f"        es.push(e.message);\n"+
              f"    }};\n"
              for _t,value_codec in self.value_codecs.items()]
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{try{{\n" +
            f"    var es = new Array<string>();\n"+
            "".join(asas)+
            f"    throw new Error(es.join(' and '));\n"+
            f"}}catch(e:any)\n"+
            f"{{\n"+
            f"    throw new Error(`${{v}} is not a {tt} because ${{e}}`);\n"+
            f"}}}})({expression})")
    pass

class DictCodecImpl:
    decode_key:Callable[[JsonType],object]
    def __init__(self, key_codec, value_codec):
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
            if not isinstance(ek, (str,bool,int,float,NoneType)):
                raise Exception(f'key encoder {self.key_codec} produced non-str/bool/int/float/None {ek} from dict key {k}')
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
        try:
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
        except Exception:
            raise in_function_context(DictCodecImpl.get_json_schema,vars()) from None
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        """get dict[{self.key_codec}:{self.value_codec}] object key json schema"""
        try:
            raise Exception("dict is not allowed as json object key type")
        except Exception:
            raise in_function_context(DictCodecImpl.get_object_key_json_schema,vars()) from None
    
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"{{ [key: {self.key_codec.typescript_as_object_key_type(back_refs)}]: {self.value_codec.typescript_type(back_refs)} }}")
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    def ensure_typescript_defs(self, namespace) -> None:
        self.key_codec.ensure_typescript_defs(namespace)
        self.value_codec.ensure_typescript_defs(namespace)
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(((x:any):x is {self.typescript_type(back_refs)}=>(\n"
            f"    x !== null &&\n"
            f"    typeof (x) === 'object' &&\n"
            f"    !Array.isArray(x) &&\n"
            f"    (():boolean=>{{for (const k in x){{\n"
            f"        if (!x.hasOwnProperty(k)) continue;\n"
            f"        if ((typeof k !== 'string') ||\n"
            f"            !({indent(8,self.key_codec.get_typescript_isa_key(TypeScriptSourceCode('k'),namespace,back_refs))}) ||\n"+
            f"            !({indent(8,self.value_codec.get_typescript_isa(TypeScriptSourceCode('x[k]'),namespace,back_refs))})) return false;}}\n"
            f"    return true;\n"
            f"}})()))({expression}))")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
        
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((x: any): {tt} => {{try{{\n"
            f"    if (x === null) throw new Error(`${{x}} is not an object it is null`);\n"
            f"    if (typeof x !== 'object') throw new Error(`${{x}} is not an object it is a ${{typeof x}}`);\n"
            f"    if (Array.isArray(x)) throw new Error(`${{x}} is not an object it is an array`);\n"
            f"    for(const k in x){{try{{\n"+
            f"        if (!x.hasOwnProperty(k)) continue;\n"+
            f"        const v=x[k];\n"
            f"        if (typeof k !== 'string') {{\n"+
            f"            const kt=typeof k;\n"+
            f"            throw new Error(`key type is ${{kt}} not string`);\n"+
            f"        }}\n"+
            f"        {indent(8,self.key_codec.get_typescript_asa_key(TypeScriptSourceCode('k'),namespace,back_refs))};\n"
            f"        {indent(8,self.value_codec.get_typescript_asa(TypeScriptSourceCode('v'),namespace,back_refs))};\n"
            f"    }}catch(e:any){{\n"
            f"        throw new Error(`element ${{k}} is invalid because ${{e}}`);\n"
            f"    }}}};\n"
            f"    return x as {tt};\n"
            f"}}catch(e:any){{throw new Error(`${{x}} is not a {tt} because ${{e}}`);}}}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    pass

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
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        """get dict[str,JsonType] object key json schema"""
        try:
            raise Exception("dict is not allowed as json object key type")
        except Exception:
            raise in_function_context(AnyDictCodecImpl.get_object_key_json_schema,vars()) from None
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"{{ [key: {self.key_codec.typescript_as_object_key_type(back_refs)}]: {self.value_codec.typescript_type(back_refs)} }}")
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(((x:any):x is {self.typescript_type(back_refs)}=>(\n"
            f"    (x !== null &&\n"
            f"    typeof (x) === 'object' &&\n"
            f"    !Array.isArray(x) &&\n"
            f"    (():boolean=>{{for (const k in x){{\n"
            f"        if (!x.hasOwnProperty(k)) continue;\n"
            f"        if (!({indent(12,self.key_codec.get_typescript_isa(TypeScriptSourceCode('k'),namespace,back_refs))} )||\n"
            f"            !({indent(12,self.value_codec.get_typescript_isa(TypeScriptSourceCode('x[k]'),namespace,back_refs))})) return false;}}\n"
            f"        return true;\n"
            f"    }})())))({expression}))")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
        
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((x: any): {tt} => {{try{{\n"
            f"    if (x === null) throw new Error(`${{x}} is not an object it is null`);\n"
            f"    if (typeof x !== 'object') throw new Error(`${{x}} is not an object it is a ${{typeof x}}`);\n"
            f"    if (Array.isArray(x)) throw new Error(`${{x}} is not an object it is an array`);\n"
            f"    for(const k in x){{\ntry{{\n"+
            f"        if (!x.hasOwnProperty(k)) continue;\n"+
            f"        const v=x[k];\n"
            f"        {indent(8,self.key_codec.get_typescript_asa(TypeScriptSourceCode('k'),namespace,back_refs))};\n"
            f"        {indent(8,self.value_codec.get_typescript_asa(TypeScriptSourceCode('v'),namespace,back_refs))};\n"
            f"    }}catch(e:any){{\n"
            f"        throw new Error(`element ${{k}} is invalid because ${{e}}`);\n"
            f"    }}}};\n"
            f"    return x as {tt};\n"
            f"}}catch(e:any){{throw new Error(`${{x}} is not a {tt} because ${{e}}`);}}}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    pass

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
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        """get dict[str,JsonType] object key json schema"""
        try:
            raise Exception("any json type is not allowed as json object key type")
        except Exception:
            raise in_function_context(AnyJsonCodecImpl.get_object_key_json_schema,vars()) from None
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->TypeScriptSourceCode:
        return TypeScriptSourceCode('any')
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            "true")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
        
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    pass

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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(self.get_type_fqn())
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        x=self.get_type_fqn().replace('/*','**').replace('*/','**')
        return TypeScriptSourceCode(f"string /* {x} */")
    def ensure_typescript_defs(self, namespace) -> None:
        typescript_fqn=[TypeScriptUQN(_) for _ in self.get_type_fqn().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        typescript_type_name=typescript_fqn[-1]
        if typescript_type_name not in target_namespace.defs:
            target_namespace.defs[typescript_type_name]=TypeScriptSourceCode(
                f"type {typescript_type_name} = number;")
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"function asInstanceOf{typescript_type_name}(v: any): {typescript_type_name}\n"
                f"{{\n"
                f"    try{{\n"
                f"        if (typeof v !== 'number') throw new Error(`${{v}} is a ${{typeof v}}`);\n"
                f"        return v;\n"
                f"    }}\n"
                f"    catch(e:any){{\n"
                f"        throw new Error(`${{v}} is not a {typescript_type_name} because ${{e}}`);\n"
                f"    }}\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f'isInstanceOf{typescript_type_name}')]=TypeScriptSourceCode(
                f"function isInstanceOf{typescript_type_name}(v:any): v is number\n"
                f"{{\n"
                f"    return typeof v === 'number';\n"
                f"}}")
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(typeof ({expression}) == 'number' /* {self.get_type_fqn()} */)")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(typeof ({expression}) == 'string' /* {self.get_type_fqn()} */ && !isNaN(parseInt({expression})))")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (typeof v !== 'number' /* {self.get_type_fqn()} */) throw new Error(`${{v}} is not a {self.get_type_fqn()} i.e. a number, it is a ${{typeof v}}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_as_object_key_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (typeof v !== 'string' /* {self.get_type_fqn()} */) throw new Error(`key ${{v}} is not a {self.get_type_fqn()} key type i.e. a string, it is a ${{typeof v}}`);\n"
            f"    if (isNaN(parseInt(v))) throw new Error(`key ${{v}} is not a stringified integer`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    pass

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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(self.get_type_fqn())
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        x=f"string /* {self.get_type_fqn().replace('/*','**').replace('*/','**')} */"
        return TypeScriptSourceCode(x)
    def ensure_typescript_defs(self, namespace) -> None:
        typescript_fqn=[TypeScriptUQN(_) for _ in self.get_type_fqn().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        typescript_type_name=typescript_fqn[-1]
        if typescript_type_name not in target_namespace.defs:
            target_namespace.defs[typescript_type_name]=TypeScriptSourceCode(
                f"type {typescript_type_name} = number;")
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"function asInstanceOf{typescript_type_name}(v: any): {typescript_type_name}\n"
                f"{{\n"
                f"    try{{\n"
                f"        if (typeof v !== 'number') throw new Error(`${{v}} is a ${{typeof v}}`);\n"
                f"        return v;\n"
                f"    }}\n"
                f"    catch(e:any){{\n"
                f"        throw new Error(`${{v}} is not a {typescript_type_name} because ${{e}}`);\n"
                f"    }}\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f'isInstanceOf{typescript_type_name}')]=TypeScriptSourceCode(
                f"function isInstanceOf{typescript_type_name}(v:any): v is number\n"
                f"{{\n"
                f"    return typeof v === 'number';\n"
                f"}}")
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(typeof ({expression}) == 'number' /* {self.get_type_fqn()} */)")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(typeof ({expression}) == 'string' /* {self.get_type_fqn()} */ && !isNaN(parseFloat({expression})))")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (typeof v !== 'number' /* {self.get_type_fqn()} */) throw new Error(`${{v}} is not a {self.get_type_fqn()} i.e. a number, it is a ${{typeof v}}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_as_object_key_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (typeof v !== 'string' /* {self.get_type_fqn()} */) throw new Error(`key ${{v}} is not a {self.get_type_fqn()} i.e. a string, it is a ${{typeof v}}`);\n"
            f"    if (isNaN(parseFloat(v))) throw new Error(`key ${{v}} is not a stringified number`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    pass

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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(self.get_type_fqn())
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        x=f"string /* {self.get_type_fqn().replace('/*','**').replace('*/','**')} */"
        return TypeScriptSourceCode(x)
    def ensure_typescript_defs(self, namespace) -> None:
        typescript_fqn=[TypeScriptUQN(_) for _ in self.get_type_fqn().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        typescript_type_name=typescript_fqn[-1]
        if typescript_type_name not in target_namespace.defs:
            target_namespace.defs[typescript_type_name]=TypeScriptSourceCode(
                f"type {typescript_type_name} = string;")
            as_a=self.get_typescript_asa(TypeScriptSourceCode("v"),
                                         TypeScriptNamespace({}),
                                         None)
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"function asInstanceOf{typescript_type_name}(v: any): {typescript_type_name}\n"
                f"{{\n"
                f"    return {as_a};\n"
                f"}}")
            is_a=self.get_typescript_isa(TypeScriptSourceCode("v"),
                                         TypeScriptNamespace({}),
                                         None)
            target_namespace.defs[TypeScriptUQN(f'isInstanceOf{typescript_type_name}')]=TypeScriptSourceCode(
                f"function isInstanceOf{typescript_type_name}(v:any): v is string /* {self.get_type_fqn().replace('/*','**').replace('*/','**')} */\n"
                f"{{\n"
                f"    return {is_a};\n"
                f"}}")
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        if self.t.pattern is not None:
            return TypeScriptSourceCode(
                f"(typeof ({expression}) == 'string' /* {self.get_type_fqn()} */ &&\n"
                f"(new RegExp({self.t.pattern.pattern!r})).exec({expression})!==null)")
        return TypeScriptSourceCode(
            f"(typeof ({expression}) == 'string') /* {self.get_type_fqn()} */")
    def get_typescript_isa_key(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return self.get_typescript_isa(expression,namespace,back_refs)
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        if self.t.pattern is not None:
            return TypeScriptSourceCode(
                f"((v: any): {tt} => {{\n"
                f"    if (typeof v !== 'string' /* {self.get_type_fqn()} */) throw new Error(`${{v}} is not a {self.get_type_fqn()} i.e. a string, it is a ${{typeof v}}`);\n"
                f"    if ((new RegExp({self.t.pattern.pattern!r})).exec({expression})===null) throw new Error(`${{v}} does not match pattern `+{self.t.pattern.pattern!r});\n"
                f"    return v as {tt};\n"
                f"}})({expression})")
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (typeof v !== 'string' /* {self.get_type_fqn()} */) throw new Error(`${{v}} is not a {self.get_type_fqn()} i.e. a string, it is a ${{typeof v}}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return self.get_typescript_asa(expression,namespace,back_refs)
    pass

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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(self.get_type_fqn())
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode('string /* xju.time.Timestamp */')
    def ensure_typescript_defs(self, namespace) -> None:
        typescript_fqn=[TypeScriptUQN(_) for _ in self.get_type_fqn().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        typescript_type_name=typescript_fqn[-1]
        if typescript_type_name not in target_namespace.defs:
            target_namespace.defs[typescript_type_name]=TypeScriptSourceCode(
                f"type {typescript_type_name} = number;")
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"function asInstanceOf{typescript_type_name}(v: any): {typescript_type_name}\n"
                f"{{\n"
                f"    try{{\n"
                f"        if (typeof v !== 'number') throw new Error(`${{v}} is a ${{typeof v}}`);\n"
                f"        return v;\n"
                f"    }}\n"
                f"    catch(e:any){{\n"
                f"        throw new Error(`${{v}} is not a {typescript_type_name} because ${{e}}`);\n"
                f"    }}\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f'isInstanceOf{typescript_type_name}')]=TypeScriptSourceCode(
                f"function isInstanceOf{typescript_type_name}(v:any): v is number\n"
                f"{{\n"
                f"    return typeof v === 'number';\n"
                f"}}")
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(typeof ({expression}) == 'number' /* {self.get_type_fqn()} */)")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(typeof ({expression}) == 'string' /* {self.get_type_fqn()} */ && !isNaN(parseFloat({expression})))")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (typeof v !== 'number' /* {self.get_type_fqn()} */) throw new Error(`${{v}} is not a {self.get_type_fqn()} i.e. a number, it is a ${{typeof v}}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_as_object_key_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (typeof v !== 'string' /* {self.get_type_fqn()} */) throw new Error(`key ${{v}} is not a {self.get_type_fqn()} i.e. a string, it is a ${{typeof v}}`);\n"
            f"    if (isNaN(parseFloat(v))) throw new Error(`xju.time.Timesamp key ${{v}} is not a stringified number`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    pass

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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f'"{self.value}"')
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f"{self.value}")
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v:any): v is {tt}=>(v==={tt}))({expression})")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return self.get_typescript_isa(expression,namespace,back_refs)
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (v !== {tt}) throw new Error(`(${{typeof v}}) ${{v}} is not the string {self.value}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return self.get_typescript_asa(expression,namespace,back_refs)
    pass

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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f'{self.value}')
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f"'{self.value}'")
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v:any): v is {tt}=>(v==={tt}))({expression})")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_as_object_key_type(back_refs)
        return TypeScriptSourceCode(
            f"((v:any): v is {tt}=>(parseInt(v)==={self.value}))({expression})")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (v !== {tt}) throw new Error(`(${{typeof v}}) ${{v}} is not the number {self.value}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_as_object_key_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (parseInt(v) !== parseInt({tt})) throw new Error(`(${{typeof v}}) ${{v}} is not the stringified number {self.value}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    pass

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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode('true' if self.value else 'false')
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode("'true'") if self.value else TypeScriptSourceCode("'false'")
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v:any): v is {tt}=>(v==={tt}))({expression})")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_as_object_key_type(back_refs)
        return TypeScriptSourceCode(
            f"((v:any): v is {tt}=>(v==={tt}))({expression})")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (v !== {tt}) throw new Error(`(${{typeof v}}) ${{v}} is not {self.typescript_type(None)}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_as_object_key_type(back_refs)
        typescript_value=tt
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (v !== `${{{typescript_value}}}`) throw new Error(`(${{typeof v}}) ${{v}} is not {typescript_value}`);\n"
            f"    return v as {typescript_value};\n"
            f"}})({expression})")
    pass

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

    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        raise Exception(f"bytes is not allowed as json object key type")
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->TypeScriptSourceCode:
        return TypeScriptSourceCode(f"Array<number> /* bytes */")
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return self.value_codec.get_typescript_isa(expression,namespace,back_refs)
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
        
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return self.value_codec.get_typescript_asa(expression,namespace,back_refs)
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    pass

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
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    @staticmethod
    def xju_json_codec_get_typescript_isa(
            expression:TypeScriptSourceCode,
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''return typescript source code that turns {expression} into a bool indicating whether the expression is a T
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    @staticmethod
    def xju_json_codec_get_typescript_asa(
            expression:TypeScriptSourceCode,
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''return typescript source code that safely casts {expression} to a T, throwing an Error if {expression} is not valid as a T
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
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
    def xju_json_codec_get_typescript_isa_key(
            expression:TypeScriptSourceCode,
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''return typescript source code that turns {expression} into a bool indicating whether the (string) expression is a T-as-object-key
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    @staticmethod
    def xju_json_codec_get_typescript_asa_key(
            expression:TypeScriptSourceCode,
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''return typescript source code that safely casts {expression} to a T-as-object-key i.e. a string, throwing an Error if {expression} is not valid as a T-as-object-key
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
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
    def xju_json_codec_get_typescript_isa_key(
            expression:TypeScriptSourceCode,
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''return typescript source code that turns {expression} into a bool indicating whether the (string) expression is a T-as-object-key
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    @staticmethod
    def xju_json_codec_get_typescript_asa_key(
            expression:TypeScriptSourceCode,
            namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''return typescript source code that safely casts {expression} to a T-as-object-key i.e. a string, throwing an Error if {expression} is not valid as a T-as-object-key
           - may add any supporting definitions to namespace, e.g. type for T itself'''
        assert False  #pragma NO COVER
        return TypeScriptSourceCode('')
    pass

class PythonAttrNameTag: pass
class PythonAttrName(xju.newtype.Str[PythonAttrNameTag]): pass
class JsonAttrNameTag: pass
class JsonAttrName(xju.newtype.Str[JsonAttrNameTag]): pass

attr_name_map = dict[type, dict[PythonAttrName, JsonAttrName]]()
dont_encode_attr_map = dict[type, set[PythonAttrName]]()

@dataclass
class AttrCodec:
    encoded_name: JsonAttrName
    codec: Any  # codec
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

class ClassCodecImpl:
    t:type
    attr_codecs:dict[str,AttrCodec]  # codec
    custom_codec:CustomClassCodec|None = None
    def __init__(self, t:type, attr_codecs:dict[str,Any]):
        self.t=t
        self.attr_codecs=attr_codecs
        if issubclass(t,CustomClassCodec):
            self.custom_codec=t
        pass
    def encode(self,x) -> JsonType:
        'encode {x} as a {self.t}'
        try:
            if not isinstance(x, self.t):
                xt=type(x)
                raise Exception(f'{x!r} (of type {xt}) is not a {self.t}')
            if self.custom_codec is not None:
                return self.custom_codec.xju_json_codec_encode(x)
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
            if self.custom_codec is not None:
                result=self.custom_codec.xju_json_codec_decode(x)
                assert isinstance(result,self.t), (repr(result),self.t)
                return result
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
        if self.custom_codec is not None:
            return self.custom_codec.xju_json_codec_get_json_schema(definitions)
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
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        if self.custom_codec is not None and isinstance(self.custom_codec, (
                CustomStringKeyClassCodec, CustomNonStringKeyClassCodec)):
            return self.custom_codec.xju_json_codec_get_object_key_json_schema(definitions)
        raise Exception(f"{self.get_type_fqn()} is not allowed as json object key type")
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        if self.custom_codec is not None:
            return self.custom_codec.xju_json_codec_get_typescript_type()
        return TypeScriptSourceCode(self.get_type_fqn())
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None=None) -> TypeScriptSourceCode:
        if self.custom_codec is not None and isinstance(self.custom_codec, (
                CustomNonStringKeyClassCodec, CustomStringKeyClassCodec)):
            return TypeScriptSourceCode(
                f"string /* {self.custom_codec.xju_json_codec_get_typescript_type().replace('/*','**').replace('*/','**')} */")
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    def ensure_typescript_defs(self,namespace)->None:
        '''ensure {namespace} has a typescript definition for {self.get_type_fqn()}'''
        typescript_fqn=[TypeScriptUQN(_) for _ in self.get_type_fqn().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        typescript_type_name=typescript_fqn[-1]
        tt=self.typescript_type(None)
        def type_back_ref()->TypeScriptSourceCode:
            return self.typescript_type(None)
        def isa_back_ref(expression:TypeScriptSourceCode)->TypeScriptSourceCode:
            return self.get_typescript_isa(expression,namespace,None)
        def asa_back_ref(expression:TypeScriptSourceCode)->TypeScriptSourceCode:
            return self.get_typescript_asa(expression,namespace,None)
        back_refs=TypeScriptBackRefs(type_back_ref,self.typescript_as_object_key_type,isa_back_ref,asa_back_ref)
        for attr_codec in self.attr_codecs.values():
            attr_codec.codec.ensure_typescript_defs(namespace)
        if typescript_type_name not in target_namespace.defs:
            target_namespace.defs[typescript_type_name]=TypeScriptSourceCode(
                f"type {typescript_type_name} = {{\n"+
                ''.join([f"    {attr_codec.encoded_name.value()}: {indent(4,attr_codec.codec.typescript_type(back_refs))};\n"
                         for attr_name, attr_codec in self.attr_codecs.items()])+
                f"}};")
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"function asInstanceOf{typescript_type_name}(v: any): {tt}\n"
                f"{{\n"
                f"    try{{\n"
                f"        if (Array.isArray(v)) throw new Error(`${{v}} is an array`);\n"
                f"        if (v == null) throw new Error(`${{v}} is not an object it is null`);\n"
                f"        if (typeof v !== 'object') throw new Error(`${{v}} is not an object it is a ${{typeof v}}`);\n"
                f"        const attr_asa=function(name:string, asa:any):any{{\n"
                f"            try{{\n"
                f"                asa(v[name]);\n"
                f"            }}\n"
                f"            catch(e:any){{\n"
                f"                throw new Error(`attribute ${{name}} is invalid because ${{e}}`);\n"
                f"            }}\n"
                f"        }}\n" +
                ''.join([f"        attr_asa('{attr_codec.encoded_name.value()}',(x:any)=>{indent(8,attr_codec.codec.get_typescript_asa('x',namespace,back_refs))});\n"
                         for attr_name, attr_codec in self.attr_codecs.items()])+
                f"        return v as {tt};\n"
                f"    }}\n"
                f"    catch(e:any){{\n"
                f"        throw new Error(`${{v}} is not a {tt} because ${{e}}`);\n"
                f"    }}\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f"isInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"function isInstanceOf{typescript_type_name}(v:any): v is {tt}\n"
                f"{{\n"
                f"    return (\n"
                f"        !Array.isArray(v) &&\n"
                f"        v !== null &&"+
                f"        typeof v === 'object'"+
                ''.join([" &&\n        "+indent(8,attr_codec.codec.get_typescript_isa(f'v["{attr_codec.encoded_name.value()}"]',namespace,back_refs))
                         for attr_name, attr_codec in self.attr_codecs.items()])+")"
                f"}}")
            pass
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        if self.custom_codec is not None:
            return self.custom_codec.xju_json_codec_get_typescript_isa(expression,namespace)
        self.ensure_typescript_defs(namespace)
        fqn=self.get_type_fqn().split('.')
        return TypeScriptSourceCode('.'.join(fqn[0:-1]+[f"isInstanceOf{fqn[-1]}({expression})"]))

    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        if self.custom_codec is not None and isinstance(
                self.custom_codec,(CustomStringKeyClassCodec, CustomNonStringKeyClassCodec)):
            return self.custom_codec.xju_json_codec_get_typescript_isa_key(expression,namespace)
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
        
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        if self.custom_codec is not None:
            return self.custom_codec.xju_json_codec_get_typescript_asa(expression,namespace)
        self.ensure_typescript_defs(namespace)
        fqn=self.get_type_fqn().split('.')
        fqn=self.get_type_fqn().split('.')
        return TypeScriptSourceCode('.'.join(fqn[0:-1]+[f"asInstanceOf{fqn[-1]}({expression})"]))
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        if self.custom_codec is not None and isinstance(
                self.custom_codec,(CustomStringKeyClassCodec, CustomNonStringKeyClassCodec)):
            return self.custom_codec.xju_json_codec_get_typescript_asa_key(expression,namespace)
        raise Exception(f"{self.typescript_type(back_refs)} is not allowed as a typescript object key type")
    pass

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
        fqn=f"self.get_type_fqn().{self.name}"
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
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        return get_type_fqn(self.t)
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f'{self.get_type_fqn()}.{self.value.name}')
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f'{self.get_type_fqn()}.{self.value.name}')
    def typescript_value(self) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(json.dumps(self.value.value))
    def ensure_typescript_defs(self,namespace)->None:
        # EnumCodecImpl supplies defs
        pass
    def get_typescript_isa(
            self,
            expression:TypeScriptSourceCode,
            namespace: TypeScriptNamespace,
            back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f'({expression} === {self.typescript_type(back_refs)})')
    def get_typescript_isa_key(
            self,
            expression:TypeScriptSourceCode,
            namespace: TypeScriptNamespace,
            back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f'({expression} === `${{{self.typescript_type(back_refs)}}}`)')
    def get_typescript_asa(
            self,
            expression:TypeScriptSourceCode,
            namespace: TypeScriptNamespace,
            back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (v !== {tt}) throw new Error(`the ${{typeof v}} ${{v}} is not {tt}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    def get_typescript_asa_key(
            self,
            expression:TypeScriptSourceCode,
            namespace: TypeScriptNamespace,
            back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_as_object_key_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (v !== {tt}) throw new Error(`the ${{typeof v}} ${{v}} is not {tt}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    pass
    
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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->TypeScriptSourceCode:
        return TypeScriptSourceCode(self.get_type_fqn())
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(f"string /* {self.typescript_type(back_refs).replace('/*','**').replace('*/','**')} */")
    def ensure_typescript_defs(self, namespace) -> None:
        typescript_fqn=[TypeScriptUQN(_) for _ in self.get_type_fqn().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        typescript_type_name=typescript_fqn[-1]
        if typescript_type_name not in target_namespace.defs:
            enum_value_codec:EnumValueCodecImpl
            target_namespace.defs[typescript_type_name]=TypeScriptSourceCode(
                f"enum {typescript_type_name} {{\n"+
                ',\n'.join([f'    {name} = {enum_value_codec.typescript_value().value()}'
                         for name, enum_value_codec in self.value_codecs.items()])+
                f'\n}};\n')
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"function asInstanceOf{typescript_type_name}(v: any): {typescript_type_name}\n"
                f"{{\n"
                f"    return {indent(4,self.get_typescript_asa(TypeScriptSourceCode('v'),namespace,None))};\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f"isInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"function isInstanceOf{typescript_type_name}(v:any): v is {typescript_type_name}\n"
                f"{{\n"
                f"    return {indent(4,self.get_typescript_isa(TypeScriptSourceCode('v'),namespace,None))};\n"
                f"}}")
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        enum_value_codec:EnumValueCodecImpl
        isas=[' ||\n       '+indent(12,enum_value_codec.get_typescript_isa(TypeScriptSourceCode(f"v"),
                                                                           namespace,back_refs))
              for _t,enum_value_codec in self.value_codecs.items()]
        return TypeScriptSourceCode(
            f"((v:any): v is {self.typescript_type(back_refs)}=>{{\n"
            f"    return false" +
            f"".join(isas) + ";\n"+
            f"}})({expression})")
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        enum_value_codec:EnumValueCodecImpl
        isas=[' ||\n       '+indent(12,enum_value_codec.get_typescript_isa_key(
            TypeScriptSourceCode(f"v"),
            namespace,back_refs))
              for _t,enum_value_codec in self.value_codecs.items()]
        return TypeScriptSourceCode(
            f"((v:any): v is {self.typescript_as_object_key_type(back_refs)}=>{{\n"
            f"    return false" +
            f"".join(isas) + ";\n"+
            f"}})({expression})")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        enum_value_codec:EnumValueCodecImpl
        tt=self.typescript_type(back_refs)
        asas=[f"    try{{\n"+
              f"        {indent(8,enum_value_codec.get_typescript_asa(TypeScriptSourceCode('v'),namespace,back_refs))};\n"+
              f"        return v as {tt};\n"+
              f"    }}\n"+
              f"    catch(e:any){{\n"+
              f"        es.push(e.message);\n"+
              f"    }};\n"
              for _t,enum_value_codec in self.value_codecs.items()]
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{try{{\n" +
            f"    var es = new Array<string>();\n"+
            "".join(asas)+
            f"    throw new Error(es.join(' and '));\n"+
            f"}}catch(e:any)\n"+
            f"{{\n"+
            f"    throw new Error(`${{v}} is not a {tt} because ${{e}}`);\n"+
            f"}}}})({expression})")
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        enum_value_codec:EnumValueCodecImpl
        tt=self.typescript_as_object_key_type(back_refs)
        asas=[f"    try{{\n"+
              f"        {indent(8,enum_value_codec.get_typescript_asa_key(TypeScriptSourceCode('v'),namespace,back_refs))};\n"+
              f"        return v as {tt};\n"+
              f"    }}\n"+
              f"    catch(e:any){{\n"+
              f"        es.push(e.message);\n"+
              f"    }};\n"
              for _t,enum_value_codec in self.value_codecs.items()]
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{try{{\n" +
            f"    var es = new Array<string>();\n"+
            "".join(asas)+
            f"    throw new Error(es.join(' and '));\n"+
            f"}}catch(e:any)\n"+
            f"{{\n"+
            f"    throw new Error(`${{v}} is not a {tt} key because ${{e}}`);\n"+
            f"}}}})({expression})")
    pass

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
    def get_object_key_json_schema(self, definitions:dict[str,dict]) -> dict:
        assert self.codec is not None
        fqn=self.codec.get_type_fqn()
        return  {
            '$ref': f'#/definitions/{fqn}'
        }
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        assert self.codec is not None
        return self.codec.typescript_type(back_refs)
    def typescript_as_object_key_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        assert self.codec is not None
        return self.codec.typescript_as_object_key_type(back_refs)
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        assert self.codec is not None
        return self.codec.get_typescript_isa(expression,namespace,back_refs)
    def get_typescript_isa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        assert self.codec is not None
        return self.codec.get_typescript_isa_key(expression,namespace,back_refs)
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        assert self.codec is not None
        return self.codec.get_typescript_asa(expression,namespace,back_refs)
    def get_typescript_asa_key(self,
                               expression:TypeScriptSourceCode,
                               namespace: TypeScriptNamespace,
                               back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        assert self.codec is not None
        return self.codec.get_typescript_asa_key(expression,namespace,back_refs)

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
            return TupleCodec([_explodeSchema(_,type_var_map,codec_backrefs) for _ in get_args(t)])
        if t is dict:
            return AnyDictCodecImpl()
        if type(t) is GenericAlias and get_origin(t) is dict:
            return DictCodecImpl(*[_explodeSchema(_, type_var_map,codec_backrefs) for _ in get_args(t)])
        if type(t) is UnionType:
            return UnionCodecImpl(get_args(t),
                                  {at:_explodeSchema(at, type_var_map,codec_backrefs) for at in get_args(t)})
        if type(t) is _UnionGenericAlias:
            return UnionCodecImpl(get_args(t),
                                  {at:_explodeSchema(at, type_var_map,codec_backrefs) for at in get_args(t)})
        if t is Self:
            return codec_backrefs[Self]
        if type(t) is _GenericAlias:
            if type(get_origin(t)) is _SpecialForm and str(get_origin(t))=="typing.Final":
                return _explodeSchema(get_args(t)[0],type_var_map,codec_backrefs)
            local_type_var_map={
                type_var: value
                for type_var, value in
                list((type_var_map or {}).items())+list(zip(get_origin(t).__parameters__, get_args(t)))
            }
            dont_encode=dont_encode_attr_map.get(t)
            old_self=codec_backrefs.get(Self,None)
            self_ref=BackRefCodecImpl(None)
            codec_backrefs[Self]=self_ref
            try:
                result=ClassCodecImpl(
                    get_origin(t),
                    { n: AttrCodec(get_attr_encoded_name(getmro(get_origin(t)), PythonAttrName(n)),
                                   _explodeSchema(nt,local_type_var_map,codec_backrefs))
                      for n,nt in get_type_hints(get_origin(t)).items()
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
        if type(t) is EnumType and issubclass(t, Enum):
            return EnumCodecImpl(
                t,{name: EnumValueCodecImpl(name,vv.__class__,vv,_explodeSchema(type(vv.value),type_var_map,codec_backrefs))
                   for name,vv in t.__members__.items()})
        if isinstance(t,Enum):
            return EnumValueCodecImpl(t.name,t.__class__,t,_explodeSchema(type(t.value),type_var_map,codec_backrefs))
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
            result=ClassCodecImpl(
            t,{n: AttrCodec(get_attr_encoded_name(getmro(t), PythonAttrName(n)),
                            _explodeSchema(nt,type_var_map,codec_backrefs))
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
