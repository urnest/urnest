from typing import Callable, Final, Self
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
from mypy.checkexpr import ExpressionChecker

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
    assert isinstance(checker_api, TypeChecker), (type(checker_api), checker_api)
    with ExpressionCheckerPatch(checker_api.expr_checker):
        return infer_codec_value_type(arg_exprs[0],checker_api)

def calculate_overload_return_type(overload: Overloaded) -> Type:
    return_types={_.ret_type for _ in overload.items}
    match len(return_types):
        case 1:
            return return_types.pop()
        case _:
            return UnionType(list(return_types))
    pass

def expand_member_expr(expr: MemberExpr) -> NameExpr:
    qualifiers=list[str]()
    e:Expression = expr
    while isinstance(e,MemberExpr):
        qualifiers.append(e.name)
        e=e.expr
        pass
    assert isinstance(e,NameExpr),e
    return NameExpr('.'.join(reversed(qualifiers+[e.name])))

def infer_codec_value_type(
        arg_expr: Expression,
        checker_api:CheckerPluginInterface,
) -> Type:
    """
    infer codec({arg_expr}) return type

    - raise CodecParamInvalid for invalid param e.g. as in codec(7)
    """
    try:
        # rely on Type[X] where X is a type always producing a type constructor
        # function def (...)->X  or a TypeType
        c=checker_api.type_context[0]
        expr_type=checker_api.get_expression_type(arg_expr, c)
        match expr_type:
            # codec(O.a) where O is an enum gives us Instance with LiteralType last known value
            case Instance() if isinstance(expr_type.last_known_value, LiteralType):
                return expr_type.last_known_value
            case CallableType()|Overloaded()|TypeType()|UnionType():
                result=type_type_instance_type(expr_type)
                if not isinstance(result, Type):
                    raise CodecParamInvalid(f"apparently {result} (which the return type of {arg_expr}) is not a my.types.type?")
                return result
        expr_type_type=type(expr_type)
        raise CodecParamInvalid(f"apparently {expr_type} (which is of type {expr_type_type} and is the type of the runtime value of {arg_expr}) is not a type (because it is not a callable, a type-type or a union type)")
    except Exception as e:
        raise Exception(f"failed to infer proper {CODEC_FQN}() return type from expression {arg_expr} because {e}; report this to https://github/urnest") from None

def type_type_instance_type(expr_type: CallableType|Overloaded|TypeType|UnionType) -> Type:
    match expr_type:
        case UnionType():
            return expr_type
        case TypeType():
            return expr_type.item
        case Overloaded():
            return calculate_overload_return_type(expr_type)
        case CallableType():
            return expr_type.ret_type

def adjust_type_or_type_return_type(x: MethodContext) -> Type:
    """
    assuming x is a call to builtins.type.__or__ or builtins.type.__ror__
    with param *values* T1 and T2, adjust return type to be mypy.types.UnionType([T1,T2])
    """
    rhs_expr=x.args[0][0]
    return UnionType([x.type, infer_codec_value_type(rhs_expr,x.api)])

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
    return_type=x.default_return_type
    typeof_return_type=type(return_type)
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


class ExpressionCheckerPatch:
    def __init__(self, expression_checker: ExpressionChecker) -> None:
        self.expression_checker=expression_checker
        self._f=self.visit_index_expr_helper
        self._f2=self.alias_type_in_runtime_context
    @property
    def chk(self) -> TypeChecker:
        return self.expression_checker.chk
    def __enter__(self) -> Self:
        self.f_=self.expression_checker.visit_index_expr_helper
        setattr(self.expression_checker,'visit_index_expr_helper',self.visit_index_expr_helper)
        self.f2_=self.expression_checker.alias_type_in_runtime_context
        setattr(self.expression_checker,'alias_type_in_runtime_context',
                self.alias_type_in_runtime_context)
        return self
    def __exit__(self, *_) -> None:
        setattr(self.expression_checker,'alias_type_in_runtime_context',self.f2_)
        self.f2_=self.alias_type_in_runtime_context
        setattr(self.expression_checker,'visit_index_expr_helper',self.f_)
        self.f_=self.visit_index_expr_helper
        pass
    def visit_index_expr_helper(self, e: IndexExpr) -> Type:
        if e.analyzed:
            # It's actually a type application.
            return self.expression_checker.accept(e.analyzed)
        left_type = self.expression_checker.accept(e.base)
        if (
            # left_type is vague when it is a _SpecialForm e.g. Literal
            # so we can't ever turn it back into e.g. LiteralType; we
            # can only do that based on e.base, which still has the info
            # we need. A _SpecialForm is reprsented as an no-args Instance
            # with TypeInfo fullname "typing._SpecialForm"
            # I'd reckon ultimately _SpecialForm should not exist, python
            # should have specific types for the specific special forms
            # that would remove all this special case stuff; but for
            # now follow typeanal.py's try_analyze_special_unbound_type lead...
            isinstance(left_type, Instance) and
            isinstance(left_type.type, TypeInfo) and
            not left_type.args and
            left_type.type.fullname == 'typing._SpecialForm' and
            isinstance(e.base, NameExpr)
        ):
            #pdb_trace()
            return (
                self.visit_special_form_index(e.base.fullname, e.index) or
                self.f_(e)
            )
        return self.f_(e)
    def visit_special_form_index(self, base_fullname: str, index: Expression) -> Type | None:
        """
        Visit index into "special form" {base_fullname}[{index}], returning the type of that value.
        """
        if base_fullname in LITERAL_TYPE_NAMES:
            values = self.get_literal_values(index)
            if not values:
                return None
            if len(values)==1:
                value=values[0]
                expr_value=LiteralType(value[0], self.chk.named_type(value[1]))
                # need the type of that value, e.g. as a constructor function
                return CallableType([], [], [], expr_value,
                                    self.chk.named_type('function'))
            return UnionType(
                [CallableType([],[],[],
                              LiteralType(value[0], self.chk.named_type(value[1])),
                              self.chk.named_type('function')) for value in values])
        return None
    def get_literal_values(self, expr: Expression) -> (
            list[tuple[str|int|bool, str]]
    ):
        if isinstance(expr, StrExpr):
            return [(expr.value, 'str')]
        if isinstance(expr, IntExpr):
            return [(expr.value, 'int')]
        # surprise... bool literal e.g. True is represented as a
        # NameExpr with name "True"
        if isinstance(expr, NameExpr):
            if expr.name == "True":
                return [(True, 'bool')]
            if expr.name == "False":
                return [(False, 'bool')]
        import pdb; pdb.set_trace
        return []
    def alias_type_in_runtime_context(
        self, alias: TypeAlias, *, ctx: Context, alias_definition: bool = False
    ) -> Type:
        if isinstance(alias.target, LiteralType):
            return CallableType([],[],[],
                                alias.target,
                                self.chk.named_type('function'))
        return self.f2_(alias, ctx=ctx, alias_definition=alias_definition)
    pass

def named_type(checker_api:CheckerPluginInterface, expr: NameExpr) -> (
        Instance | TypeAliasType | TypeVarType | UninhabitedType
):
    """lookup type of expression {expr}
    
       could be:
         literal type e.g. int, Y where Y is a type  -> Instance
         type alias e.g. X where X = Y  -> TypeAliasType
         type alias to generic type X where X = C1 where class C1(Generic[T])
         typevar T where T = TypeVar('T')
         typevartuple Ts where T = TypeVarTuple('Ts')
         variable whose value is not a type e.g. x where x = 7, x: Y
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

def pdb_trace() -> None:
    import pdb; pdb.set_trace()
    pass
