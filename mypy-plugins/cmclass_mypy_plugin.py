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
)
from mypy.types import (
    TypeVarType,
    AnyType,
    TypeOfAny,
    UnionType,
    TypeType,
    NoneType,
)
from types import TracebackType

DECORATOR_FQN="xju.cmc.cmclass"

def ensure_cm_functions(class_def_in_context: ClassDefContext) -> bool:
    info = class_def_in_context.cls.info
    api = class_def_in_context.api
    import pdb; pdb.set_trace()

    if not '__enter__' in info.names:  # names is local-defined names, i.e. exlucidng base classes
        #class_def_in_context.api.setup_self_type() # fills in info.self_type
        if not info.self_type: return False
        add_method_to_class(
            class_def_in_context.api,
            class_def_in_context.cls,
            "__enter__",
            args=[],  # self is implicit
            return_type=info.self_type)
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
                        [TypeType(api.named_type_or_none('builtins.BaseException')), NoneType()]),
                    initializer=None,
                    kind=ArgKind.ARG_POS),
                Argument(
                    variable=Var('exc_value'),
                    type_annotation=UnionType.make_union(
                        [api.named_type_or_none('builtins.BaseException'), NoneType()]),
                    initializer=None,
                    kind=ArgKind.ARG_POS),
                Argument(
                    variable=Var('traceback'),
                    type_annotation=UnionType.make_union(
                        [api.named_type_or_none('types.TracebackType'), NoneType()]),
                    initializer=None,
                    kind=ArgKind.ARG_POS),
            ],
            return_type=UnionType.make_union([api.named_type_or_none('builtins.bool'),NoneType()]))
    return True

class CmClassPlugin(Plugin):
    '''add @cmclass methods i.e. __enter__ and __exit__ to class def'''
    def get_class_decorator_hook_2(
        self, fullname: str
    ) -> Callable[[ClassDefContext], bool] | None:
        if fullname==DECORATOR_FQN:
            return ensure_cm_functions
        return None
    

def plugin(version: str):
    # ignore version argument if the plugin works with all mypy versions.
    return CmClassPlugin
