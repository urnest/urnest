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
from dataclasses import dataclass
from xju.xn import Xn,in_context,in_function_context,readable_repr
from typing import TypeVar, Generic, Type, cast, Any, Protocol, Self, Callable, get_type_hints
from typing import Sequence, Literal, NewType
from typing import _LiteralGenericAlias  # type: ignore  # mypy 1.1.1
from typing import _UnionGenericAlias  # type: ignore  # mypy 1.1.1
from typing import _GenericAlias  # type: ignore  # mypy 1.2.0
from typing import get_origin,get_args, runtime_checkable
from types import GenericAlias, UnionType, NoneType
import xju.newtype

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
    isa_back_ref:Callable[[TypeScriptSourceCode],TypeScriptSourceCode]
    asa_back_ref:Callable[[TypeScriptSourceCode],TypeScriptSourceCode]
    
def codec(t: Type[T]) -> 'Codec[T]':
    '''build codec to encode/decode a "t" to/from json'''
    return Codec[T](t)

class Codec(Generic[T]):
    t:Type[T]
    def __init__(self, t: Type[T]):
        'initialse json decoder for type %(t)r'
        self.t=t
        self.codec:CodecProto=_explodeSchema(self.t, {})
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
    def typescript_type(self) -> TypeScriptUQN:
        '''return typescript unqualified equivalent type for T'''
        return self.codec.typescript_type(None)
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
        '''get typescript "{expression} is a T" code, adding any necessary definitions to {namespace}
           result is a type-guard expression, examples of some possible results:
             parameter       possible code                  note
             "x.y"           "(typeof (x.y) == 'number')"   (uses built-in type guard pattern)
             "z"             "m.isInstanceOfFred(z)"        (calls a type-guard function)'''
        return self.codec.get_typescript_isa(expression,namespace,None)
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace) -> TypeScriptSourceCode:
        '''get typescript "{expression} as a T" code, adding any necessary definitions to {namespace}
           result is a cast expression that throws an Error if {expression} is not a T
        '''
        return self.codec.get_typescript_asa(expression,namespace,None)
    pass

class CodecProto(Protocol):
    def encode(self,x:Any,back_ref:None|Callable[[Any],JsonType])->JsonType:
        pass
    def decode(self,x:JsonType,back_ref:None|Callable[[JsonType],Any])->Any:
        pass
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        pass
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> TypeScriptUQN:
        pass
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        pass
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        pass
    pass

Atom=TypeVar('Atom',int,str,bool,float)

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
        return { 'type': st }
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> str:
        '''return typescript equivalent type for T'''
        if self.t is int: return 'number'
        if self.t is str: return 'string'
        if self.t is float: return 'number'
        assert self.t is bool, self.t
        return 'boolean'
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(typeof ({expression}) == '{self.typescript_type(back_refs)}')")
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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> str:
        '''return typescript equivalent type for None'''
        return 'null'
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(({expression}) === {self.typescript_type(back_refs)})")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"((v: any): null => {{\n"
            f"    if (v !== null) throw new Error(`${{v}} is not null it is a ${{typeof v}}`);\n"
            f"    return v as null;\n"
            f"}})({expression})")
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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->str:
        return f"Array<{self.value_codec.typescript_type(back_refs)}>"
    def ensure_typescript_defs(self, namespace) -> None:
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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->str:
        return f"Array<any>"
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(Array.isArray({expression}))")
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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->str:
        value_types=','.join([_.typescript_type(back_refs) for _ in self.value_codecs])
        return f"[{value_types}]"
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        isas=[' &&\n            '+indent(12,self.value_codecs[i].get_typescript_isa(f"v[{i}]",namespace,back_refs))
              for i in range(0, self.number_of_codecs)]
        return TypeScriptSourceCode(
            f"((v:any): v is {self.typescript_type(back_refs)}=>{{\n"
            f"    if (Array.isArray(v)) {{\n" +
            f"        return v.length == {self.number_of_codecs}" +
            f"".join(isas) + ";\n"+
            f"    }}\n"+
            f"    else return false;\n"+
            f"}})({expression})")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        asas=['    '+indent(4,self.value_codecs[i].get_typescript_asa(f"v[{i}]",namespace,back_refs))+';\n'
              for i in range(0, self.number_of_codecs)]
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{try{{\n" +
            f"    if (!Array.isArray(v)) throw new Error(`${{v}} is not an array it is a ${{typeof v}}`);\n" +
            f"    if (v.length != {self.number_of_codecs}) throw new Error(`${{v}} does not have {self.number_of_codecs} elements (it has ${{v.length}} elements)`);\n" +
            ''.join(asas)+
            f"    return v as {tt};\n"+
            f"}}catch(e:any){{ throw new Error(`${{v}} is not a {tt} because ${{e}}`);}}}})({expression})")
    pass

class UnionCodec:
    def __init__(self,allowed_types, value_codecs):
        self.allowed_types=allowed_types
        self.value_codecs=value_codecs
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
        '''decode {x!r} as one of {self.allowed_types}'''
        try:
            exceptions=[]
            for t, c in self.value_codecs.items():
                try:
                    return c.decode(x,back_ref)
                except Exception as e:
                    exceptions.append( (t, e) )
                    pass
                pass
            raise Exception(' and '.join([f'failed to decode as {t!r} because {e}' for t,e in exceptions]))
        except Exception:
            raise in_function_context(UnionCodec.decode,vars()) from None
        pass
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            "oneOf": [ codec.get_json_schema(definitions, self_ref) for codec in self.value_codecs.values() ]
        }
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->str:
        return '|'.join([_.typescript_type(back_refs) for _ in self.value_codecs.values()])
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        isas=[' ||\n       '+indent(12,value_codec.get_typescript_isa(f"v",namespace,back_refs))
              for value_codec in self.value_codecs.values()]
        return TypeScriptSourceCode(
            f"((v:any): v is {self.typescript_type(back_refs)}=>{{\n"
            f"    return false" +
            f"".join(isas) + ";\n"+
            f"}})({expression})")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        asas=[f"    try{{\n"+
              f"        {indent(8,value_codec.get_typescript_asa('v',namespace,back_refs))};\n"+
              f"        return v as {tt};\n"+
              f"    }}\n"+
              f"    catch(e:any){{\n"+
              f"        es.push(e.message);\n"+
              f"    }};\n"
              for value_codec in self.value_codecs.values()]
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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->str:
        return f"{{ [key: {self.key_codec.typescript_type(back_refs)}]: {self.value_codec.typescript_type(back_refs)} }}"
    def ensure_typescript_defs(self, namespace) -> None:
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
            f"        if (!({indent(8,self.key_codec.get_typescript_isa(TypeScriptSourceCode('k'),namespace,back_refs))} )||\n"
            f"            !({indent(8,self.value_codec.get_typescript_isa(TypeScriptSourceCode('x[k]'),namespace,back_refs))})) return false;}}\n"
            f"    return true;\n"
            f"}})()))({expression}))")
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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->str:
        return f"{{ [key: {self.key_codec.typescript_type(back_refs)}]: {self.value_codec.typescript_type(back_refs)} }}"
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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None)->str:
        return 'any'
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            "true")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"({expression})")
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
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        if self.t.__module__=='__main__':
            return self.t.__name__
        return f"{self.t.__module__}.{self.t.__name__}"
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            'description': self.get_type_fqn(),
            'type': 'integer'
        }
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> str:
        return 'number'
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
            f"(typeof ({expression}) == '{self.typescript_type(back_refs)}')")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (typeof v !== '{tt}') throw new Error(`${{v}} is not a {self.get_type_fqn()} i.e. a {tt}, it is a ${{typeof v}}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
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
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        if self.t.__module__=='__main__':
            return self.t.__name__
        return f"{self.t.__module__}.{self.t.__name__}"
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            'description': self.get_type_fqn(),
            'type': 'number'
        }
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> str:
        return 'number'
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
            f"(typeof ({expression}) == '{self.typescript_type(back_refs)}')")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (typeof v !== '{tt}') throw new Error(`${{v}} is not a {self.get_type_fqn()} i.e. a {tt}, it is a ${{typeof v}}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
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
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        if self.t.__module__=='__main__':
            return self.t.__name__
        return f"{self.t.__module__}.{self.t.__name__}"
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            'description': self.get_type_fqn(),
            'type': 'string'
        }
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> str:
        return 'string'
    def ensure_typescript_defs(self, namespace) -> None:
        typescript_fqn=[TypeScriptUQN(_) for _ in self.get_type_fqn().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        typescript_type_name=typescript_fqn[-1]
        if typescript_type_name not in target_namespace.defs:
            target_namespace.defs[typescript_type_name]=TypeScriptSourceCode(
                f"type {typescript_type_name} = string;")
            target_namespace.defs[TypeScriptUQN(f"asInstanceOf{typescript_type_name}")]=TypeScriptSourceCode(
                f"function asInstanceOf{typescript_type_name}(v: any): {typescript_type_name}\n"
                f"{{\n"
                f"    try{{\n"
                f"        if (typeof v !== 'string') throw new Error(`${{v}} is a ${{typeof v}}`);\n"
                f"        return v;\n"
                f"    }}\n"
                f"    catch(e:any){{\n"
                f"        throw new Error(`${{v}} is not a {typescript_type_name} because ${{e}}`);\n"
                f"    }}\n"
                f"}}")
            target_namespace.defs[TypeScriptUQN(f'isInstanceOf{typescript_type_name}')]=TypeScriptSourceCode(
                f"function isInstanceOf{typescript_type_name}(v:any): v is string\n"
                f"{{\n"
                f"    return typeof v === 'string';\n"
                f"}}")
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        return TypeScriptSourceCode(
            f"(typeof ({expression}) == '{self.typescript_type(back_refs)}')")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (typeof v !== '{tt}') throw new Error(`${{v}} is not a {self.get_type_fqn()} i.e. a {tt}, it is a ${{typeof v}}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> str:
        return f'"{self.value}"'
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v:any): v is {tt}=>(v==={tt}))({expression})")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (v !== {tt}) throw new Error(`the ${{typeof v}} ${{v}} is not the string {self.value}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    pass

class LiteralIntCodec:
    value:int
    def __init__(self,value:int):
        self.value=value
    def encode(self, x:int, _:None|Callable[[Any],JsonType])->int:
        if type(x) is not int or x != self.value:
            raise Exception(f'{x!r} is not {self.value!r}')
        return x
    def decode(self, x:JsonType,_:None|Callable[[JsonType],Any])->int:
        if type(x) is not int:
            raise Exception(f'{x!r} is not a int')
        if x != self.value:
            raise Exception(f'{x!r} is not {self.value!r}')
        return x
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        return {
            'type': 'number',
            'enum': [ self.value ]
        }
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> str:
        return f'{self.value}'
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v:any): v is {tt}=>(v==={tt}))({expression})")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (v !== {tt}) throw new Error(`the ${{typeof v}} ${{v}} is not the number {self.value}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    pass

class LiteralBoolCodec:
    value:bool
    def __init__(self,value:bool):
        self.value=value
    def encode(self, x:bool, _:None|Callable[[Any],JsonType])->bool:
        if type(x) is not bool or x != self.value:
            raise Exception(f'{x!r} is not {self.value!r}')
        return x
    def decode(self, x:JsonType,_:None|Callable[[JsonType],Any])->bool:
        if type(x) is not bool:
            raise Exception(f'{x!r} is not a boolean')
        if x != self.value:
            raise Exception(f'{x!r} is not {self.value!r}')
        return x
    def get_json_schema(self, definitions:dict[bool,dict], self_ref:None|bool) -> dict:
        return {
            'type': 'boolean',
            'enum': [ self.value ]
        }
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> str:
        return 'true' if self.value else 'false'
    def ensure_typescript_defs(self, namespace) -> None:
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        return TypeScriptSourceCode(
            f"((v:any): v is {tt}=>(v==={tt}))({expression})")
    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        tt=self.typescript_type(back_refs)
        typescript_value='true' if self.value else 'false'
        return TypeScriptSourceCode(
            f"((v: any): {tt} => {{\n"
            f"    if (v !== {tt}) throw new Error(`the ${{typeof v}} ${{v}} is not {self.typescript_type(None)}`);\n"
            f"    return v as {tt};\n"
            f"}})({expression})")
    pass

@runtime_checkable
class CustomClassCodec(Protocol):
    '''implement these methods on class T to use custom encoding of instances of T'''
    @staticmethod
    def xju_json_codec_encode(
            x:object  # x is a T
    ) -> dict:  # JSON-encodable
        pass
    @staticmethod
    def xju_json_codec_decode(x:JsonType) -> object:  # must return a T
        pass
    pass

class ClassCodec:
    t:type
    attr_codecs:dict[str,Any]  # codec
    custom_encode:None|Callable[[object],JsonType]=None
    custom_decode:None|Callable[[JsonType],object]=None
    def __init__(self, t:type, attr_codecs:dict[str,Any]):
        self.t=t
        self.attr_codecs=attr_codecs
        if issubclass(t,CustomClassCodec):
            self.custom_encode=t.xju_json_codec_encode
            self.custom_decode=t.xju_json_codec_decode
        pass
    def encode(self,x,_:None|Callable[[Any],JsonType]) -> JsonType:
        'encode {x} as a {self.t}'
        try:
            if self.custom_encode is not None:
                assert isinstance(x,self.t), (repr(x),self.t)
                return self.custom_encode(x)
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
            if hasattr(self.t, '__encode__'):
                result = self.t.__encode__(result)
            return result
        except Exception:
            raise in_function_context(ClassCodec.encode,vars()) from None
        pass
    def decode(self,x,_:None|Callable[[JsonType],Any]) -> object:
        'deocde {x} as a {self.t}'
        try:
            if self.custom_decode is not None:
                result=self.custom_decode(x)
                assert isinstance(result,self.t), (repr(result),self.t)
                return result
            def back_ref(x:JsonType) -> object:
                return self.decode(x,None)
            if hasattr(self.t, '__decode__'):
                x = self.t.__decode__(x)
            attr_values={}
            for n, attr_codec in self.attr_codecs.items():
                if n in x:
                    try:
                        value=attr_codec.decode(x[n],back_ref)
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
            raise in_function_context(ClassCodec.decode,vars()) from None
        pass
    def get_type_fqn(self):
        '''get the fully qualified name of {self.t}'''
        if self.t.__module__ == "__main__":
            return self.t.__name__
        return f'{self.t.__module__}.{self.t.__name__}'
    def get_json_schema(self, definitions:dict[str,dict], self_ref:None|str) -> dict:
        fqn=self.get_type_fqn()
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
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> str:
        return self.get_type_fqn()
    def ensure_typescript_defs(self,namespace)->None:
        '''ensure {namespace} has a typescript definition for {self.get_type_fqn()}'''
        typescript_fqn=[TypeScriptUQN(_) for _ in self.get_type_fqn().split('.')]
        target_namespace=namespace.get_namespace_of(typescript_fqn)
        typescript_type_name=typescript_fqn[-1]
        tt=self.typescript_type(None)
        def type_back_ref()->TypeScriptSourceCode:
            return TypeScriptSourceCode(self.typescript_type(None))
        def isa_back_ref(expression:TypeScriptSourceCode)->TypeScriptSourceCode:
            return self.get_typescript_isa(expression,namespace,None)
        def asa_back_ref(expression:TypeScriptSourceCode)->TypeScriptSourceCode:
            return self.get_typescript_asa(expression,namespace,None)
        back_refs=TypeScriptBackRefs(type_back_ref,isa_back_ref,asa_back_ref)
        if typescript_type_name not in target_namespace.defs:
            target_namespace.defs[typescript_type_name]=TypeScriptSourceCode(
                f"type {typescript_type_name} = {{\n"+
                ''.join([f"    {attr_name}: {indent(4,TypeScriptSourceCode(attr_codec.typescript_type(back_refs)))};\n"
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
                ''.join([f"        attr_asa('{attr_name}',(x:any)=>{indent(8,attr_codec.get_typescript_asa('x',namespace,back_refs))});\n"
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
                ''.join([" &&\n        "+indent(8,attr_codec.get_typescript_isa(f'v["{attr_name}"]',namespace,back_refs))
                         for attr_name, attr_codec in self.attr_codecs.items()])+")"
                f"}}")
            pass
        pass
    def get_typescript_isa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        fqn=self.get_type_fqn().split('.')
        return TypeScriptSourceCode('.'.join(fqn[0:-1]+[f"isInstanceOf{fqn[-1]}({expression})"]))

    def get_typescript_asa(self,
                           expression:TypeScriptSourceCode,
                           namespace: TypeScriptNamespace,
                           back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        self.ensure_typescript_defs(namespace)
        fqn=self.get_type_fqn().split('.')
        fqn=self.get_type_fqn().split('.')
        return TypeScriptSourceCode('.'.join(fqn[0:-1]+[f"asInstanceOf{fqn[-1]}({expression})"]))
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
        assert self_ref is not None
        return {
            '$ref': self_ref
        }
    def typescript_type(self,back_refs:TypeScriptBackRefs|None) -> str:
        assert back_refs is not None
        return back_refs.type_back_ref().value()
    def ensure_typescript_defs(self,namespace)->None:
        # self would already have been done
        pass
    def get_typescript_isa(
            self,
            expression:TypeScriptSourceCode,
            namespace: TypeScriptNamespace,
            back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        assert back_refs is not None
        return back_refs.isa_back_ref(expression)
    def get_typescript_asa(
            self,
            expression:TypeScriptSourceCode,
            namespace: TypeScriptNamespace,
            back_refs:TypeScriptBackRefs|None) -> TypeScriptSourceCode:
        assert back_refs is not None
        return back_refs.asa_back_ref(expression)
    pass
    
def _explodeSchema(t:type|NewType|TypeVar|GenericAlias|UnionType|_LiteralGenericAlias|_GenericAlias,
                   type_var_map:dict[TypeVar,Any]|None):
    '''explode type {t!r} into a tree of codecs using map {type_var_map} to resolve any generic type refs i.e. TypeVars'''
    try:
        if type(t) is TypeVar:
            assert type_var_map is not None
            assert t in type_var_map, type_var_map.keys()
            return _explodeSchema(type_var_map[t],type_var_map)
        if t is float or (isinstance(t,NewType) and t.__supertype__ is float):
            return NoopCodec[float](float)
        if t is int or (isinstance(t,NewType) and t.__supertype__ is int):
            return NoopCodec[int](int)
        if t is str or (isinstance(t,NewType) and t.__supertype__ is str):
            return NoopCodec[str](str)
        if t is bool or (isinstance(t,NewType) and t.__supertype__ is bool):
            return NoopCodec[bool](bool)
        if t is None or t is NoneType:
            return NoneCodec()
        if t is list:
            return AnyListCodec()
        if type(t) is GenericAlias and get_origin(t) is list:
            return ListCodec(_explodeSchema(get_args(t)[0],type_var_map))
        if type(t) is _LiteralGenericAlias and len(t.__args__)==1:
            return _explode_literal(t.__args__[0])
        if type(t) is _LiteralGenericAlias:
            return UnionCodec(get_args(t),
                              {t:_explode_literal(t) for t in get_args(t)})
        if type(t) is GenericAlias and get_origin(t) is tuple:
            return TupleCodec([_explodeSchema(_,type_var_map) for _ in get_args(t)])
        if t is dict:
            return AnyDictCodec()
        if type(t) is GenericAlias and get_origin(t) is dict:
            return DictCodec(*[_explodeSchema(_, type_var_map) for _ in get_args(t)])
        if type(t) is UnionType:
            return UnionCodec(get_args(t),
                              {t:_explodeSchema(t, type_var_map) for t in get_args(t)})
        if type(t) is _UnionGenericAlias:
            return UnionCodec(get_args(t),
                              {t:_explodeSchema(t, type_var_map) for t in get_args(t)})
        if t is Self:
            return SelfCodec()
        if type(t) is _GenericAlias:
            local_type_var_map={
                type_var: value
                for type_var, value in
                list((type_var_map or {}).items())+list(zip(get_origin(t).__parameters__, get_args(t)))
            }
            return ClassCodec(
                get_origin(t),
                { n: _explodeSchema(nt,local_type_var_map)
                  for n,nt in get_type_hints(get_origin(t)).items()})
        assert isinstance(t,type), t
        if issubclass(t,xju.newtype.Int):
            return NewIntCodec(t)
        if issubclass(t,xju.newtype.Float):
            return NewFloatCodec(t)
        if issubclass(t,xju.newtype.Str):
            return NewStrCodec(t)
        return ClassCodec(
            t,{n: _explodeSchema(nt,type_var_map) for n,nt in get_type_hints(t).items()})
    except Exception:
        raise in_function_context(_explodeSchema,vars()) from None
    pass

def _explode_literal(value: Any):
    '''create codec for literal value {value!r}'''
    try:
        if type(value) is str:
            return LiteralStrCodec(value)
        if type(value) is bool:
            return LiteralBoolCodec(value)
        if type(value) is int:
            return LiteralIntCodec(value)
        t=type(value)
        raise Exception(f'{t} literals are not supported (only support str, int, bool)')
    except:
        raise in_function_context(_explode_literal, vars())
    pass

def indent(n: int, s:TypeScriptSourceCode)->str:
    lines=s.splitlines()
    return '\n'.join([lines[0].value()]+[(' '*n)+l.value() for l in lines[1:]])
