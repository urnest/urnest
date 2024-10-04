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
from typing import Callable
from mypy.plugin import (
    Plugin, FunctionContext,
    CheckerPluginInterface,
    FunctionContext,
)
from mypy.types import (
    Type,
)
from mypy.nodes import (
    CallExpr,
    NameExpr,
    MemberExpr,
    ArgKind,
    StrExpr,
    Expression,
    ExpressionStmt,
    SymbolNode,
    SymbolTableNode,
    FuncDef,
    TypeInfo,
)
from mypy.checker import TypeChecker
from mypy.checkexpr import ExpressionChecker
from mypy.fastparse import ast3_parse, visit_FormattedValue

class DocStringError(Exception):
    pass
    
IN_FUNCTION_CONTEXT_FQN="xju.xn.in_function_context"

class XnPlugin(Plugin):
    def get_function_hook(self, fullname: str) -> Callable[[FunctionContext], Type] | None:
        if fullname==IN_FUNCTION_CONTEXT_FQN:
            return check_in_function_context
        return None
    
def plugin(version: str):
    # ignore version argument if the plugin works with all mypy versions.
    return XnPlugin

def check_in_function_context(x: FunctionContext) -> Type:
    """
    assuming x is a call to xju.xn.in_function_context(fn, vars()), check
    fn.__doc__.format(**vars()) is valid
    """
    try:
        checker_api = x.api
        assert isinstance(checker_api, TypeChecker), (type(checker_api), checker_api)
        expr_checker = checker_api.expr_checker
        if len(x.args) != 4:
            raise DocStringError(f"in_function_context expects two, three or four parameters not {len(x.args)}")
        if not (
            isinstance(x.args[1][0], CallExpr) and
            isinstance(x.args[1][0].callee, NameExpr) and
            x.args[1][0].callee.name == 'vars' and
            x.arg_kinds[1][0] == ArgKind.ARG_POS
        ):
            raise DocStringError(f"second parameter to in_function_context should be vars()")

        vars_call_expr: CallExpr = x.args[1][0]

        format_value = get_function_doc_string_value(x.args[0][0]).strip().split('\n\n')[0].strip()
        
        format_callee=MemberExpr(StrExpr(format_value), 'format')
        
        e = CallExpr(format_callee,
                     [vars_call_expr],
                     [ArgKind.ARG_STAR2],
                     [None],
                     None)
        
        expr_checker.strfrm_checker.check_str_format_call(e, format_value)
        return x.default_return_type
    except DocStringError as e:
        x.api.fail(str(e), x.context)
        return x.default_return_type
    pass

def get_function_doc_string_value(expr: Expression) -> str:
    match expr:
        case NameExpr():
            f=expr.node
            function_name=expr.name
        case MemberExpr():
            function_name=expr.fullname
            pdb_trace()
            if not isinstance(expr.expr, NameExpr):
                raise DocStringError(f"{expr} is not an xn-supported function reference")
            a:NameExpr=expr.expr
            if a.node is None:
                raise DocStringError(f"{expr} is not an xn-supported function reference (expected TypeInfo not None?)")
            if not isinstance(a.node, TypeInfo):
                raise DocStringError(f"{expr} is not an xn-supported function reference (expected TypeInfo not {type(a.node)}")
            b:SymbolTableNode=a.node.names[expr.name]
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
        return expr.fullname
    raise DocStringError(f"{function_name} has no docstring")

def pdb_trace() -> None:
    import pdb; pdb.set_trace()
    pass
