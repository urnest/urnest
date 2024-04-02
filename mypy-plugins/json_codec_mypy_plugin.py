from typing import Callable
from mypy.plugin import (
    Plugin, FunctionSigContext, FunctionContext, Expression,
    CheckerPluginInterface
)
from mypy.nodes import (
    Expression, NameExpr, IndexExpr, TupleExpr, StrExpr, IntExpr, FloatExpr, OpExpr, MemberExpr,
    TypeVarExpr, CallExpr,
    TypeInfo,
    TypeAlias
)
from mypy.types import (
    AnyType,
    CallableType,
    FunctionLike,
    Instance,
    LiteralType,
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
)
from mypy.typeops import tuple_fallback
from mypy.checker import TypeChecker

CODEC_FQN="xju.json_codec.codec"


def adjust_codec_signature(s: FunctionSigContext) -> FunctionLike:
    """adjust signature to Any -> Any so we can param type after mypy resolves param type"""
    if len(s.args) != 1:
        l=len(s.args)
        s.api.fail(f"{CODEC_FQN}() takes exactly one parameter, not {l}", s.context)
        return s.default_signature
    s.default_signature.arg_types[0] = AnyType(TypeOfAny.explicit)
    return s.default_signature

def flatten_union_terms(expr: OpExpr) -> list[Expression] | UninhabitedType:
    if expr.op != "|":
        return UninhabitedType()
    left = flatten_union_terms(expr.left) if isinstance(expr.left, OpExpr) else [expr.left]
    right = flatten_union_terms(expr.right) if isinstance(expr.right, OpExpr) else [expr.right]
    if isinstance(left, UninhabitedType) or isinstance(right, UninhabitedType):
        return UninhabitedType()
    return left+right

def infer_literal_type(expr: Expression, checker_api:CheckerPluginInterface) -> LiteralType:
    try:
        match expr:
            case StrExpr():
                return LiteralType(
                    expr.value,
                    builtin_type(checker_api,'str'))
            case IntExpr():
                return LiteralType(
                    expr.value,
                    builtin_type(checker_api,'int'))
            case FloatExpr():
                return LiteralType(
                    expr.value,
                    builtin_type(checker_api,'float'))
            # surprise... bool literal e.g. True is represented as a
            # NameExpr with name "True"
            case NameExpr():
                return LiteralType(
                    expr.name == "True",
                    builtin_type(checker_api,'bool'))
        raise Exception(f"{expr} is not one of StrExpr, IntExpr, FloatExpr, NameExpr")
    except Exception as e:
        raise Exception(f"failed to infer type for presumed literal {expr} becase {e}") from None
    pass

class CodecParamInvalid(Exception):
    pass
    
def infer_codec_value_type_from_args(arg_exprs: list[Expression],
                                     checker_api:CheckerPluginInterface) -> Type:
    assert len(arg_exprs)==1, "expected one argument to codec(), got "+str(len(arg_exprs))
    return infer_codec_value_type(arg_exprs[0],checker_api)

def calculate_overload_return_type(overload: Overloaded) -> Type:
    return_types={_.ret_type for _ in overload.items}
    match len(return_types):
        case 1:
            return return_types.pop()
        case _:
            return UnionType(list(return_types))
    pass

def infer_codec_value_type(arg_expr: Expression,
                           checker_api:CheckerPluginInterface) -> Type:
    #import pdb; pdb.set_trace()
    try:
        match arg_expr:
            case NameExpr() if arg_expr.fullname == 'types.NoneType':
                return NoneType()
            case NameExpr() if arg_expr.name == 'None':
                return NoneType()
            case NameExpr() if arg_expr.fullname.startswith('builtins.'):
                return builtin_type(checker_api,arg_expr.name)
            case MemberExpr() | NameExpr() | CallExpr() | IndexExpr():
                # expression with dots, e.g. xju.misc.BitsPerSecond
                # note unqualified names end up in NameExpr() below
                #import pdb; pdb.set_trace()
                c=checker_api.type_context[0]
                result=checker_api.get_expression_type(arg_expr, c)
                # type(X) where X is a class give result Callable, which
                # is X's constructor, we want just X which is the return
                # type of the callable; note this will consider any function
                # passed to codec() to be a constructor (which it is)
                match result:
                    case CallableType():
                        if not isinstance(result.ret_type, Type):
                            raise CodecParamInvalid(f"apparently {result.ret_type} is not a my.types.type?")
                        return result.ret_type
                    case Overloaded():
                        return calculate_overload_return_type(result)
                    case TypeType():
                        return result.item
                    # type alias ref X where X = int | str comes back as typing._SpecialForm
                    # with no useful info in result; as does member expression y.X
                    case Instance() if result.type.fullname == 'typing._SpecialForm':
                        return TypeAliasType(arg_expr.node, [])
                return result
            #?case StrExpr():   e.g. codec("fred")
            case OpExpr() if arg_expr.op == "|":
                # union
                terms = flatten_union_terms(arg_expr)
                if isinstance(terms, UninhabitedType):
                    return terms
                return UnionType([infer_codec_value_type(a, checker_api) for a in terms])
        return UninhabitedType()
    except Exception as e:
        raise Exception(f"failed to infer proper {CODEC_FQN}() return type from expression {arg_expr} because {e}; report this to https://github/urnest") from None

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
    assert isinstance(x.default_return_type, Instance), x.default_return_type
    return_type=x.default_return_type
    typeof_return_type=type(return_type)
    #import pdb; pdb.set_trace()
    return return_type

class JsonCodecPlugin(Plugin):
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


def named_type(checker_api:CheckerPluginInterface, expr: NameExpr) -> (
        Instance | TypeAliasType | TypeVarType | UninhabitedType
):
    """lookup type of expression {expr}
    
    - name can be an alias -> TypeAliasType
    - otherwise -> Instance
    """
    try:
        # need more info than CheckerPluginInterface exposes
        assert isinstance(checker_api, TypeChecker), (type(checker_api), checker_api)

        # checker_api.named_type is not good enough as it does not handle aliases properly

        # apparently checker_api keeps context, so we have to give the unqualified name
        # not the fullname
        try:
            node = checker_api.lookup_qualified(expr.name).node
        except KeyError:
            return UninhabitedType()
        if isinstance(node, TypeAlias):
            return TypeAliasType(node, [])
        if isinstance(node, TypeVarExpr):
            #import pdb; pdb.set_trace()
            return TypeVarType(
                node.name,
                node.fullname,
                TypeVarId.new(0),
                node.values,
                node.upper_bound,
                AnyType(TypeOfAny.from_omitted_generics),
                node.variance)
        assert isinstance(node, TypeInfo), f"{expr} is not a type, it is the {node} {node.__class__}"
        return Instance(node, [])
    except Exception as e:
        raise Exception(f"failed to lookup type named by {expr} becase {e}") from None
