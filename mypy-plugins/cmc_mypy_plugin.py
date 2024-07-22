from typing import Callable, Union
import sys

from mypy.plugin import (
    Plugin, ClassDefContext, Expression,
    CheckerPluginInterface
)
from mypy.plugins.common import (
    add_method_to_class,
)
from mypy.nodes import (
    Argument,
    ArgKind,
    Var,
    Decorator,
    SymbolTableNode,
    MDEF,
    TypeVarExpr,
    TypeInfo,
)
from mypy.types import (
    TypeVarType,
    TypeVarId,
    AnyType,
    TypeOfAny,
    UnionType,
    TypeType,
    NoneType,
    Instance,
)
from mypy.typevars import fill_typevars
from types import TracebackType

DECORATOR_FQN="xju.cmc.cmclass"
ASYNC_DECORATOR_FQN="xju.cmc.async_cmclass"
SELF_TVAR_NAME="_CT"

def ensure_cm_functions(class_def_in_context: ClassDefContext) -> bool:
    info = class_def_in_context.cls.info
    api = class_def_in_context.api
    #import pdb; pdb.set_trace()

    if not '__enter__' in info.names:  # names is local-defined names, i.e. exlucidng base classes
        # for T.__enter__ to properly return Self it has to look like:
        # def T.__enter__(self: S) -> S where S is TypeVar('S', T)
        # ... I think, anyway stole this from combination of dataclasses plugin and emanal.py
        self_tvar_expr = TypeVarExpr(
            SELF_TVAR_NAME,
            info.fullname + "." + SELF_TVAR_NAME,
            [],
            fill_typevars(info),
            AnyType(TypeOfAny.from_omitted_generics))
        info.names[SELF_TVAR_NAME] = SymbolTableNode(MDEF, self_tvar_expr)
        self_tvar_def = TypeVarType(
            SELF_TVAR_NAME,
            info.fullname + "."+SELF_TVAR_NAME,
            id=TypeVarId(-1),
            values=[],
            upper_bound=fill_typevars(info),
            default=AnyType(TypeOfAny.from_omitted_generics),
        )
        add_method_to_class(
            class_def_in_context.api,
            class_def_in_context.cls,
            "__enter__",
            args=[],  # self is implicit
            return_type=self_tvar_def,
            self_type=self_tvar_def,
            tvar_def=self_tvar_def)
        pass
    if not '__exit__' in info.names:  # names is local-defined names, i.e. exlucidng base classes
        add_method_to_class(
            class_def_in_context.api,
            class_def_in_context.cls,
            "__exit__",
            args=[
                # self is implicit,
                Argument(
                    variable=Var('exc_type'),
                    type_annotation=UnionType.make_union(
                        [TypeType(api.named_type('builtins.BaseException')), NoneType()]),
                    initializer=None,
                    kind=ArgKind.ARG_POS),
                Argument(
                    variable=Var('exc_value'),
                    type_annotation=UnionType.make_union(
                        [api.named_type('builtins.BaseException'), NoneType()]),
                    initializer=None,
                    kind=ArgKind.ARG_POS),
                Argument(
                    variable=Var('traceback'),
                    type_annotation=UnionType.make_union(
                        [api.named_type('types.TracebackType'), NoneType()]),
                    initializer=None,
                    kind=ArgKind.ARG_POS),
            ],
            return_type=UnionType.make_union([api.named_type('builtins.bool'),NoneType()]))
    return True

def ensure_asynccm_functions(class_def_in_context: ClassDefContext) -> bool:
    info = class_def_in_context.cls.info
    api = class_def_in_context.api
    #import pdb; pdb.set_trace()

    if not '__aenter__' in info.names:  # names is local-defined names, i.e. exlucidng base classes
        # for T.__aenter__ to properly return Self it has to look like:
        # async def T.__aenter__(self: S) -> S where S is TypeVar('S', T)
        # ... I think, anyway stole this from combination of dataclasses plugin and emanal.py
        self_tvar_expr = TypeVarExpr(
            SELF_TVAR_NAME,
            info.fullname + "." + SELF_TVAR_NAME,
            [],
            fill_typevars(info),
            AnyType(TypeOfAny.from_omitted_generics))
        info.names[SELF_TVAR_NAME] = SymbolTableNode(MDEF, self_tvar_expr)
        self_tvar_def = TypeVarType(
            SELF_TVAR_NAME,
            info.fullname + "."+SELF_TVAR_NAME,
            id=TypeVarId(-1),
            values=[],
            upper_bound=fill_typevars(info),
            default=AnyType(TypeOfAny.from_omitted_generics),
        )
        coroutine=api.lookup_fully_qualified('typing.Coroutine')
        assert coroutine is not None
        assert coroutine.node is not None
        assert isinstance(coroutine.node, TypeInfo)
        add_method_to_class(
            class_def_in_context.api,
            class_def_in_context.cls,
            "__aenter__",
            args=[],  # self is implicit
            return_type=Instance(coroutine.node,
                                 [AnyType(TypeOfAny.from_omitted_generics),
                                  AnyType(TypeOfAny.from_omitted_generics),
                                  self_tvar_def]),
            self_type=self_tvar_def,
            tvar_def=self_tvar_def)
        pass
    if not '__aexit__' in info.names:  # names is local-defined names, i.e. exlucidng base classes
        coroutine=api.lookup_fully_qualified('typing.Coroutine')
        assert coroutine is not None
        assert coroutine.node is not None
        assert isinstance(coroutine.node, TypeInfo)
        add_method_to_class(
            class_def_in_context.api,
            class_def_in_context.cls,
            "__aexit__",
            args=[
                # self is implicit,
                Argument(
                    variable=Var('exc_type'),
                    type_annotation=UnionType.make_union(
                        [TypeType(api.named_type('builtins.BaseException')), NoneType()]),
                    initializer=None,
                    kind=ArgKind.ARG_POS),
                Argument(
                    variable=Var('exc_value'),
                    type_annotation=UnionType.make_union(
                        [api.named_type('builtins.BaseException'), NoneType()]),
                    initializer=None,
                    kind=ArgKind.ARG_POS),
                Argument(
                    variable=Var('traceback'),
                    type_annotation=UnionType.make_union(
                        [api.named_type('types.TracebackType'), NoneType()]),
                    initializer=None,
                    kind=ArgKind.ARG_POS),
            ],
            return_type=Instance(
                coroutine.node,
                [AnyType(TypeOfAny.from_omitted_generics),
                 AnyType(TypeOfAny.from_omitted_generics),
                 UnionType.make_union([api.named_type('builtins.bool'),NoneType()])]))
    return True

class CmcPlugin(Plugin):
    '''add @cmclass methods i.e. __enter__ and __exit__ to class def'''
    '''add @async_cmclass methods i.e. __aenter__ and __aexit__ to class def'''
    def get_class_decorator_hook_2(
        self, fullname: str
    ) -> Callable[[ClassDefContext], bool] | None:
        if fullname==DECORATOR_FQN:
            return ensure_cm_functions
        if fullname==ASYNC_DECORATOR_FQN:
            return ensure_asynccm_functions
        return None
    

def plugin(version: str):
    # ignore version argument if the plugin works with all mypy versions.
    return CmcPlugin
