# Copyright (c) 2024 Trevor Taylor
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
from typing import Callable, Final, Self, TypeGuard, overload, Sequence, NewType, Literal
from mypy.plugin import (
    Plugin, FunctionSigContext, FunctionContext, Expression, MethodContext,
    CheckerPluginInterface
)
from mypy.nodes import (
    Expression, NameExpr, IndexExpr, TupleExpr, StrExpr, IntExpr, FloatExpr, OpExpr, MemberExpr,
    TypeVarExpr, CallExpr,
    TypeInfo,
    TypeAlias,
    Context,
    SymbolNode,
    Var,
    FuncDef,
    Decorator,
    OverloadedFuncDef,
)
from mypy.types import (
    LITERAL_TYPE_NAMES,
    AnyType,
    CallableType,
    FunctionLike,
    Instance,
    NoneType,
    TupleType,
    Type,
    TypeAliasType,
    TypeVarType,
    TypeVarId,
    TypeOfAny,
    TypeType,
    UninhabitedType,
    UnionType,
    Overloaded,
    LiteralType,
)
from mypy.typeops import tuple_fallback
from mypy.checker import TypeChecker
from mypy.checkexpr import ExpressionChecker, get_proper_type
from mypy.erasetype import erase_typevars
from mypy.subtypes import is_subtype

CODEC_FQN="xju.json_codec.codec"

def adjust_codec_signature(s: FunctionSigContext) -> FunctionLike:
    """adjust signature to Any -> Any so we can analyse param type after mypy resolves param type"""
    if len(s.args) != 1:
        l=len(s.args)
        s.api.fail(f"{CODEC_FQN}() takes exactly one parameter, not {l}", s.context)
        return s.default_signature
    s.default_signature.arg_types[0] = AnyType(TypeOfAny.explicit)
    return s.default_signature

class CodecParamInvalid(Exception):
    pass
    
def infer_codec_value_type_from_args(arg_exprs: list[Expression],
                                     checker_api:CheckerPluginInterface) -> Type:
    assert len(arg_exprs)==1, "expected one argument to codec(), got "+str(len(arg_exprs))
    assert isinstance(checker_api, TypeChecker), (type(checker_api), checker_api)
    result=infer_type_from_expr(arg_exprs[0],checker_api)
    verify_type_encodable(result, checker_api)
    return result
    
KnownExpressionType = ( LiteralType | UnionType | Instance | TypeAliasType | TupleType |
                        NoneType | TypeVarType | AnyType )

def infer_type_from_expr(
        expr: Expression,
        checker_api:CheckerPluginInterface) -> KnownExpressionType:
    """pre: expr is not the base of an index expression"""
    match expr:
        case IndexExpr():
            return infer_type_from_index_expr(expr.base, expr.index, checker_api)

        case OpExpr() if expr.op == '|':
            return UnionType(
                [
                    infer_type_from_expr(expr.left, checker_api),
                    infer_type_from_expr(expr.right, checker_api)
                ],
                is_evaluated = True,
                uses_pep604_syntax = True)

        case (MemberExpr() | NameExpr()) if expr.node is not None:
            if isinstance(expr.node,TypeInfo):
                match expr.node._fullname:
                    case 'builtins.list' | 'builtins.set':
                        # since we are not the base of an index expression (see pre: above)
                        # we will end up getting Any args e.g. list[Any]. Treat that as
                        # list[JsonType] because json_codec cannot encode/decode anything else.
                        assert isinstance(checker_api, TypeChecker)
                        return Instance(expr.node, [get_json_type_type(checker_api)])
                    case 'builtins.dict':
                        # Similarly treat dict as dict[str, JsonType].
                        assert isinstance(checker_api, TypeChecker)
                        return Instance(expr.node, [checker_api.named_type('builtins.str'),
                                                    get_json_type_type(checker_api)])
                    
            return infer_type_from_symbol_node(
                expr.fullname,
                expr.node,
                checker_api)
    raise CodecParamInvalid(f"{expr} is not valid as parameter to xju.json_codec.codec()")

def is_generic_list(x: Expression) -> bool:
    """does {x} represent the generic list (i.e. list without any explicit element type)"""
    return (
        isinstance(x, (NameExpr, MemberExpr)) and
        isinstance(x.node, TypeInfo) and
        x.node._fullname == 'builtins.list'
    )

def is_generic_set(x: Expression) -> bool:
    """does {x} represent the generic set (i.e. set without any explicit element type)"""
    return (
        isinstance(x, (NameExpr, MemberExpr)) and
        isinstance(x.node, TypeInfo) and
        x.node._fullname == 'builtins.set'
    )

def is_generic_tuple(x: Expression) -> bool:
    """does {x} represent the generic tuple (i.e. tuple without any explicit element type)"""
    return (
        isinstance(x, (NameExpr, MemberExpr)) and
        isinstance(x.node, TypeInfo) and
        x.node._fullname == 'builtins.tuple'
    )

def is_generic_dict(x: Expression) -> bool:
    """does {x} represent the generic dict (i.e. dict without any explicit element type)"""
    return (
        isinstance(x, (NameExpr, MemberExpr)) and
        isinstance(x.node, TypeInfo) and
        x.node._fullname == 'builtins.dict'
    )

def infer_type_from_index_expr(
        base: Expression,
        index: Expression,
        checker_api: CheckerPluginInterface) -> (
            LiteralType | UnionType | Instance | TypeAliasType | TupleType):
    # must handle Literal specially before it gets dumbed down to SpecialForm
    if isinstance(base, NameExpr) and base.fullname == 'typing.Literal':
        return get_value_literal_type(index, checker_api)
    # only generics allowed as base of index i.e as X in X[A, B, C]
    if is_generic_list(base):
        index_type = infer_type_from_expr(index, checker_api)
        assert isinstance(checker_api, TypeChecker)
        return Instance(checker_api.named_type('builtins.list').type, [index_type])
    if is_generic_set(base):
        index_type = infer_type_from_expr(index, checker_api)
        assert isinstance(checker_api, TypeChecker)
        return Instance(checker_api.named_type('builtins.set').type, [index_type])
    index_types: list[Type]
    if is_generic_tuple(base):
        if isinstance(index, TupleExpr):
            index_types = [infer_type_from_expr(x, checker_api) for x in index.items]
        else:
            index_types = [infer_type_from_expr(index, checker_api)]
        assert isinstance(checker_api, TypeChecker)
        return TupleType(index_types, tuple_fallback(TupleType(
            index_types,
            checker_api.named_type('builtins.tuple'))))
    if is_generic_dict(base):
        if isinstance(index, TupleExpr):
            if len(index.items) == 2:
                index_types = [infer_type_from_expr(x, checker_api) for x in index.items]
                assert isinstance(checker_api, TypeChecker)
                return Instance(checker_api.named_type('builtins.dict').type, index_types)
    if isinstance(base, (MemberExpr, NameExpr)):
        args_as_types: list[Type]
        match base.node:
            case TypeInfo():
                args_as_types=[
                    infer_type_from_expr(arg_expr,checker_api)
                    for arg_expr in (index.items if isinstance(index, TupleExpr) else [index])
                ]
                return Instance(base.node, args_as_types)
            case TypeAlias():
                args_as_types=[
                    infer_type_from_expr(arg_expr,checker_api)
                    for arg_expr in (index.items if isinstance(index, TupleExpr) else [index])
                ]
                return TypeAliasType(base.node, args_as_types)
    raise CodecParamInvalid(f"{base}[...] is not valid as xju.json_codec.codec() parameter")

def get_value_literal_type(expr: Expression, checker_api: CheckerPluginInterface) -> LiteralType | UnionType:
    match expr:
        case StrExpr():
            return LiteralType(expr.value, builtin_type(checker_api, 'str'))
        case IntExpr():
            return LiteralType(expr.value, builtin_type(checker_api, 'int'))
        # surprise... bool literal e.g. True is represented as a
        # NameExpr with name "True"
        case NameExpr() if expr.name == "True":
            return LiteralType(True, builtin_type(checker_api, 'bool'))
        case NameExpr() if expr.name == "False":
            return LiteralType(False, builtin_type(checker_api, 'bool'))
        case MemberExpr() if isinstance(expr.expr, NameExpr) and is_enum(expr.expr.node):
            return get_enum_member_literal_type(expr.name, expr.expr.node, checker_api)
        case TupleExpr():
            return UnionType([ get_value_literal_type(item, checker_api) for item in expr.items],
                             is_evaluated = True,
                             uses_pep604_syntax = True)
    raise CodecParamInvalid(f"{expr} is not valid for use with Literal[]")

def get_enum_member_literal_type(name: str, enum_class: TypeInfo | TypeAlias, checker_api) -> LiteralType | UnionType:
    """get {enum_class}.{name} value as a Literal[X] type assuming enum_class is an enum"""
    assert is_enum(enum_class), enum_class
    match enum_class:
        case TypeInfo():
            return LiteralType(name, Instance(enum_class, []))
        case TypeAlias():
            target = get_proper_type(enum_class.target)
            if isinstance(target, Instance) and not target.args:
                return LiteralType(name, target)
            raise CodecParamInvalid(f"{name} is not valid for use with Literal[]")

def infer_type_from_symbol_node(
        name: str,
        node: SymbolNode,
        checker_api:CheckerPluginInterface) -> (
            Instance | NoneType | TypeAliasType | TypeVarType | AnyType | TupleType | NoneType):
    match node:
        case TypeInfo() if node._fullname == 'types.NoneType':
            return NoneType()
        case TypeInfo():
            # codec return type should be instance of the TypeInfo() with no args:
            # (Pdb) p type(return_type)
            # <class 'mypy.types.Instance'>
            # (Pdb) p return_type.args
            # ()
            # (Pdb) p type(return_type.type)
            # <class 'mypy.nodes.TypeInfo'>
            return Instance(node, [])
        case Var() if node.name == 'None':
            return NoneType()
        case Var() if isinstance(node.type, TypeType):
            match node.type.item:
                 case TypeVarType() | AnyType() | Instance() | TupleType() | NoneType():
                     return node.type.item
                 case CallableType():
                     raise CodecParamInvalid(f"{node.name} is a callable and so not valid as parameter to xju.json_codec.codec()")
        case Var():
            raise CodecParamInvalid(f"{name} is a variable and so not valid as parameter to xju.json_codec.codec()")
        case FuncDef():
            raise CodecParamInvalid(f"{name} is a function and so not valid as parameter to xju.json_codec.codec()")
        case None:
            # guessing what None means, not explained in mypy/nodes.py
            raise CodecParamInvalid(f"{name} is undefined and so not valid as parameter to xju.json_codec.codec()")
        case TypeAlias():  # undocumented in mypy/nodes.py
            return TypeAliasType(node, [])
            pass
    raise CodecParamInvalid(f"{name} is not valid as xju.json_codec.codec() parameter because type {type(node)} is unexpected")

def is_xju_newtype(t: TypeInfo) -> bool:
    """is {t} an xju.newtype.X[Y]"""
    return (
        len(t.bases) == 1 and
        t.bases[0].type._fullname in (
            'xju.newtype.Int',
            'xju.newtype.Float',
            'xju.newtype.Str',
            'xju.newtype.Bool',
        )
    )

@overload
def is_enum(t: TypeInfo) -> TypeGuard[TypeInfo]: ...
@overload
def is_enum(t: TypeAlias | None) -> TypeGuard[TypeAlias]: ...
@overload
def is_enum(t: SymbolNode | None) -> TypeGuard[TypeAlias | TypeInfo]: ...

def is_enum(t):
    """is {t} a TypeInfo for an enum.Enum subclass"""
    match t:
        case TypeInfo():
            return (
                t.has_base('enum.Enum') or
                t.has_base('enum.StrEnum') or
                t.has_base('enum.IntEnum')
            )
        case TypeAlias():
            return isinstance(t.target, Instance) and is_enum(t.target.type)
    return False

def get_custom_class_codec_type(chk: TypeChecker) -> Type:
    # you'd think we could do checker_api.named_type("xju.json_codec.CustomClassCodec")):
    # but no, it looks for module xju, finds it but it is empty... the top level
    # has a module called xju.json_codec though...
    json_codec_module=chk.modules["xju.json_codec"]
    result = json_codec_module.names["CustomClassCodec"].node
    assert isinstance(result, TypeInfo)
    return Instance(result, [])

def get_custom_string_key_class_codec_type(chk: TypeChecker) -> Type:
    # you'd think we could do checker_api.named_type("xju.json_codec.CustomStringKeyClassCodec")):
    # but no, it looks for module xju, finds it but it is empty... the top level
    # has a module called xju.json_codec though...
    json_codec_module=chk.modules["xju.json_codec"]
    result = json_codec_module.names["CustomStringKeyClassCodec"].node
    assert isinstance(result, TypeInfo)
    return Instance(result, [])

def get_custom_non_string_key_class_codec_type(chk: TypeChecker) -> Type:
    # you'd think we could do checker_api.named_type("xju.json_codec.CustomNonStringKeyClassCodec")):
    # but no, it looks for module xju, finds it but it is empty... the top level
    # has a module called xju.json_codec though...
    json_codec_module=chk.modules["xju.json_codec"]
    result = json_codec_module.names["CustomNonStringKeyClassCodec"].node
    assert isinstance(result, TypeInfo)
    return Instance(result, [])

def get_json_type_type(chk: TypeChecker) -> Type:
    # you'd think we could do checker_api.named_type("xju.json_codec.JsonType")):
    # but no, it looks for module xju, finds it but it is empty... the top level
    # has a module called xju.json_codec though...
    json_codec_module=chk.modules["xju.json_codec"]
    result = json_codec_module.names["JsonType"].node
    assert isinstance(result, TypeAlias)
    return TypeAliasType(result, [])

def verify_type_encodable(
    t: KnownExpressionType,
    checker_api:CheckerPluginInterface
) -> Literal[True]:
    match t:
        case NoneType() | TypeVarType() | AnyType():
            return True
        case LiteralType():
            # can only encode str, int, bool, and enumerations of those
            if t.fallback.type._fullname in ( 'builtins.str', 'builtins.int', 'builtins.bool'):
                return True
            if is_enum(t.fallback.type) and isinstance(t.value,str):
                n=t.fallback.type.names[t.value].node
                if isinstance(n,Var):
                    if not isinstance(n.type, Instance):
                        raise CodecParamInvalid(f"unexpected enum member {t.value} type {n.type}")
                    verify_type_encodable(n.type,checker_api)
                    return True
            raise CodecParamInvalid(f"Literal[{t.value}] not valid as parameter to xju.json_codec.code()")
        case UnionType() | TupleType():
            for arg in t.items:
                if not isinstance(arg, KnownExpressionType.__args__):
                    raise CodecParamInvalid(f"unexpected union/tuple item type {arg}")
                verify_type_encodable(arg,checker_api)
            return True
        case Instance():
            if t.type.fullname in (
                    'builtins.str','builtins.bytes','builtins.int','builtins.bool','builtins.float',
                    'enum.auto'):
                return True
            assert isinstance(checker_api, TypeChecker), checker_api
            if not is_subtype(t, get_custom_class_codec_type(checker_api)):
                for arg in t.args:
                    if not isinstance(arg, KnownExpressionType.__args__):
                        raise CodecParamInvalid(f"unexpected generic type arg type {arg}")
                    verify_type_encodable(arg,checker_api)
                if t.type.fullname in ('builtins.set','builtins.list'):
                    return True
                if t.type.fullname in ('builtins.dict'):
                    # valid key types: str, int, float, bool, None
                    # or any union of int, float, bool, None
                    assert isinstance(t.args[0], KnownExpressionType.__args__)
                    verify_dict_key_type(t.args[0],checker_api)
                    return True
                for base in t.type.bases:
                    if base.type._fullname != 'builtins.object':
                        if not isinstance(base, (LiteralType,UnionType,Instance,TypeAliasType,TupleType,NoneType,TypeVarType,AnyType)):
                            raise CodecParamInvalid(f"unexpected base class type {base.type}")
                        verify_type_encodable(base,checker_api)
                for attr_name, attr_node in t.type.names.items():
                    if attr_name not in (
                        '__init__', '__match_args__', '__dataclass_fields__', '__mypy-replace', '__doc__',
                        '_DT',
                    ):
                        match attr_node.node:
                            case SymbolNode() if isinstance(attr_node.node, Var) and attr_node.node.type is not None:
                                if not isinstance(attr_node.node.type, KnownExpressionType.__args__):
                                    raise CodecParamInvalid(f"{t.type._fullname}.{attr_name} type ({attr_node.node.type}) is not encodable")
                                verify_type_encodable(attr_node.node.type,checker_api)
                            case SymbolNode() if not isinstance(attr_node.node, (FuncDef, OverloadedFuncDef, Decorator) ):
                                raise CodecParamInvalid(f"{t.type._fullname}.{attr_name} unexpected {attr_node.node}")
                            case None:
                                raise CodecParamInvalid(f"{t.type._fullname}.{attr_name} type is unknown")
            return True
        case TypeAliasType():
            for arg in t.args:
                if not isinstance(arg, KnownExpressionType.__args__):
                    raise CodecParamInvalid(f"unexpected generic type alias arg type {arg}")
                verify_type_encodable(arg,checker_api)
            if t.alias is None:
                raise CodecParamInvalid(f"unexpected type alias target None")
            if not isinstance(t.alias.target, (LiteralType,UnionType,Instance,TypeAliasType,TupleType,NoneType,TypeVarType,AnyType)):
                raise CodecParamInvalid(f"unexpected type alias target type {t.alias.target}")
            verify_type_encodable(t.alias.target,checker_api)
            return True

def verify_dict_key_type(
    t: KnownExpressionType,
    checker_api:CheckerPluginInterface,
) -> Literal['String','NonString','Any']:
    """
    verify type {t} is usable as a json dictionary key

    - return 'String' if t is string-like i.e. will never decode from int/bool/float/None
    - return 'NonString' if t is non-string-like i.e. might decode from int/bool/float/None
    - return 'Any' if t is Any
    """
    match t:
        case NoneType():
            return 'NonString'
        case TypeVarType():
            raise CodecParamInvalid("dict keys must be str,xju.newtype.Str or any union of int, float, bool, None, xju.newtype.Int, xju.newtype.Float, xju.newtype.Bool (not TypeVar)")
        case AnyType():
            # Any must bypass type checking
            return 'Any'
        case LiteralType():
            # can only encode str, int, bool, and enumerations of those
            if t.fallback.type._fullname in ( 'builtins.str',):
                return 'String'
            if t.fallback.type._fullname in ( 'builtins.int', 'builtins.bool', 'xju.time.Timestamp'):
                return 'NonString'
            if is_enum(t.fallback.type) and isinstance(t.value,str):
                n=t.fallback.type.names[t.value].node
                if isinstance(n,Var):
                    if not isinstance(n.type, Instance):
                        raise CodecParamInvalid(f"unexpected enum member {t.value} type {n.type}")
                    verify_type_encodable(n.type,checker_api)
                    return verify_dict_key_type(n.type,checker_api)
            raise CodecParamInvalid(f"Literal[{t.value}] not valid as parameter to xju.json_codec.code()")
        case UnionType():
            string_args=list[KnownExpressionType]()
            non_string_args=list[KnownExpressionType]()
            for arg in t.items:
                if not isinstance(arg, KnownExpressionType.__args__):
                    raise CodecParamInvalid(f"unexpected generic type arg type {arg}")
                match verify_dict_key_type(arg,checker_api):
                    case 'String':
                        string_args.append(arg)
                    case 'NonString':
                        non_string_args.append(arg)
                    case 'Any':
                        raise CodecParamInvalid(f"Any is not a valid union-type member")
                pass
            if string_args and non_string_args:
                raise CodecParamInvalid(f"union dict key types must not be a mix of string-like ({string_args}) and non-string-like ({non_string_args})")
                pass
            return 'String' if string_args else 'NonString'
        case TupleType():
            raise CodecParamInvalid("dict keys must be str,xju.newtype.Str or any union of int, float, bool, None, xju.newtype.Int, xju.newtype.Float, xju.newtype.Bool (not tuple)")
        case Instance():
            if is_xju_newtype(t.type):
                return 'String' if t.type.bases[0].type._fullname=='xju.newtype.Str' else 'NonString'
            if t.type.fullname in ('builtins.str',):
                return 'String'
            if t.type.fullname in ('builtins.int','builtins.bool','builtins.float','xju.time.Timestamp'):
                return 'NonString'
            if is_enum(t.type):
                for attr_name, attr_node in t.type.names.items():
                    if attr_name not in (
                        '__init__', '__match_args__', '__dataclass_fields__', '__mypy-replace', '__doc__',
                        '_DT',
                    ):
                        match attr_node.node:
                            case SymbolNode() if isinstance(attr_node.node, Var) and attr_node.node.type is not None:
                                if not isinstance(attr_node.node.type, KnownExpressionType.__args__):
                                    raise CodecParamInvalid(f"{t.type._fullname}.{attr_name} type ({attr_node.node.type}) is not encodable")
                                string_members=list[str]()
                                non_string_members=list[str]()
                                match verify_dict_key_type(attr_node.node.type,checker_api):
                                    case 'String':
                                        string_members.append(attr_name)
                                    case 'NonString':
                                        non_string_members.append(attr_name)
                                    case 'Any':
                                        raise CodecParamInvalid(
                                            f"member {attr_name}: Any is not a valid enum value type")
                                if string_members and non_string_members:
                                    raise CodecParamInvalid(f"enum member types must not be a mix of string-like ({string_members}) and non-string-like ({non_string_members})")
                                    pass
                                return 'String' if string_members else 'NonString'
                            case SymbolNode() if not isinstance(attr_node.node, (FuncDef, OverloadedFuncDef, Decorator) ):
                                raise CodecParamInvalid(f"{t.type._fullname}.{attr_name} unexpected {attr_node.node}")
                            case None:
                                raise CodecParamInvalid(f"{t.type._fullname}.{attr_name} type is unknown")
            assert isinstance(checker_api, TypeChecker), checker_api
            if is_subtype(t, get_custom_class_codec_type(checker_api)):
                if is_subtype(t, get_custom_string_key_class_codec_type(checker_api)):
                    return 'String'
                if is_subtype(t, get_custom_non_string_key_class_codec_type(checker_api)):
                    return 'NonString'
                raise CodecParamInvalid(f"{t.type.fullname} has custom encoding but is neither a CustomStringKeyClassCodec nor a CustomNonStringKeyClassCodec: {t.type.fullname} must implement one of these if it is to be used as a dictionary key")
            if t.type.is_newtype:
                if t.type.bases[0].type.fullname == 'builtins.str':
                    return 'String'
                if t.type.bases[0].type.fullname in ('builtins.int','builtins.bool','builtins.float'):
                    return 'NonString'
                raise CodecParamInvalid(f"NewType {t.type.fullname} is not based on str, int, bool or float and so cannot be used as a dictionary key")
            raise CodecParamInvalid(f"dict keys must be str,xju.newtype.Str or any union of int, float, bool, None, xju.newtype.Int, xju.newtype.Float, xju.newtype.Bool (not {t.type.fullname})")
        case TypeAliasType():
            if t.args:
                raise CodecParamInvalid("dict keys must be str,xju.newtype.Str or any union of int, float, bool, None, xju.newtype.Int, xju.newtype.Float, xju.newtype.Bool (not generic alias instance)")
            if t.alias is None:
                raise CodecParamInvalid(f"unexpected type alias target None")
            if not isinstance(t.alias.target, (LiteralType,UnionType,Instance,TypeAliasType,TupleType,NoneType,TypeVarType,AnyType)):
                raise CodecParamInvalid(f"unexpected type alias target type {t.alias.target}")
            return verify_dict_key_type(t.alias.target,checker_api)

    
def adjust_type_or_type_return_type(x: MethodContext) -> Type:
    """
    assuming x is a call to builtins.type.__or__ or builtins.type.__ror__
    with param *values* T1 and T2, adjust return type to be mypy.types.UnionType([T1,T2])
    """
    rhs_expr=x.args[0][0]
    return UnionType([x.type, x.api.get_expression_type(rhs_expr,x.type)])

def adjust_codec_return_type(x: FunctionContext) -> Type:
    """
    assuming x is a call to xju.json_codec.codec, adjust T in the return type
    (CodecProto[T]) to its proper value

    for example the call might be codec(int | str), in which case the
    argument to x has type typeof(int|str); int|str is the type we want
    to replace T with i.e. we want the value of the expression passed to x
    (which should be a type).
    """
    try:
        arg_exprs=x.args[0]
        inferred_codec_type = infer_codec_value_type_from_args(arg_exprs, x.api)
        if isinstance(inferred_codec_type, UninhabitedType):
            arg_type=x.arg_types[0]
            x.api.fail(f"type {arg_type} is not supported by {CODEC_FQN}(). Note codec() parameter must be a type, not an instance.", x.context)
            return AnyType(TypeOfAny.from_error)
        assert isinstance(x.default_return_type, Instance), x.default_return_type
        x.default_return_type.args=(inferred_codec_type, )
        return x.default_return_type
    except CodecParamInvalid as e:
        x.api.fail(str(e), x.context)
        return AnyType(TypeOfAny.from_error)
    pass

# for development to figure out what return type should be in each case via pdb
def show_return_type(x: FunctionContext) -> Type:
    return_type=get_proper_type(x.default_return_type)
    typeof_return_type=type(return_type)
    pdb_trace()
    return return_type

class JsonCodecPlugin(Plugin):
    def get_method_hook(self, fullname: str) -> Callable[[MethodContext], Type] | None:
        if fullname == '__or__ of type':
            return adjust_type_or_type_return_type
        return None

    def get_function_hook(self, fullname: str) -> Callable[[FunctionContext], Type] | None:
        if fullname==CODEC_FQN:
            return adjust_codec_return_type
        if fullname=="xju.json_codec._xxx":
            return show_return_type
        return None
    
    def get_function_signature_hook(
            self, fullname: str
    ) -> Callable[[FunctionSigContext], FunctionLike] | None:
        if fullname==CODEC_FQN:
            return adjust_codec_signature
        return None

def plugin(version: str):
    # ignore version argument if the plugin works with all mypy versions.
    return JsonCodecPlugin


def builtin_type(checker_api:CheckerPluginInterface, name: str) -> Instance:
    """lookup built-in type {name}"""
    try:
        # need more info than CheckerPluginInterface exposes
        assert isinstance(checker_api, TypeChecker), (type(checker_api), checker_api)
        # checker_api.named_type is good enough for builtins
        return checker_api.named_type(name)
    except Exception as e:
        raise Exception(f"failed to lookup built-in type {name} becase {e}")

def pdb_trace() -> None:
    import pdb; pdb.set_trace()
    pass
