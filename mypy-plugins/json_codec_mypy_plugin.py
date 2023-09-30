from typing import Callable
from mypy.plugin import (
    Plugin, FunctionSigContext, FunctionContext, Expression,
    CheckerPluginInterface
)
from mypy.nodes import Expression, NameExpr, IndexExpr, TupleExpr, StrExpr, IntExpr, FloatExpr, OpExpr
from mypy.types import (
    AnyType,
    FunctionLike,
    Instance,
    LiteralType,
    NoneType,
    TupleType,
    Type,
    TypeOfAny,
    UninhabitedType,
    UnionType,
)
from mypy.typeops import tuple_fallback

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
                    checker_api.named_generic_type('str', []))
            case IntExpr():
                return LiteralType(
                    expr.value,
                    checker_api.named_generic_type('int', []))
            case FloatExpr():
                return LiteralType(
                    expr.value,
                    checker_api.named_generic_type('float', []))
            # surprise... bool literal e.g. True is represented as a
            # NameExpr with name "True"
            case NameExpr():
                return LiteralType(
                    expr.name == "True",
                    checker_api.named_generic_type('bool', []))
        raise Exception(f"{expr} is not one of StrExpr, IntExpr, FloatExpr, NameExpr")
    except Exception as e:
        raise Exception(f"failed to infer type for presumed literal {expr} becase {e}") from None

def infer_codec_value_type(arg_expr: Expression | list[Expression],
                           checker_api:CheckerPluginInterface) -> Type:
    # import pdb; pdb.set_trace()
    try:
        match arg_expr:
            case list():
                if len(arg_expr) != 1:
                    return UninhabitedType()
                return infer_codec_value_type(arg_expr[0], checker_api)
            case NameExpr() if arg_expr.name == 'None':
                return NoneType()
            case NameExpr():
                return checker_api.named_generic_type(arg_expr.name, [])
            #?case StrExpr():
            case IndexExpr():
                if not isinstance(arg_expr.base, NameExpr):
                    return UninhabitedType()
                if arg_expr.base.name=="Literal" and not isinstance(arg_expr.index, TupleExpr):
                    # single literal value
                    return infer_literal_type(arg_expr.index, checker_api)
                if arg_expr.base.name=="Literal":
                        assert isinstance(arg_expr.index, TupleExpr)
                        # multi-value literal
                        return UnionType([
                            infer_literal_type(a, checker_api) for a in arg_expr.index.items])
                if isinstance(arg_expr.index, TupleExpr):
                    if arg_expr.base.name=="tuple":
                        items=[infer_codec_value_type(a, checker_api) for a in arg_expr.index.items]
                        return TupleType(
                            items,
                            checker_api.named_generic_type("tuple",[]))
                    # other e.g. dict[str,int]
                    return checker_api.named_generic_type(
                        arg_expr.base.name,
                        [infer_codec_value_type(a, checker_api) for a in arg_expr.index.items]
                    )
                # only one type param, e.g. list[int]
                return checker_api.named_generic_type(
                    arg_expr.base.name,
                    [infer_codec_value_type(arg_expr.index, checker_api)]
                )
            case OpExpr() if arg_expr.op == "|":
                # union
                terms = flatten_union_terms(arg_expr)
                if isinstance(terms, UninhabitedType):
                    return terms
                return UnionType([infer_codec_value_type(a, checker_api) for a in terms])
        return UninhabitedType()
    except Exception as e:
        raise Exception(f"failed to infer xju.json_codec.codec value type from expression {arg_expr} because{e}") from None

def adjust_codec_return_type(x: FunctionContext) -> Type:
    arg_type=x.arg_types[0]
    arg_expr=x.args[0]
    inferred_codec_type = infer_codec_value_type(arg_expr, x.api)
    if isinstance(inferred_codec_type, UninhabitedType):
        arg_value_str=[str(_) for _ in arg_expr]
        return_type=x.default_return_type
        typeof_return_type=type(return_type)
        x.api.fail(f"type {arg_type} is not supported by {CODEC_FQN}(); note the argument has value expression {arg_value_str}; note the return type is {typeof_return_type}({return_type})", x.context)
        return AnyType(TypeOfAny.from_error)
    assert isinstance(x.default_return_type, Instance)
    x.default_return_type.args=(inferred_codec_type, )
    return x.default_return_type

# for development to figure out what return type should be in each case via pdb
def show_return_type(x: FunctionContext) -> Type:
    assert isinstance(x.default_return_type, Instance)
    return_type=x.default_return_type.args[0]
    typeof_return_type=type(return_type)
    import pdb; pdb.set_trace()
    return return_type

class JsonCodecPlugin(Plugin):
    def get_function_hook(self, fullname: str) -> Callable[[FunctionContext], Type] | None:
        if fullname==CODEC_FQN:
            return adjust_codec_return_type
        if fullname=="xju.json_codec.xxx":
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
