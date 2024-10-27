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
from dataclasses import dataclass
from typing import Callable, NewType, Sequence, Iterator, Collection, Self
from mypy.checkstrformat import parse_format_value
from mypy.plugin import (
    Plugin, FunctionContext,
    CheckerPluginInterface,
    FunctionContext,
)
from mypy.types import (
    Type,
    AnyType,
    TypeOfAny,
    Instance,
)
from mypy.patterns import AsPattern,StarredPattern,OrPattern,SequencePattern,MappingPattern,SingletonPattern,ClassPattern,ValuePattern
from mypy.nodes import (
    DictExpr,MatchStmt,Block,SliceExpr,Import,ParamSpecExpr,OperatorAssignmentStmt,TypeVarExpr,TempNode,NameExpr,IfStmt,SuperExpr,TryStmt,BreakStmt,ImportFrom,IndexExpr,DelStmt,EnumCallExpr,OverloadedFuncDef,WithStmt,YieldFromExpr,TypeApplication,Var,GeneratorExpr,PlaceholderNode,DictionaryComprehension,LambdaExpr,AssignmentStmt,NewTypeExpr,ListExpr,AssignmentExpr,SetComprehension,ForStmt,ListComprehension,MemberExpr,IntExpr,AssertTypeExpr,TypeAlias,TypeAliasStmt,OpExpr,SetExpr,GlobalDecl,StarExpr,TupleExpr,Decorator,FloatExpr,NonlocalDecl,BytesExpr,WhileStmt,ComplexExpr,EllipsisExpr,CastExpr,ExpressionStmt,ComparisonExpr,TypeVarTupleExpr,NamedTupleExpr,FakeExpression,ImportAll,FakeInfo,CallExpr,StrExpr,TypeAliasExpr,FuncDef,YieldExpr,AssertStmt,ReturnStmt,MypyFile,UnaryExpr,TypedDictExpr,ConditionalExpr,ContinueStmt,RaiseStmt,AwaitExpr,ClassDef,Argument,RevealExpr,PromoteExpr,PassStmt,
    MypyFile, OverloadedFuncDef, FuncDef, Decorator, Var, TypeVarLikeExpr, TypeInfo, TypeAlias, PlaceholderNode,
    OverloadedFuncDef, FuncItem,
    NameExpr,
    ArgKind,
    MemberExpr,
    SymbolTableNode,
    Expression,
    Statement,
    Lvalue,
    Node,
    TempNode,
    Context,
)
from mypy.checker import TypeChecker
from mypy.checkexpr import ExpressionChecker

class DocStringError(Exception):
    pass
    
IN_FUNCTION_CONTEXT_FQN="xju.xn.in_function_context"

LeafNodeTypes = DictExpr | MatchStmt | AsPattern | Block | SliceExpr | Import | ParamSpecExpr | OperatorAssignmentStmt | StarredPattern | TypeVarExpr | TempNode | NameExpr | IfStmt | SuperExpr | TryStmt | BreakStmt | ImportFrom | IndexExpr | DelStmt | EnumCallExpr | OverloadedFuncDef | WithStmt | YieldFromExpr | TypeApplication | Var | GeneratorExpr | PlaceholderNode | DictionaryComprehension | LambdaExpr | AssignmentStmt | ValuePattern | ClassPattern | NewTypeExpr | ListExpr | AssignmentExpr | SetComprehension | ForStmt | ListComprehension | MemberExpr | IntExpr | AssertTypeExpr | TypeAlias | TypeAliasStmt | OpExpr | SetExpr | GlobalDecl | StarExpr | TupleExpr | Decorator | SingletonPattern | FloatExpr | NonlocalDecl | BytesExpr | WhileStmt | ComplexExpr | EllipsisExpr | CastExpr | ExpressionStmt | ComparisonExpr | TypeVarTupleExpr | NamedTupleExpr | FakeExpression | ImportAll | MappingPattern | FakeInfo | CallExpr | StrExpr | TypeAliasExpr | FuncDef | YieldExpr | AssertStmt | ReturnStmt | MypyFile | UnaryExpr | SequencePattern | TypedDictExpr | OrPattern | ConditionalExpr | ContinueStmt | RaiseStmt | AwaitExpr | ClassDef | Argument | RevealExpr | PromoteExpr | PassStmt


class XnPlugin(Plugin):
    def get_function_hook(self, fullname: str) -> Callable[[FunctionContext], Type] | None:
        if fullname==IN_FUNCTION_CONTEXT_FQN:
            return check_in_function_context
        return None
    
def plugin(version: str):
    # ignore version argument if the plugin works with all mypy versions.
    return XnPlugin

VarName=NewType("VarName", str)
VarNode=Var|TypeAlias|ClassDef

def check_in_function_context(x: FunctionContext) -> Type:
    """
    assuming x is a call to xju.xn.in_function_context(fn, vars()), check
    fn.__doc__.format(**vars()) is valid
    """
    try:
        checker_api = x.api
        assert isinstance(checker_api, TypeChecker), (type(checker_api), checker_api)
        expr_checker = checker_api.expr_checker

        # x.context should be the in_function_context() call...
        expr = x.context
        assert isinstance(expr, CallExpr), (type(expr), expr)
        assert isinstance(expr.callee, (NameExpr,MemberExpr)), (type(expr.callee), expr.callee)
        assert expr.callee.fullname == 'xju.xn.in_function_context', expr.callee.name
        # ... seems like it is.

        # check params... in particular expect second param to be vars()
        if len(x.args) != 4:
            raise DocStringError(f"in_function_context expects two, three or four parameters not {len(x.args)}")
        if not (
            isinstance(x.args[1][0], CallExpr) and
            isinstance(x.args[1][0].callee, NameExpr) and
            x.args[1][0].callee.name == 'vars' and
            x.arg_kinds[1][0] == ArgKind.ARG_POS
        ):
            raise DocStringError(f"second parameter to in_function_context should be vars()")

        # xn.in_function_context uses first paragraph of doc string, and strips it
        format_string = get_function_doc_string_value(x.args[0][0]).strip().split('\n\n')[0].strip()

        if (conversion_specifiers:=parse_format_value(
                format_string, x.context, checker_api.msg
        )) is not None:
            # We want to fabricate a type-checkable call to str.format... type checking does not
            # treat vars() specially, so we have to do that ourselves i.e. find all the valid vars
            # and fabricate a .format() call that uses positional args rather than named args
            # vars() has function params as well as local variables, so our scope for finding
            # valid vars is the function we're in; in theory we should be able to do that
            # like the way mypy checks f"{x} {y}", which is that during semantic analysis, that
            # is expanded as if it was a "macro" into "{0} {1}".format(x, y) by fabricating
            # the relevant nodes to replace the usual CallExpr node - but mypy has no appropriate
            # semantic analysis hooks.
            # One good thing about or approach is that we can do var initialisation analysis
            # perhaps beyond what mypy does in general, though it is not smart enough
            # to recognise that e.g.:
            # if a:
            #    raise Exception
            # elif c:
            #    j=1
            # else:
            #    j=2
            # print(j)
            # ... always initialises j
            func_items=[x for x in checker_api.scope.stack
                        if isinstance(x, (FuncDef, OverloadedFuncDef, TypeInfo))]

            func_item=func_items[-1]
            if not isinstance(func_item, FuncDef):
                raise DocStringError(f"xn.in_function_context() should only be called from function scope")
            valid_vars_at_expr=get_valid_vars_at(
                expr,
                func_item,
                func_items[:-1]
            )

            #pdb_trace()
            args=list[Expression]()
            arg_indices=dict[VarName,int]()
            arg_type_map=dict[NameExpr,Type]()
            not_defined=list[str]()
            errors=list[str]()
            for conversion_specifier in conversion_specifiers:
                if not isinstance(conversion_specifier.key, str):
                    raise DocStringError(f"invalid format item key {conversion_specifier.key!r} (all format specifiers must name local variables)")
                var_name=VarName(conversion_specifier.key)
                if (arg_index:=arg_indices.get(var_name, None)) is None:
                    if (v:=valid_vars_at_expr.get(VarName(conversion_specifier.key))) is None:
                        errors.append(repr(conversion_specifier.key))
                        continue
                    arg_index=len(args)
                    vref=NameExpr(str(var_name))
                    vref.node=v.info if isinstance(v, ClassDef) else v
                    args.append(vref)
                    arg_indices[var_name]=arg_index
                    vref_type:Type|None=None
                    match v:
                        case Var():
                            vref_type=v.type or AnyType(TypeOfAny.unannotated)
                        case TypeAlias():
                            vref_type=v.target
                        case ClassDef():
                            vref_type=Instance(v.info,[])
                    arg_type_map[vref]=vref_type
                    pass
                # field is like 'C' or 'C.x' or 'C[1]' - turn into '1.x' etc
                assert conversion_specifier.field is not None
                assert conversion_specifier.field.startswith(str(var_name))
                conversion_specifier.field = (
                    str(arg_index)+
                    conversion_specifier.field[len(str(var_name)):]
                )
                conversion_specifier.key=str(arg_index)
                pass
            match len(errors):
                case 0:
                    pass
                case 1:
                    raise DocStringError(f"{errors[0]} is not a valid (i.e. initialised) local variable or a function parameter (i.e. not one of {list(valid_vars_at_expr.keys())}")
                case _:
                    
                    raise DocStringError(f"{', '.join(sorted(errors))} are not valid (i.e. initialised) local variables/function parameters (i.e. each is not one of {sorted(list(valid_vars_at_expr.keys()))}")

            format_callee=MemberExpr(StrExpr(format_string), 'format')
            e = CallExpr(format_callee,
                         args,
                         [ArgKind.ARG_POS for _ in args],
                         [None for _ in args],
                         None)
            e.set_line(expr)
            # note local_type_map() cannot handle exceptions
            with checker_api.local_type_map() as type_map:
                for arg, arg_type in arg_type_map.items():
                    type_map[arg]=arg_type
                
                expr_checker.strfrm_checker.check_specs_in_format_call(
                    e, conversion_specifiers, format_string
                )
                pass
            pass
        return x.default_return_type
    except DocStringError as e:
        x.api.fail(str(e), x.context)
        return x.default_return_type
    pass

def get_function_doc_string_value(expr: Expression) -> str:
    match expr:
        case NameExpr():
            f=expr.node
            if f is None:
                raise DocStringError(f"{expr.name} not found")
            function_name=expr.name
        case MemberExpr():
            function_name=expr.fullname
            #pdb_trace()
            if not isinstance(expr.expr, NameExpr):
                raise DocStringError(f"{expr} is not an xn-supported function reference")
            a:NameExpr=expr.expr
            if a.node is None:
                raise DocStringError(f"{expr} is not an xn-supported function reference (expected TypeInfo not None?)")
            if not isinstance(a.node, TypeInfo):
                raise DocStringError(f"{expr} is not an xn-supported function reference (expected TypeInfo not {type(a.node)}")
            b:SymbolTableNode=a.node.names[expr.name]
            if b.node is None:
                raise DocStringError(f"{expr.fullname} not found")
            f=b.node
        case _:
            raise DocStringError(f"{expr} is not an xn-supported function reference (i.e. not a MemberExpr)")
    if not isinstance(f, FuncDef):
        raise DocStringError(f"{f} is not a function definition")
    if (
        len(f.body.body) > 0 and
        isinstance(f.body.body[0], ExpressionStmt) and
        isinstance(f.body.body[0].expr, StrExpr)
    ):
        return f.body.body[0].expr.value
    raise DocStringError(f"{function_name} has no docstring")

@dataclass
class VarsAtExpr:
    vars_at_expr: set[VarNode]

@dataclass
class VarsSoFar:
    vars_so_far: set[VarNode]

    def union(self, *others: Self | set[VarNode]) -> "VarsSoFar":
        return VarsSoFar(
            self.vars_so_far.union(
                *[v.vars_so_far if isinstance(v, VarsSoFar) else v for v in others]))
    def __or__(self, *others: Self | set[VarNode]) -> "VarsSoFar":
        return self.union(*others)
    
    def intersection(self, *others: Self | set[VarNode]) -> "VarsSoFar":
        return VarsSoFar(
            self.vars_so_far.intersection(
                *[v.vars_so_far if isinstance(v, VarsSoFar) else v for v in others]))
    def __and__(self, *others: Self | set[VarNode]) -> "VarsSoFar":
        return self.intersection(*others)
    pass

@dataclass
class UnconditionalRaise:
    at: Context

def get_valid_vars_at(expr: Expression, within: FuncDef, scope: Sequence[FuncDef|TypeInfo]) -> dict[VarName, VarNode]:
    """
    get valid variables at expression {expr} within function {within}, which is in scope {scope}

    - assumes expr is an expression within specified FuncDef
    - like vars(), includes function parameters and locals, but not globals
    - for local variables, unlike mypy checking, requires that the variable
      be unconditionally assigned for it to be considered valid, e.g.:
        a=1
        if n:
          b=2
        in_function_context('{a} {b}', vars())
      ... a is valid, b is not valid because n might be False i.e. b is conditionally
          initialised
    """
    match collect_vars_defined_for_expr_from_statements(
        expr,
        iter(within.body.body),
        VarsSoFar(set())
    ):
        case VarsAtExpr() as result:
            vars={VarName(var.name):var for var in list(result.vars_at_expr) +
                    [a.variable for a in within.arguments]}
            target:Node=within
            for defn in reversed(scope):
                match defn:
                    case TypeInfo() as type_info:
                        if (varname:=VarName(type_info.defn.name)) not in vars:
                            vars[varname]=type_info.defn
                            pass
                        target=type_info.defn
                    case FuncDef() as func_def:
                        match collect_vars_defined_for_expr_from_statements(
                            target,
                            iter(func_def.body.body),
                            VarsSoFar(set())
                        ):
                            case VarsAtExpr() as result:
                                for var in list(result.vars_at_expr) + [
                                        a.variable for a in func_def.arguments]:
                                    if (varname:=VarName(var.name)) not in vars:
                                        vars[varname]=var
                                        pass
                                    pass
                                pass
                            case UnconditionalRaise() as u:
                                raise DocStringError(f"line {target.line} apparently unreachable due to unconditional raise at line {u.at.line}")
                            case VarsSoFar():
                                # expr not found? should not get to here
                                raise DocStringError(f"xn_mypy_plugin internal error, did not find line {target.line} in function/class at line {defn.line}")
                        target=defn
                        pass
            return vars
        case UnconditionalRaise() as u:
            raise DocStringError(f"in_function_context call at {expr.line} apparently unreachable due to unconditional raise at line {u.at.line}")
        case VarsSoFar() as vars_so_far:
            # expr not found? should not get to here
            raise DocStringError(f"xn_mypy_plugin internal error, did not find in_function_context call at {expr.line} in function at line {within.line}")

def collect_vars_defined_for_expr(
        expr:Node,
        collect_from:Node,
        vars_so_far:VarsSoFar
) -> VarsAtExpr | VarsSoFar | UnconditionalRaise:
    if expr is collect_from:
        return VarsAtExpr(vars_so_far.vars_so_far)

    assert isinstance(collect_from, LeafNodeTypes)
    
    match collect_from:
        case DictExpr() | SliceExpr() | Import() | ParamSpecExpr() | ImportFrom() | IndexExpr() | DelStmt() | EnumCallExpr() | OverloadedFuncDef() | StarredPattern() | TypeVarExpr() | TempNode() | NameExpr() | YieldFromExpr() | TypeApplication() | PlaceholderNode() | DictionaryComprehension() | LambdaExpr() | ValuePattern() | ClassPattern() | NewTypeExpr() | ListExpr() | SetComprehension() | GeneratorExpr() | MemberExpr() | IntExpr() | AssertTypeExpr() | SetExpr() | GlobalDecl() | StarExpr() | TupleExpr() | Decorator() | SingletonPattern() | FloatExpr() | BytesExpr() | ComplexExpr() | EllipsisExpr() | CastExpr() | ComparisonExpr() | TypeVarTupleExpr() | NamedTupleExpr() | FakeExpression() | ImportAll() | MappingPattern() | FakeInfo() | CallExpr() | StrExpr() | TypeAliasExpr() | FuncDef() | YieldExpr() | AssertStmt() | MypyFile() | UnaryExpr() | SequencePattern() | TypedDictExpr() | ContinueStmt() | AwaitExpr() | Argument() | RevealExpr() | PromoteExpr() | PassStmt() | OperatorAssignmentStmt() | SuperExpr() |  BreakStmt() | TypeAlias() | NonlocalDecl():
            return vars_so_far
        case Block():
            # there's no sensible handling for block, we need to handle it in context e.g.
            # as part of an if statement or a try-except
            assert False, collect_from
        case IfStmt():
            # REVISIT: why is collect_from.expr a list?
            assert len(collect_from.expr)==1, collect_from.expr
            match collect_vars_defined_for_expr(expr, collect_from.expr[0], vars_so_far):
                case VarsAtExpr() as result:
                    return result
                case UnconditionalRaise() as result:
                    return result
                case VarsSoFar() as vars_so_far:
                    match collect_vars_defined_for_expr_from_statements(
                        expr, iter(sum([block.body for block in collect_from.body], [])), vars_so_far
                    ):
                        case VarsAtExpr() as result:
                            return result
                        case UnconditionalRaise():
                            match collect_vars_defined_for_expr_from_statements(
                                expr,
                                iter(collect_from.else_body.body) if collect_from.else_body else iter([]),
                                vars_so_far
                            ):
                                case VarsAtExpr() as result:
                                    return result
                                case UnconditionalRaise() as result:
                                    return result
                                case VarsSoFar() as false_vars:
                                    # true path raised unconditionally so ignore it
                                    return false_vars
                        case VarsSoFar() as true_vars:
                            match collect_vars_defined_for_expr_from_statements(
                                expr,
                                iter(collect_from.else_body.body) if collect_from.else_body else iter([]),
                                vars_so_far
                            ):
                                case VarsAtExpr() as result:
                                    return result
                                case UnconditionalRaise():
                                    # false path raised unconditionally so ignore it
                                    return true_vars
                                case VarsSoFar() as false_vars:
                                    return true_vars & false_vars
        case TryStmt():
            for handler in collect_from.handlers:
                # we ignore except vars, they should not appear in function doc string
                match collect_vars_defined_for_expr_from_statements(expr, iter(handler.body), vars_so_far):
                    case VarsAtExpr() as result:
                        return result
                    case UnconditionalRaise() as result:
                        pass
                    case VarsSoFar() as handler_vars:
                        # note only finally is gauranteed to run, so ignore vars assigned in handlers
                        pass
            if collect_from.else_body is not None:
                match collect_vars_defined_for_expr_from_statements(
                    expr, iter(collect_from.else_body.body), vars_so_far
                ):
                    case VarsAtExpr() as result:
                        return result
                    case UnconditionalRaise() as result:
                        pass
                    case VarsSoFar() as handler_vars:
                        # note only finally is gauranteed to run, so ignore vars assigned in else body
                        pass
            match collect_vars_defined_for_expr_from_statements(expr, iter(collect_from.body.body), vars_so_far):
                case VarsAtExpr() as result:
                    return result
                case VarsSoFar() | UnconditionalRaise():
                    # Assume any statement in body could raise, in which case no vars would be
                    # initialised. (In reality some python statements never raise exceptions but
                    # not feasible in this plugin to figure out which.)
                    pass
            if collect_from.finally_body is not None:
                match collect_vars_defined_for_expr_from_statements(
                    expr, iter(collect_from.finally_body.body), vars_so_far
                ):
                    case VarsAtExpr() as result:
                        return result
                    case UnconditionalRaise() as result:
                        return result
                    case VarsSoFar() as vars_so_far:
                        return vars_so_far
            else:
                return vars_so_far

        case WithStmt():
            for with_expr in collect_from.expr:
                match collect_vars_defined_for_expr(expr, with_expr, vars_so_far):
                    case VarsAtExpr() as result:
                        return result
                    case UnconditionalRaise():
                        pass
                    case VarsSoFar():
                        # note can't assign to a variable as part of expression
                        pass
            return collect_vars_defined_for_expr_from_statements(
                expr,
                iter(collect_from.body.body),
                vars_so_far.union(
                    # as_vars (which are still valid after with block so we include them)
                    *[collect_lvalue_vars(lvalue) for lvalue in collect_from.target
                      if lvalue is not None]))
                    
        case Var():
            return vars_so_far | VarsSoFar(set([collect_from]))

        case AssignmentStmt():
            match collect_vars_defined_for_expr(expr, collect_from.rvalue, vars_so_far):
                case VarsAtExpr() as result:
                    return result
                case UnconditionalRaise() as result:
                    return result
                case VarsSoFar():
                    # note can't assign to a variable as part of expression
                    pass
            return vars_so_far.union(*[collect_lvalue_vars(lvalue) for lvalue in collect_from.lvalues
                                       if lvalue is not None])

        case AssignmentExpr():
            match collect_vars_defined_for_expr(expr, collect_from.value, vars_so_far):
                case VarsAtExpr() as result:
                    return result
                case UnconditionalRaise() as result:
                    return result
                case VarsSoFar():
                    # note can't assign to a variable as part of expression
                    pass
            return vars_so_far | collect_lvalue_vars(collect_from.target)

        case ForStmt():
            match collect_vars_defined_for_expr(expr, collect_from.expr, vars_so_far):
                case VarsAtExpr() as result:
                    return result
                case UnconditionalRaise() as result:
                    return result
                case VarsSoFar() as vars_so_far:
                    match collect_vars_defined_for_expr_from_statements(
                        expr, iter(collect_from.body.body), vars_so_far | collect_lvalue_vars(collect_from.index)
                    ):
                        case VarsAtExpr() as result:
                            return result
                        case UnconditionalRaise():
                            pass
                        case VarsSoFar():
                            pass
            if collect_from.else_body is not None:
                match collect_vars_defined_for_expr_from_statements(
                    expr, iter(collect_from.else_body.body), vars_so_far
                ):
                    case VarsAtExpr() as result:
                        return result
                    case UnconditionalRaise():
                        pass
                    case VarsSoFar():
                        pass
            # no gaurentee that body will be executed (e.g. for x in empty list); same goes
            # for else_body (e.g. we break) and loop var initilisation (e.g. for x in empty
            # list), so throw away all their vars
            return vars_so_far

        case WhileStmt():
            match collect_vars_defined_for_expr(expr, collect_from.expr, vars_so_far):
                case VarsAtExpr() as result:
                    return result
                case UnconditionalRaise() as result:
                    return result
                case VarsSoFar() as vars_so_far:
                    match collect_vars_defined_for_expr_from_statements(
                        expr, iter(collect_from.body.body), vars_so_far
                    ):
                        case VarsAtExpr() as result:
                            return result
                        case UnconditionalRaise():
                            # no gaurentee else will be executed so discard
                            pass
                        case VarsSoFar():
                            # no gaurentee that body will be executed (e.g. while False) so discard vars
                            pass
            if collect_from.else_body is not None:
                match collect_vars_defined_for_expr_from_statements(
                    expr, iter(collect_from.else_body.body), vars_so_far
                ):
                    case VarsAtExpr() as result:
                        return result
                    case UnconditionalRaise():
                        # no gaurentee else will be executed so discard
                        pass
                    case VarsSoFar():
                        # no gaurentee else will be executed so discard vars
                        pass
            return vars_so_far

        case ListComprehension():
            return collect_vars_defined_for_expr(expr, collect_from.generator, vars_so_far)

        case TypeAliasStmt():
            return collect_lvalue_vars(collect_from.name)

        case OpExpr():
            match collect_vars_defined_for_expr(expr, collect_from.left, vars_so_far):
                case VarsAtExpr() as result:
                    return result
                case UnconditionalRaise() as result:
                    return result
                case VarsSoFar() as left_vars:
                    # expr might be in rhs so ignore collect_from.right_unreachable
                    match collect_vars_defined_for_expr(expr, collect_from.right, left_vars):
                        case VarsAtExpr() as result:
                            return result
                        case UnconditionalRaise() as result:
                            if collect_from.right_always:
                                return result
                            else:
                                # ignore right branch as cases where we don't hit it
                                return left_vars
                        case VarsSoFar() as right_vars:
                            if collect_from.right_always:
                                return left_vars | right_vars
                            return left_vars

        case ExpressionStmt():
            return collect_vars_defined_for_expr(expr, collect_from.expr, vars_so_far)

        case OrPattern():
            some_raise: None | UnconditionalRaise = None
            for pattern in collect_from.patterns:
                match collect_vars_defined_for_expr(expr, pattern, vars_so_far):
                    case VarsAtExpr() as result:
                        return result
                    case UnconditionalRaise() as some_raise:
                        pass
                    case VarsSoFar() as vars_so_far:
                        # note all terms of a | b... are always evaluated
                        pass
            return some_raise or vars_so_far

        case AsPattern():
            if collect_from.pattern is not None:
                match collect_vars_defined_for_expr(expr, collect_from.pattern, vars_so_far):
                    case VarsAtExpr() as result:
                        return result
                    case UnconditionalRaise() as some_raise:
                        return some_raise
                    case VarsSoFar() as vars_so_far:
                        pass
                pass
            if collect_from.name is not None and isinstance(collect_from.name.node, (Var, TypeAlias)):
                return vars_so_far | set([collect_from.name.node])
            return vars_so_far

        case ConditionalExpr():
            assert isinstance(collect_from.if_expr, LeafNodeTypes)
            match collect_vars_defined_for_expr(expr, collect_from.if_expr, vars_so_far):
                case VarsAtExpr() as result:
                    return result
                case UnconditionalRaise() as some_raise:
                    return some_raise
                case VarsSoFar() as vars_so_far:
                    match collect_vars_defined_for_expr(expr, collect_from.cond, VarsSoFar(set[VarNode]())):
                        case VarsAtExpr() as result:
                            return result
                        case UnconditionalRaise():
                            pass
                        case VarsSoFar() as cond_vars:
                            pass
                    match collect_vars_defined_for_expr(expr, collect_from.else_expr, VarsSoFar(set[VarNode]())):
                        case VarsAtExpr() as result:
                            return result
                        case UnconditionalRaise():
                            pass
                        case VarsSoFar() as else_vars:
                            pass
                    return vars_so_far | (cond_vars & else_vars)
            pass
        case RaiseStmt():
            if collect_from.expr is not None:
                match collect_vars_defined_for_expr(expr, collect_from.expr, vars_so_far):
                    case VarsAtExpr() as result:
                        return result
                    case UnconditionalRaise() as some_raise:
                        return some_raise
                    case VarsSoFar() as vars_so_far:
                        pass
                pass
            return UnconditionalRaise(collect_from)

        case MatchStmt():
            match collect_vars_defined_for_expr(expr, collect_from.subject, vars_so_far):
                case VarsAtExpr() as result:
                    return result
                case UnconditionalRaise() as some_raise:
                    return some_raise
                case VarsSoFar() as vars_so_far:
                    pass
            for guard, body in zip(collect_from.guards, collect_from.bodies):
                guard_vars = vars_so_far
                if guard is not None:
                    match collect_vars_defined_for_expr(expr, guard, vars_so_far):
                        case VarsAtExpr() as result:
                            return result
                        case UnconditionalRaise():
                            pass
                        case VarsSoFar() as guard_vars:
                            pass
                match collect_vars_defined_for_expr(expr, body, guard_vars):
                    case VarsAtExpr() as result:
                        return result
                    case UnconditionalRaise():
                        pass
                    case VarsSoFar():
                        pass
                pass
            return vars_so_far

        case ClassDef():
            return vars_so_far | set[VarNode]([collect_from])

        case ReturnStmt():
            return (
                collect_vars_defined_for_expr(expr, collect_from.expr, vars_so_far)
                if collect_from.expr else VarsSoFar(set())
            )
            
    reveal_type(collect_from)

def collect_vars_defined_for_expr_from_statements(
        target: Node,
        collect_from:Iterator[Statement],
        vars_so_far:VarsSoFar
) -> VarsAtExpr | UnconditionalRaise | VarsSoFar:
    while (statement:=next(collect_from, None)) is not None:
        assert isinstance(statement, LeafNodeTypes)
        match collect_vars_defined_for_expr(target, statement, vars_so_far):
            case VarsAtExpr() as result:
                return result
            case UnconditionalRaise() as some_raise:
                return some_raise
            case VarsSoFar() as vars_so_far:
                pass
        pass
    return vars_so_far

def collect_lvalue_vars(lvalue: Lvalue) -> VarsSoFar:
    match lvalue:
        case NameExpr() if isinstance(lvalue.node, (Var, TypeAlias)):
            return VarsSoFar(set([lvalue.node]))
        case TupleExpr():
            return VarsSoFar.union(*[collect_lvalue_vars(item) for item in lvalue.items
                                     if isinstance(item, Lvalue)])
    return VarsSoFar(set())

def pdb_trace() -> None:
    import pdb; pdb.set_trace()
    pass
