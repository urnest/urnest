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
from mypy.erasetype import erase_typevars

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
        #pdb_trace()
        expr_type=checker_api.get_expression_type(arg_expr, c)

        # codec(O.a) where O is an enum gives us Instance with LiteralType last known value
        if isinstance(expr_type, Instance) and isinstance(expr_type.last_known_value, LiteralType):
            return expr_type.last_known_value

        result=type_type_instance_type(expr_type)
        # returning a generic type is only allowed if expression is an index expression
        # otherwise all type vars become Any (see
        # "note on mypy handling of generic type used without params" in xju/json_codec.py.test)
        if not isinstance(arg_expr, IndexExpr):
            result=erase_typevars(result)
            pass
        if not isinstance(result, Type):
            raise CodecParamInvalid(f"apparently {result} (which the return type of {arg_expr}) is not a my.types.type?")
        return result
    except Exception as e:
        raise Exception(f"failed to infer proper {CODEC_FQN}() return type from run-time value of expression {arg_expr} because {e}; report this to https://github/urnest") from None
    pass

def type_type_instance_type(expr_type: Type) -> Type:
    """
    return instance type of {expr_type} assuming that is a "type" type
    e.g. return the T from Type[T]
    """
    match expr_type:
        case NoneType():
            return expr_type
        case UnionType():
            return UnionType([type_type_instance_type(t) for t in expr_type.items])
        case TypeType():
            return expr_type.item
        case Overloaded():
            return calculate_overload_return_type(expr_type)
        case CallableType():
            return expr_type.ret_type

    expr_type_type=type(expr_type)
    raise CodecParamInvalid(f"apparently {expr_type} (which is of type {expr_type_type} and is the run-time expression value) is not a type (because it is not a callable, a type-type, a union type or None-type)")

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
    return_type=x.default_return_type
    typeof_return_type=type(return_type)
    #pdb_trace()
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
        self._f3=check_callable_call
        self._f4=apply_type_arguments_to_callable
    @property
    def chk(self) -> TypeChecker:
        return self.expression_checker.chk
    def __enter__(self) -> Self:
        self._f=self.expression_checker.visit_index_expr_helper
        setattr(self.expression_checker,'visit_index_expr_helper',self.visit_index_expr_helper)
        self._f2=self.expression_checker.alias_type_in_runtime_context
        setattr(self.expression_checker,'alias_type_in_runtime_context',
                self.alias_type_in_runtime_context)
        self._f3=ExpressionChecker.check_callable_call
        setattr(ExpressionChecker, 'check_callable_call',check_callable_call)
        self._f4=ExpressionChecker.apply_type_arguments_to_callable
        setattr(ExpressionChecker, 'apply_type_arguments_to_callable',apply_type_arguments_to_callable)
        return self
    def __exit__(self, *_) -> None:
        setattr(ExpressionChecker,'apply_type_arguments_to_callable',self._f4)
        self._f4=apply_type_arguments_to_callable
        setattr(ExpressionChecker,'check_callable_call',self._f3)
        self._f3=check_callable_call
        setattr(self.expression_checker,'alias_type_in_runtime_context',self._f2)
        self._f2=self.alias_type_in_runtime_context
        setattr(self.expression_checker,'visit_index_expr_helper',self._f)
        self._f=self.visit_index_expr_helper
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
            return (
                self.visit_special_form_index(e.base.fullname, e.index) or
                self._f(e)
            )
        return self._f(e)
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
            return CallableType([],[],[],
                                UnionType(
                                    [LiteralType(value[0], self.chk.named_type(value[1]))
                                     for value in values]), self.chk.named_type('function'))
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
        if isinstance(expr, TupleExpr):
            return sum([self.get_literal_values(item) for item in expr.items],[])
        return []
    def alias_type_in_runtime_context(
        self, alias: TypeAlias, *, ctx: Context, alias_definition: bool = False
    ) -> Type:
        # LiteralType('xxx') is mypy's representation of python type Literal['xxx']
        # which will be the target of the alias, note that is "va;ue" and we need
        # to return the type of that value, i.e. a constructor function for
        # LiteralType('xxx')
        if isinstance(alias.target, LiteralType):
            return CallableType([],[],[],
                                alias.target,
                                self.chk.named_type('function'))
        # you'd think mypy would represent Literal('xxx','yyy') as
        # LiteralType(['xxx','yyy']) but no, the representation is
        # UnionType([LiteralType('xxx'),LiteralType('yyy')
        if isinstance(alias.target, UnionType) and all(
                isinstance(x, LiteralType) for x in alias.target.items):
            return CallableType([],[],[],
                                alias.target,
                                self.chk.named_type('function'))
        return self._f2(alias, ctx=ctx, alias_definition=alias_definition)
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

# patch for mypy 1.9.0 checkexpr.ExpressionChecker.check_callable_call
from mypy.checkexpr import (
    ExpressionChecker,
    CallableType,
    Expression,
    ArgKind,
    Context,
    Sequence,
    Type,
    get_proper_type,
    RefExpr,
    ENUM_BASES,
    message_registry,
    IMPLICITLY_ABSTRACT,
    UnpackType,
    ARG_STAR,
    codes,
    map_actuals_to_formals,
    freshen_all_functions_type_vars,
    freeze_all_type_vars,
    ParamSpecType,
    TypeVarTupleType,
    ARG_STAR2,
    is_equivalent,
    freshen_function_type_vars,
    ParamSpecFlavor,
    ARG_POS,
)
# patched mypy 1.9.0 source
def check_callable_call(
    self: ExpressionChecker,
    callee: CallableType,
    args: list[Expression],
    arg_kinds: list[ArgKind],
    context: Context,
    arg_names: Sequence[str | None] | None,
    callable_node: Expression | None,
    callable_name: str | None,
    object_type: Type | None,
) -> tuple[Type, Type]:
    """Type check a call that targets a callable value.

    See the docstring of check_call for more information.
    """
    # Always unpack **kwargs before checking a call.
    callee = callee.with_unpacked_kwargs().with_normalized_var_args()
    if callable_name is None and callee.name:
        callable_name = callee.name
    ret_type = get_proper_type(callee.ret_type)
    if callee.is_type_obj() and isinstance(ret_type, Instance):
        callable_name = ret_type.type.fullname
    if isinstance(callable_node, RefExpr) and callable_node.fullname in ENUM_BASES:
        # An Enum() call that failed SemanticAnalyzerPass2.check_enum_call().
        return callee.ret_type, callee

    if (
        callee.is_type_obj()
        and callee.type_object().is_protocol
        # Exception for Type[...]
        and not callee.from_type_type
    ):
        self.chk.fail(
            message_registry.CANNOT_INSTANTIATE_PROTOCOL.format(callee.type_object().name),
            context,
        )
    elif (
        callee.is_type_obj()
        and callee.type_object().is_abstract
        # Exception for Type[...]
        and not callee.from_type_type
        and not callee.type_object().fallback_to_any
    ):
        type = callee.type_object()
        # Determine whether the implicitly abstract attributes are functions with
        # None-compatible return types.
        abstract_attributes: dict[str, bool] = {}
        for attr_name, abstract_status in type.abstract_attributes:
            if abstract_status == IMPLICITLY_ABSTRACT:
                abstract_attributes[attr_name] = self.can_return_none(type, attr_name)
            else:
                abstract_attributes[attr_name] = False
        self.msg.cannot_instantiate_abstract_class(
            callee.type_object().name, abstract_attributes, context
        )

    var_arg = callee.var_arg()
    if var_arg and isinstance(var_arg.typ, UnpackType):
        # It is hard to support multiple variadic unpacks (except for old-style *args: int),
        # fail gracefully to avoid crashes later.
        seen_unpack = False
        for arg, arg_kind in zip(args, arg_kinds):
            if arg_kind != ARG_STAR:
                continue
            arg_type = get_proper_type(self.accept(arg))
            if not isinstance(arg_type, TupleType) or any(
                isinstance(t, UnpackType) for t in arg_type.items
            ):
                if seen_unpack:
                    self.msg.fail(
                        "Passing multiple variadic unpacks in a call is not supported",
                        context,
                        code=codes.CALL_ARG,
                    )
                    return AnyType(TypeOfAny.from_error), callee
                seen_unpack = True

    formal_to_actual = map_actuals_to_formals(
        arg_kinds,
        arg_names,
        callee.arg_kinds,
        callee.arg_names,
        lambda i: self.accept(args[i]),
    )

    # This is tricky: return type may contain its own type variables, like in
    # def [S] (S) -> def [T] (T) -> tuple[S, T], so we need to update their ids
    # to avoid possible id clashes if this call itself appears in a generic
    # function body.
    ret_type = get_proper_type(callee.ret_type)
    if isinstance(ret_type, CallableType) and ret_type.variables:
        fresh_ret_type = freshen_all_functions_type_vars(callee.ret_type)
        freeze_all_type_vars(fresh_ret_type)
        callee = callee.copy_modified(ret_type=fresh_ret_type)

    if callee.is_generic():
        need_refresh = any(
            isinstance(v, (ParamSpecType, TypeVarTupleType)) for v in callee.variables
        )
        callee = freshen_function_type_vars(callee)
        callee = self.infer_function_type_arguments_using_context(callee, context)
        if need_refresh:
            # Argument kinds etc. may have changed due to
            # ParamSpec or TypeVarTuple variables being replaced with an arbitrary
            # number of arguments; recalculate actual-to-formal map
            formal_to_actual = map_actuals_to_formals(
                arg_kinds,
                arg_names,
                callee.arg_kinds,
                callee.arg_names,
                lambda i: self.accept(args[i]),
            )
        callee = self.infer_function_type_arguments(
            callee, args, arg_kinds, arg_names, formal_to_actual, need_refresh, context
        )
        if need_refresh:
            formal_to_actual = map_actuals_to_formals(
                arg_kinds,
                arg_names,
                callee.arg_kinds,
                callee.arg_names,
                lambda i: self.accept(args[i]),
            )

    param_spec = callee.param_spec()
    if param_spec is not None and arg_kinds == [ARG_STAR, ARG_STAR2]:
        arg1 = self.accept(args[0])
        arg2 = self.accept(args[1])
        if (
            isinstance(arg1, ParamSpecType)
            and isinstance(arg2, ParamSpecType)
            and arg1.flavor == ParamSpecFlavor.ARGS
            and arg2.flavor == ParamSpecFlavor.KWARGS
            and arg1.id == arg2.id == param_spec.id
        ):
            return callee.ret_type, callee

    arg_types = self.infer_arg_types_in_context(callee, args, arg_kinds, formal_to_actual)

    self.check_argument_count(
        callee,
        arg_types,
        arg_kinds,
        arg_names,
        formal_to_actual,
        context,
        object_type,
        callable_name,
    )

    self.check_argument_types(
        arg_types, arg_kinds, args, callee, formal_to_actual, context, object_type=object_type
    )

    if (
        callee.is_type_obj()
        and (len(arg_types) == 1)
        and is_equivalent(callee.ret_type, self.named_type("builtins.type"))
    ):
        callee = callee.copy_modified(ret_type=TypeType.make_normalized(arg_types[0]))

    if callable_node:
        # Store the inferred callable type.
        self.chk.store_type(callable_node, callee)

    if object_type is not None and callee.name is not None and self.plugin.get_method_hook(callee.name):
        new_ret_type = self.apply_function_plugin(
            callee,
            arg_kinds,
            arg_types,
            arg_names,
            formal_to_actual,
            args,
            callee.name,
            object_type,
            context,
        )
        callee = callee.copy_modified(ret_type=new_ret_type)
    elif callable_name and (
        (object_type is None and self.plugin.get_function_hook(callable_name))
        or (object_type is not None and self.plugin.get_method_hook(callable_name))
    ):
        new_ret_type = self.apply_function_plugin(
            callee,
            arg_kinds,
            arg_types,
            arg_names,
            formal_to_actual,
            args,
            callable_name,
            object_type,
            context,
        )
        callee = callee.copy_modified(ret_type=new_ret_type)
    return callee.ret_type, callee

# patched mypy 1.9.0 source
def apply_type_arguments_to_callable(
    self: ExpressionChecker, tp: Type, args: Sequence[Type], ctx: Context
) -> Type:
    """Apply type arguments to a generic callable type coming from a type object.

    This will first perform type arguments count checks, report the
    error as needed, and return the correct kind of Any. As a special
    case this returns Any for non-callable types, because if type object type
    is not callable, then an error should be already reported.
    """
    tp = get_proper_type(tp)

    if isinstance(tp, CallableType):
        min_arg_count = sum(not v.has_default() for v in tp.variables)
        has_type_var_tuple = any(isinstance(v, TypeVarTupleType) for v in tp.variables)
        if (
            len(args) < min_arg_count or len(args) > len(tp.variables)
        ) and not has_type_var_tuple:
            if tp.is_type_obj() and tp.type_object().fullname == "builtins.tuple":
                #pdb_trace()
                return CallableType(
                    [get_proper_type(t) for t in args],
                    [ARG_POS for _ in args],
                    [chr(ord('a')+i) for i in range(0,len(args))],
                    TupleType(list(args),self.chk.named_type('tuple')),
                    self.chk.named_type('abc.ABCMeta'),
                    name='tuple',
                    definition=tp.definition,
                    bound_args=tp.bound_args
                )
                return tp
            self.msg.incompatible_type_application(
                min_arg_count, len(tp.variables), len(args), ctx
            )
            return AnyType(TypeOfAny.from_error)
        return self.apply_generic_arguments(tp, self.split_for_callable(tp, args, ctx), ctx)
    if isinstance(tp, Overloaded):
        for it in tp.items:
            min_arg_count = sum(not v.has_default() for v in it.variables)
            has_type_var_tuple = any(isinstance(v, TypeVarTupleType) for v in it.variables)
            if (
                len(args) < min_arg_count or len(args) > len(it.variables)
            ) and not has_type_var_tuple:
                self.msg.incompatible_type_application(
                    min_arg_count, len(it.variables), len(args), ctx
                )
                return AnyType(TypeOfAny.from_error)
        return Overloaded(
            [
                self.apply_generic_arguments(it, self.split_for_callable(it, args, ctx), ctx)
                for it in tp.items
            ]
        )
    return AnyType(TypeOfAny.special_form)

