# Copyright (c) 2022 Trevor Taylor
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
# CMC - Context Management, I don't know what the extra C stands for
#
from __future__ import annotations

import asyncio
import sys
import contextlib
from dataclasses import dataclass
from pathlib import Path
from typing import TypeVar, Iterable, Dict as _Dict, overload, Tuple, Sequence, Union, Optional
from typing import ItemsView, KeysView, IO, Literal
from typing import _GenericAlias # type: ignore  # mypy 1.0.0 :-(
from types import GenericAlias
from typing import Mapping, Type, List, Generic, Any, Callable, Awaitable, Self
from collections import OrderedDict
from collections.abc import Coroutine
from subprocess import Popen, STDOUT, DEVNULL, PIPE
from signal import SIGKILL
import builtins
import threading
from xju.time import Duration
from xju.xn import in_function_context, in_context
from xju.assert_ import Assert

T = TypeVar('T')
U = TypeVar('U')

T_CMT = TypeVar('T_CMT', bound=Type[contextlib.AbstractContextManager])

def cmclass(cls:T_CMT) -> T_CMT:
    '''Class decorator that adds context management __enter__ and __exit__
       that enter all type-hinted attributes implementing contextlib.AbstractContextManager
       then any existing __enter__. The return value is the return value of the existing
       __enter__, or self if there is no such function.  Exits are done in the reverse
       order, eventually returning the value of the existing __exit__, or none if there
       is no such function. Note an exception during the "entry" sequence will exit any
       complete steps. See cmclass.py.test for examples.
       To satisfy mypy use the xju.cmclass_mypy_plugin mypy plugin.
       See also async_cmclass, which provides an asyncio equivalent.
    '''
    wont_happen = [ base_class.__name__ for base_class in cls.__bases__
                    if issubclass(base_class, contextlib.AbstractAsyncContextManager) ]
    assert wont_happen == [], f"the following base classes of {cls} are async context managers - they cannot be managed via @cmclass; use @async_cmclass to decorate {cls} instead: {wont_happen}"
    base_classes_to_enter = [ base_class for base_class in cls.__bases__
                              if (
                                      base_class is not CM and
                                      issubclass(base_class, contextlib.AbstractContextManager)
                              ) ]
    wont_happen = [ n for n, t in cls.__annotations__.items()
                    if _is_subclass(n, t, contextlib.AbstractAsyncContextManager)]
    assert wont_happen == [], f"the following attributes of {cls} are async context managers - they cannot be managed via @cmclass; use @async_cmclass to decorate {cls} instead: {wont_happen}"
    attrs_to_enter = [ n for n, t in cls.__annotations__.items()
                         if _is_subclass(n, t, contextlib.AbstractContextManager)]
    # need a unique place to keep resources acquired by enter so exit can
    # exit them, only want resources for self, not subclasses (which do their own
    # handling). Note replacing . with _ could lead to clashes but there's no
    # way to avoid all clashes.
    resources_attr_name=f'{cls.__module__}.{cls.__name__}'.replace('.','_')
    orig_enter=cls.__dict__.get('__enter__',lambda self: self)
    def enter(self,
              base_classes_to_enter=base_classes_to_enter,
              attrs_to_enter=attrs_to_enter,
              resources_attr_name=resources_attr_name,
              orig_enter=orig_enter) -> Type[contextlib.AbstractContextManager[T]]:
        with contextlib.ExitStack() as tentative:
            for base_class in base_classes_to_enter:
                cm = _ClassCm(base_class, self)
                tentative.enter_context(cm)
                pass
            for n in attrs_to_enter:
                tentative.enter_context(getattr(self, n))
                pass
            assert getattr(self,resources_attr_name, None) is None, f'{cls.__module__}.{cls.__name__} has a f{resources_attr_name} attribute already. Perhaps it is inherited multiple times (which is not allowed by xju.cmc.cmclass decorator)?'
            result = orig_enter(self)
            setattr(self,resources_attr_name,tentative.pop_all())
        return result
    orig_exit=cls.__dict__.get('__exit__',lambda self,t,e,b: None)
    def exit(self,t,e,b,resources_attr_name=resources_attr_name):
        resources = getattr(self,resources_attr_name)
        setattr(self,resources_attr_name,None)
        with contextlib.closing(resources):
            return orig_exit(self,t,e,b)
    old_sa=cls.__setattr__
    def sa(self, name, value, attrs_to_enter=attrs_to_enter):
        if getattr(self,resources_attr_name,None) and name in attrs_to_enter:
            assert False, f'xju.cmc {self} has been entered so cannot replace context-managed attribute {cls.__module__}.{cls.__name__}.{name}'
        return old_sa(self,name,value)
    setattr(cls, '__enter__', enter)
    setattr(cls, '__exit__', exit)
    setattr(cls, '__setattr__', sa)
    cls.__annotations__['__enter__'] = type(enter)
    cls.__annotations__['__exit__'] = type(exit)
    return cls

class CM(contextlib.AbstractContextManager):
    def __enter__(self):
        assert False, f'{CM.__module__}.CM.__enter__ not overridden, have you forgotten @cmclass on {self.__module__}.{self.__class__.__name__}?'
        return self
    def __exit__(self, t, e, b) -> bool | None:
        assert False, f'{CM.__module__}.CM.__exit__ not overridden, have you forgotten @cmclass on {self.__module__}.{self.__class__.__name__}?'
        pass

K = TypeVar('K')
V = TypeVar('V', bound=contextlib.AbstractContextManager)

class Dict(Mapping[K, V], contextlib.AbstractContextManager):
    '''dictionary that enters and exits its values, all of which must be context managers
       note that values may be added and removed before and/or after self is entered:
         before: the value will be entered when self is entered
         after:  the value will be entered before it is added (noting that any existing
                 value will be exited once (if) it is removed) - the enter-new-exit-old 
                 order might be important to you i.e. might not be what you need!'''
    entered = False
    x:_Dict[K,V]

    @overload
    def __init__(self):
        '''new empty Dict'''

    @overload
    def __init__(self, x:_Dict[K,V]):
        '''initialise with value from {x} assuming those values have not been "entered"'''

    @overload
    def __init__(self, x:Iterable[Tuple[K,V]]):
        '''initialise with value from {x} assuming those values have not been "entered"'''

    def __init__(self, *args, **kwargs):
        self.x = OrderedDict(*args, **kwargs)
        pass
    
    def __enter__(self):
        '''"enters" all values in order they were inserted'''
        with contextlib.ExitStack() as tentative:
            for k,v in self.x.items():
                tentative.enter_context(v)
            tentative.pop_all()
            pass
        self.entered = True
        return self

    def __exit__(self, t, e, b) -> bool | None:
        '''"exits" all values in order they were inserted'''
        if self.entered:
            with contextlib.ExitStack() as resources:
                for k,v in self.x.items():
                    resources.push(v)
                    pass
                pass
            pass
        return None

    def __contains__(self, key:object) -> bool:
        return key in self.x

    def __delitem__(self, key:K):
        v = self.x[key]
        del self.x[key]
        if self.entered:
            with contextlib.ExitStack() as f:
                f.push(v)
                pass
            pass
        pass

    def __getitem__(self, key:K) -> V:
        return self.x[key]
    
    def __iter__(self):
        return iter(self.x.keys())

    def __len__(self) -> int:
        return len(self.x)

    def __repr__(self) -> str:
        return repr(self.x)

    def __setitem__(self, key:K, value:V) -> None:
        '''replace any current value of {key} with {value}'''
        '''- "enters" the new value and then "exits" any old value'''
        old = self.x.get(key,None)
        if old is not value:
            if self.entered:
                value.__enter__()
                if old is None:
                    self.x[key]=value
                else:
                    with contextlib.ExitStack() as f:
                        f.push(old)
                        self.x[key]=value
                    pass
                pass
            else:
                self.x[key]=value
            pass
        pass
    
    def __sizeof__(self) -> int:
        return self.x.__sizeof__()

    def clear(self):
        while len(self):
            self.popitem()
            pass
        pass

    def get(self, key, default=None):
        return self.x.get(key, default)

    def items(self) -> ItemsView[K,V]:
        '''items in order they were first inserted'''
        return self.x.items()

    def keys(self) -> KeysView[K]:
        '''keys in order they were first inserted'''
        return self.x.keys()

    @overload
    def pop(self, key:K) -> V:
        '''pop and "exit" value of {key} if self has a value for it
           otherwise raise KeyError'''

    @overload
    def pop(self, key:K, default:T) -> V|T:
        '''pop and "exit" value of {key} if self has a value for it
        otherwise return default'''

    def pop(self, key, default=None):
        if default is None:
            v = self.x.pop(key)
            if self.entered:
                with contextlib.ExitStack() as f:
                    f.push(v)
                pass
            return v
        else:
            try:
                v = self.x.pop(key)
                if self.entered:
                    with contextlib.ExitStack() as f:
                        f.push(v)
                    pass
                return v
            except KeyError:
                return default
            pass
        pass

    def popitem(self) -> Tuple[K, V]:
        '''pop and "exit" most recently added item'''
        k, v = self.x.popitem()
        if self.entered:
            with contextlib.ExitStack() as f:
                f.push(v)
                pass
            pass
        return k, v

    @overload
    def setdefault(self, key:K, default:V) -> V:
        '''set value of {key} to default "entered" if it has no value
           return value of {key}'''

    @overload
    def setdefault(self, key:K, default:None) -> Optional[V]:
        '''same as get(key,None)'''

    def setdefault(self, key, default):
        if default is not None:
            if not key in self:
                self[key] = default
            return self.x.get(key)
        else:
            return self.x.get(key, None)
        pass

    @overload
    def update(self, x:_Dict[K, V]):
        '''update self from {x} assuming its values are not yet entered'''

    @overload
    def update(self, x:Iterable[Tuple[K,V]]):
        '''update self from {x} assuming its values are not yet entered'''

    def update(self, x):
        if getattr(x,'keys',None):
            for k in x.keys():
                self[k] = x[k]
                pass
            pass
        else:
            for k,v in x:
                self[k] = v
                pass
            pass
        pass

class Opt(Generic[V],contextlib.AbstractContextManager[None|V]):
    '''holds an optional context manager
       - value can be set (see set()) and cleared (see clear()) at any time
       - if value is set while active, new value is entered and then old value if any is exited
       - if value is cleared while active, old value if any is exited
    '''
    entered = False
    x:None|V = None

    @overload
    def __init__(self):
        '''new empty optional'''
        pass
    @overload
    def __init__(self, x:Opt[V]):
        '''initialise with value from {x} assuming those values have not been "entered"'''
        pass
    @overload
    def __init__(self, x:V):
        '''initialise with value {x} assuming it has not been "entered"'''
        pass
    def __init__(self, *args):
        if len(args):
            match args[0]:
                case Opt() as y:
                    self.x = y.x
                    pass
                case v:
                    self.x=v
                    pass
            pass
        pass
    
    def __enter__(self) -> None|V:
        '''"enters" value if present'''
        result:None|V = None
        if self.x is not None:
            result=self.x.__enter__()
            pass
        self.entered=True
        return result

    def __exit__(self, t, e, b) -> bool | None:
        '''"exits" value if present'''
        assert self.entered, self.x
        if self.x:
            with contextlib.ExitStack() as resources:
                resources.push(self.x)
                pass
            pass
        self.entered=False
        return None

    def __repr__(self) -> str:
        return f"Opt({self.x})"

    @overload
    def set(self,value:V) -> None|V:
        '''replace any current value of {key} with {value}
           - if active, "enters" the new value and then "exits" any old value
           - returns old value if any
        '''
        pass
    @overload
    def set(self,value:Opt[V]) -> None|V:
        '''replace any current value of {key} with {value}
           - if active, "enters" any new value and then "exits" any old value
           - returns old value if any
           - noop if value is the old value, e.g. x.set(x.get())
        '''
        pass
    def set(self,value) -> None|V:
        '''replace any current value of {key} with {value}
           - if active, "enters" any new value and then "exits" any old value
           - returns old value if any
           - noop if value is the old value, e.g. x.set(x.get())
        '''
        if isinstance(value,Opt):
            if value.x is None:
                return self.clear()
            else:
                return self.set_value(value.x)
        return self.set_value(value)

    def set_value(self,value:V) -> None|V:
        '''replace any current value with {value}
           - assumes value has not been entered
           - if active, "enters" new value and then "exits" any old value
           - noop if value is the old value, e.g. x.set(x.get())
           - returns old value if any
        '''
        old = self.x
        if old is not value:
            if self.entered:
                value.__enter__()
                if old is None:
                    self.x=value
                else:
                    with contextlib.ExitStack() as f:
                        f.push(old)
                        self.x=value
                    pass
                pass
            else:
                self.x=value
            pass
        return old
    
    def clear(self) -> None|V:
        '''clear any current value
           - if active, "exits" old value
           - returns old value
        '''
        old = self.x
        if old is not None:
            if self.entered:
                with contextlib.ExitStack() as f:
                    f.push(old)
                    self.x=None
                pass
            else:
                self.x=None
            pass
        return old

    def get(self) -> None|V:
        '''get current value if any'''
        return self.x

class _ClassCm(contextlib.AbstractContextManager):
    '''target object {x} subclass {cls} context management methods
       - so for x: X where X(Y), __ClassCM(Y, x) will call Y.__enter__
         and Y.__exit__ directly avoiding any X.__enter__/__exit__
         overrides'''
    cls:Type
    x:object

    def __init__(self, cls, x):
        self.cls=cls
        self.x=x
        pass
    def __enter__(self):
        self.cls.__enter__(self.x)
        return self
    def __exit__(self, t, e, b) -> bool | None:
        return self.cls.__exit__(self.x, t, e, b)
    pass


def _is_subclass(n:str, t1, t2:type):
    '''check whether {n}'s type {t1} is a sub-class of {t2}'''
    try:
        Assert(t2).isInstanceOf(type)
        if issubclass(type(t1),GenericAlias):
            return _is_subclass(n,t1.__origin__,t2)
        elif issubclass(type(t1),_GenericAlias):
            return _is_subclass(n,t1.__origin__,t2)
        elif isinstance(t1,type):
            return issubclass(t1, t2)
        else:
            return False
        pass
    except Exception:
        raise in_function_context(_is_subclass,vars()) from None
    pass

ResultType=TypeVar('ResultType')

# deprecated since xju 1.2.13, use AsyncTask, because...
# this task mistakenly takes a coroutine, which means that if the task is never
# run, the program warns of coroutine-not-awaited at exit
class Task(contextlib.AbstractAsyncContextManager,Generic[ResultType]):
    '''asyncio Task context manager that guarentees Task awaited before exit
       - note that normally, application will have waited for the task
         within the context; cancel-on-exit ensures clean up on exception
         e.g. when using gather() with its default return_exceptions=False'''
    coroutine: Coroutine[Any,Any,ResultType]
    task: None | asyncio.Task[ResultType]
    def __init__(self, coroutine: Coroutine[Any,Any,ResultType]) -> None:
        self.coroutine=coroutine
        self.task=None
    async def __aenter__(self) -> asyncio.Task[ResultType]:
        '''create and return task'''
        assert self.task is None
        self.task=asyncio.create_task(self.coroutine)
        return self.task
    async def __aexit__(self, t, e, b) -> bool | None:
        '''cancel and await task'''
        assert self.task is not None
        task=self.task
        self.task=None
        task.cancel()
        try:
            await task
        except asyncio.CancelledError:
            pass
        return None

class Thread(Generic[ResultType],contextlib.AbstractContextManager[Callable[[],ResultType]]):
    __t: threading.Thread
    __result: ResultType | Exception
    
    def __init__(self, run: Callable[[],ResultType], stop: Callable[[],None]):
        '''at context entry call {run}() in new thread
           at context exit call {stop}() and wait for thread to exit
           result is not available until context exited'''
        self.run = run
        self.stop = stop
        self.__t = threading.Thread(target=self.__run)
        self.__result = Exception('thread not run')
        pass

    def __str__(self):
        return f'thread {self.__t} to run {self.run}'

    def __enter__(self) -> Callable[[],ResultType]:
        '''start {self}'''
        try:
            self.__t.start()
            return self.result
        except Exception:
            raise in_function_context(Thread.__enter__,vars())
        pass

    def __exit__(self, t, e, b) -> bool | None:
        '''stop (via {self.stop}()) and wait for (i.e. join) {self}'''
        try:
            self.stop()
            self.__t.join()
            return None
        except Exception:
            raise in_function_context(Thread.__exit__, vars())

    def result(self) -> ResultType:
        '''assuming context has exited, return/raise result of run()'''
        if isinstance(self.__result, Exception):
            raise self.__result
        return self.__result

    def __run(self) -> None:
        try:
            self.__result = self.run()
        except Exception as e:
            self.__result = e
            pass
    pass

class Mutex:
    def __init__(self):
        self.m = threading.Lock()
        pass
    pass

class Lock(contextlib.AbstractContextManager):
    def __init__(self, m: Mutex):
        self.m = m
        self.active=False
        pass

    def __enter__(self) -> Self:
        self.m.m.acquire()
        self.active=True
        return self

    def __exit__(self,t,e,b) -> bool | None:
        self.active=False
        self.m.m.release()
        return None

    pass

class Condition:
    def __init__(self, m:Mutex):
        self.m = m
        self.c = threading.Condition(m.m)

    def wait_for(self, l:Lock, timeout:Duration) -> None:
        assert l.m == self.m and l.active
        self.c.wait(timeout=timeout.value())
        pass

    def notify_all(self, l:Lock) -> None:
        assert l.m == self.m and l.active
        self.c.notify_all()
    pass

_io_special_values:dict={
    "PIPE": PIPE,
    "DEVNULL": DEVNULL,
    "STDOUT": STDOUT
}

class Process:
    p: Popen | None=None

    def __init__(self,
                 argv:Sequence[str],
                 executable: Path | None =None,
                 stdin: Literal[None,"PIPE","DEVNULL"] | int | IO = None,
                 stdout: Literal[None,"PIPE","DEVNULL"] | int | IO = None,
                 stderr: Literal[None,"PIPE","DEVNULL","STDOUT"] | int | IO = None,
                 env: dict[str,str]|None=None
    ):
        self.argv=argv
        self.executable=executable
        self.stdin=_io_special_values.get(stdin,stdin)
        self.stdout=_io_special_values.get(stdout,stdout)
        self.stderr=_io_special_values.get(stderr,stderr)
        self.env=env
        pass

    def __str__(self) -> str:
        return f'process {self.p} {self.argv}'

    def __enter__(self) -> Popen:
        '''start {self}'''
        try:
            self.p = Popen(self.argv,
                           executable=self.executable,
                           stdin=self.stdin,
                           stdout=self.stdout,
                           stderr=self.stderr,
                           close_fds=True,
                           shell=False,
                           env=self.env)
            return self.p
        except Exception:
            raise in_function_context(Process.__enter__,vars())
        pass

    def __exit__(self, t, e, b) -> bool | None:
        '''kill {self}'''
        assert self.p is not None
        self.p.kill()
        self.p.wait()
        return None

    pass


ACMT = TypeVar('ACMT', bound=contextlib.AbstractAsyncContextManager)
T_ACMT = TypeVar('T_ACMT', bound=Type[contextlib.AbstractAsyncContextManager])

def async_cmclass(cls:T_ACMT) -> T_ACMT:
    '''Class decorator that adds async context management __aenter__ and __aexit__
       that enter all type-hinted attributes implementing 
       contextlib.AbstractAsyncContextManager or contextlib.AbstractContextManager
       then any existing __aenter__ and exit in the opposite order. The __aenter__
       return value is that of the existing __aenter__ or self if there is no such
       function; the __exit__ return value is that of the existing __aexit__, or None
       if there is no such function.
       See async_cmclass.py.test for examples.
    '''
    base_classes_to_enter = _make_base_classes_to_enter(cls) # type: ignore  # mypy 1.4.1 gets confused
    attrs_to_enter = _make_attrs_to_enter(cls) # type: ignore  # mypy 1.4.1 gets confused
    # need a unique place to keep resources acquired by enter so exit can
    # exit them, only want resources for self, not subclasses (which do their own
    # handling). Note replacing . with _ could lead to clashes but there's no
    # way to avoid all clashes.
    resources_attr_name=f'xju_cmc_async_cmclass_resources_{cls.__module__}.{cls.__name__}'.replace('.','_')
    orig_enter=cls.__dict__.get('__aenter__')
    async def aenter(self,
                     base_classes_to_enter=base_classes_to_enter,
                     attrs_to_enter=attrs_to_enter,
                     resources_attr_name=resources_attr_name,
                     orig_enter=orig_enter) -> Type[contextlib.AbstractAsyncContextManager[T]]:
        async with contextlib.AsyncExitStack() as tentative:
            for f in base_classes_to_enter:
                cm:contextlib.AbstractAsyncContextManager = f(self)
                await tentative.enter_async_context(cm)
                pass
            for f in attrs_to_enter.values():
                cm = f(self)
                await tentative.enter_async_context(cm)
                pass
            assert getattr(self,resources_attr_name, None) is None, f'{cls.__module__}.{cls.__name__} has a f{resources_attr_name} attribute already. Perhaps it is inherited multiple times (which is not allowed by xju.cmc.async_cmclass decorator)?'
            if orig_enter is not None:
                result=await orig_enter(self)
            else:
                result=self
                pass
            setattr(self,resources_attr_name,tentative.pop_all())
        return result
    orig_exit=cls.__dict__.get('__aexit__',None)
    async def aexit(self,t,e,b,resources_attr_name=resources_attr_name) -> None:
        resources = getattr(self,resources_attr_name)
        setattr(self,resources_attr_name,None)
        async with contextlib.aclosing(resources):
            if orig_exit is not None:
                result=await orig_exit(self,t,e,b)
                pass
            else:
                result=None
            pass
        pass
    old_sa=cls.__setattr__
    def sa(self, name, value, attrs_to_enter=attrs_to_enter):
        if getattr(self,resources_attr_name,None) and name in attrs_to_enter:
            raise Exception(
                f'xju.cmc {self} has been entered so cannot replace context-managed attribute {cls.__module__}.{cls.__name__}.{name}')
        return old_sa(self,name,value)
    setattr(cls, '__aenter__', aenter)
    setattr(cls, '__aexit__', aexit)
    setattr(cls, '__setattr__', sa)
    cls.__annotations__['__aenter__'] = type(aenter)
    cls.__annotations__['__aexit__'] = type(aexit)
    return cls

class AsyncCM(contextlib.AbstractAsyncContextManager):
    '''base class for use with async_cm_class decorator above, to appease type checker'''
    async def __aenter__(self) -> Self:
        assert False, f'{CM.__module__}.AsyncCM.__aenter__ not overridden, have you forgotten @async_cmclass on {self.__module__}.{self.__class__.__name__}?'
        return self
    async def __aexit__(self, t, e, b) -> bool | None:
        assert False, f'{CM.__module__}.AsyncCM.__aexit__ not overridden, have you forgotten @async_cmclass on {self.__module__}.{self.__class__.__name__}?'
        pass

AsyncV = TypeVar('AsyncV', bound=contextlib.AbstractAsyncContextManager)

class AsyncDict(Mapping[K, AsyncV], contextlib.AbstractAsyncContextManager):
    '''dictionary that enters and exits its values, all of which must be context managers
       note that values may be added and removed before and/or after self is entered:
         before: the value will be entered when self is entered
         after:  the value will be entered before it is added (noting that any existing
                 value will be exited once (if) it is removed) - the enter-new-exit-old 
                 order might be important to you i.e. might not be what you need!
       note that x['a']==y is not implementable, use x.set('a',y)
       note that del x['a'] is not implementable, use x.pop('a')
    '''
    entered = False
    x:_Dict[K,AsyncV]

    @overload
    def __init__(self):
        '''new empty Dict'''

    @overload
    def __init__(self, x:_Dict[K,AsyncV]):
        '''initialise with value from {x} assuming those values have not been "entered"'''

    @overload
    def __init__(self, x:Iterable[Tuple[K,AsyncV]]):
        '''initialise with value from {x} assuming those values have not been "entered"'''

    def __init__(self, *args, **kwargs):
        self.x = OrderedDict(*args, **kwargs)
        pass
    
    async def __aenter__(self):
        '''"enters" all values in order they were inserted'''
        async with contextlib.AsyncExitStack() as tentative:
            for k,v in self.x.items():
                await tentative.enter_async_context(v)
            tentative.pop_all()
            pass
        self.entered = True
        return self

    async def __aexit__(self, t, e, b) -> bool | None:
        '''"exits" all values in order they were inserted'''
        if self.entered:
            async with contextlib.AsyncExitStack() as resources:
                for k,v in reversed(self.x.items()):
                    resources.push_async_exit(v)
                    pass
                pass
            pass
        return None

    def __contains__(self, key:object) -> bool:
        return key in self.x

    def __getitem__(self, key:K) -> AsyncV:
        return self.x[key]
    
    def __iter__(self):
        return iter(self.x.keys())

    def __len__(self) -> int:
        return len(self.x)

    def __repr__(self) -> str:
        return repr(self.x)

    async def set(self, key:K, value:AsyncV) -> None:
        '''replace any current value of {key} with {value}'''
        '''- "enters" the new value and then "exits" any old value'''
        old = self.x.get(key,None)
        if old is not value:
            if self.entered:
                await value.__aenter__()
                if old is None:
                    self.x[key]=value
                else:
                    async with contextlib.AsyncExitStack() as f:
                        f.push_async_exit(old)
                        self.x[key]=value
                    pass
                pass
            else:
                self.x[key]=value
            pass
        pass
    
    def __sizeof__(self) -> int:
        return self.x.__sizeof__()

    async def clear(self):
        while len(self):
            await self.popitem()
            pass
        pass

    def get(self, key, default=None):
        return self.x.get(key, default)

    def items(self) -> ItemsView[K,AsyncV]:
        '''items in order they were first inserted'''
        return self.x.items()

    def keys(self) -> KeysView[K]:
        '''keys in order they were first inserted'''
        return self.x.keys()

    @overload
    async def pop(self, key:K) -> AsyncV:
        '''pop and "exit" value of {key} if self has a value for it
           otherwise raise KeyError'''

    @overload
    async def pop(self, key:K, default:T) -> AsyncV|T:
        '''pop and "exit" value of {key} if self has a value for it
        otherwise return default'''

    async def pop(self, key, *rem):
        if not rem:
            v = self.x.pop(key)
            if self.entered:
                async with contextlib.AsyncExitStack() as f:
                    f.push_async_exit(v)
                pass
            return v
        else:
            try:
                v = self.x.pop(key)
                if self.entered:
                    async with contextlib.AsyncExitStack() as f:
                        f.push_async_exit(v)
                    pass
                return v
            except KeyError:
                return rem[0]
            pass
        pass

    async def popitem(self) -> Tuple[K, AsyncV]:
        '''pop and "exit" most recently added item'''
        k, v = self.x.popitem()
        if self.entered:
            async with contextlib.AsyncExitStack() as f:
                f.push_async_exit(v)
                pass
            pass
        return k, v

    @overload
    async def setdefault(self, key:K, default:AsyncV) -> AsyncV:
        '''set value of {key} to default "entered" if it has no value
           return value of {key}'''

    @overload
    async def setdefault(self, key:K, default:None) -> AsyncV|None:
        '''same as get(key,None)'''

    async def setdefault(self, key, default):
        if default is not None:
            if not key in self:
                await self.set(key,default)
            return self.x.get(key)
        else:
            return self.x.get(key, None)
        pass

    @overload
    async def update(self, x:_Dict[K, AsyncV]):
        '''update self from {x} assuming its values are not yet entered
           - note if k,v from x is alredy in self, v is not re-entered
        '''

    @overload
    async def update(self, x:Iterable[Tuple[K,AsyncV]]):
        '''update self from {x} assuming its values are not yet entered
           - note if k,v from x is alredy in self, v is not re-entered
        '''
    async def update(self, x):
        if getattr(x,'keys',None):
            for k in x.keys():
                await self.set(k,x[k])
                pass
            pass
        else:
            for k,v in x:
                await self.set(k,v)
                pass
            pass
        pass

class AsyncOpt(Generic[AsyncV],contextlib.AbstractAsyncContextManager[None|AsyncV]):
    '''holds an optional context manager
       - value can be set (see set()) and cleared (see clear()) at any time
       - if value is set while active, new value is entered and then old value if any is exited
       - if value is cleared while active, old value if any is exited
    '''
    entered = False
    x:None|AsyncV = None

    @overload
    def __init__(self):
        '''new empty optional'''
        pass
    @overload
    def __init__(self, x:AsyncOpt[AsyncV]):
        '''initialise with value from {x} assuming those values have not been "entered"'''
        pass
    @overload
    def __init__(self, x:AsyncV):
        '''initialise with value {x} assuming it has not been "entered"'''
        pass
    def __init__(self, *args, **kwargs):
        if len(args):
            match args[0]:
                case AsyncOpt() as y:
                    self.x = y.x
                    pass
                case v:
                    self.x=v
                    pass
            pass
        pass
    
    async def __aenter__(self) -> None|AsyncV:
        '''"enters" value if present'''
        result:None|AsyncV = None
        if self.x is not None:
            result=await self.x.__aenter__()
            pass
        self.entered=True
        return result

    async def __aexit__(self, t, e, b) -> bool | None:
        '''"exits" value if present'''
        assert self.entered, self.x
        if self.x:
            async with contextlib.AsyncExitStack() as resources:
                resources.push_async_exit(self.x)
                pass
            pass
        self.entered=False
        return None

    def __repr__(self) -> str:
        return f"AsyncOpt({self.x})"

    @overload
    async def set(self,value:AsyncV) -> None|AsyncV:
        '''replace any current value with {value}
           - "enters" the new value and then "exits" any old value
           - assumes value has not been entered
           - if active, "enters" any new value and then "exits" any old value
           - returns old value
           - noop if value is the old value, e.g. x.set(x.get())
        '''
        pass
    @overload
    async def set(self,value:AsyncOpt[AsyncV]) -> None|AsyncV:
        '''replace any current value with {value}
           - assumes value has not been entered
           - if active, "enters" any new value and then "exits" any old value
           - returns old value
           - noop if value is the old value, e.g. x.set(x.get())
        '''
        pass
    async def set(self,value) -> None|AsyncV:
        if isinstance(value,AsyncOpt):
            if value.x is None:
                return await self.clear()
            else:
                return await self.set_value(value.x)
        return await self.set_value(value)

    async def set_value(self,value:AsyncV) -> None|AsyncV:
        '''replace any current value with {value}
           - assumes value has not been entered
           - if active, "enters" new value and then "exits" any old value
           - returns old value
           - noop if value is the old value, e.g. x.set(x.get())
        '''
        old = self.x
        if old is not value:
            if self.entered:
                await value.__aenter__()
                if old is None:
                    self.x=value
                else:
                    async with contextlib.AsyncExitStack() as f:
                        f.push_async_exit(old)
                        self.x=value
                    pass
                pass
            else:
                self.x=value
            pass
        return old

    async def clear(self) -> None|AsyncV:
        '''clear any current value
           - if active, "exits" old value
           - returns old value
        '''
        old = self.x
        if old is not None:
            if self.entered:
                async with contextlib.AsyncExitStack() as f:
                    f.push_async_exit(old)
                    self.x=None
                pass
            else:
                self.x=None
            pass
        return old

    def get(self) -> None|AsyncV:
        '''get current value if any'''
        return self.x

class AsyncTask(contextlib.AbstractAsyncContextManager,Generic[ResultType]):
    '''asyncio Task context manager that guarentees Task awaited before exit
       - note that normally, application will have waited for the task
         within the context; cancel-on-exit ensures clean up on exception
         e.g. when using gather() with its default return_exceptions=False'''
    function: Callable[[],Coroutine[Any,Any,ResultType]]
    task: None | asyncio.Task[ResultType]
    def __init__(self, function: Callable[[],Coroutine[Any,Any,ResultType]]) -> None:
        self.function=function
        self.task=None
    async def __aenter__(self) -> asyncio.Task[ResultType]:
        '''create and return task'''
        assert self.task is None
        self.task=asyncio.create_task(self.function())
        return self.task
    async def __aexit__(self, t, e, b) -> bool | None:
        '''cancel and await task'''
        assert self.task is not None
        task=self.task
        self.task=None
        if not task.done():
            task.cancel()
            try:
                await task
            except asyncio.CancelledError:
                pass
        return None

class AsyncServiceQueue(contextlib.AbstractAsyncContextManager):
    loop:asyncio.AbstractEventLoop
    log_exception:Callable[[Exception],None]
    q:asyncio.Queue
    entered:bool

    def __init__(self,loop:asyncio.AbstractEventLoop,log_exception:Callable[[Exception],None]):
        self.loop=loop
        self.log_exception=log_exception
        self.q=asyncio.Queue()
        self.entered=False
        
    def enqueue(self, x:Coroutine[Any,Any,None]) -> None:
        '''enqueue {x} for later async execution
           - thread safe'''
        self.loop.call_soon_threadsafe(self.q.put_nowait,x)
        pass
    async def __aenter__(self) -> Self:
        self.entered=True
        return self
    async def __aexit__(self,t,e,b) -> bool | None:
        self.entered=False
        # pop entries to avoid RuntimeWarning for coroutines never awaited noting
        # they were never started
        while not self.q.empty():
            x=self.q.get_nowait()
            assert isinstance(x,Coroutine)
            x.close()
            pass
        return None

    async def run(self) -> None:
        '''execute queue entries in order logging exceptions'''
        assert self.entered, f'AsyncServiceQueue context not entered'
        while True:
            x=await self.q.get()
            await self._execute(x)
        pass

    async def _execute(self,x:Coroutine[Any,Any,None]) -> None:
        "execute coroutine {x}"
        try:
            await x
        except Exception:
            e=in_function_context(AsyncServiceQueue._execute,vars())
            assert isinstance(e,Exception)
            self.log_exception(e)
            pass
        pass
    pass

class AsyncMutex:
    def __init__(self):
        self.m = asyncio.Lock()
        pass
    pass

class AsyncLock(contextlib.AbstractAsyncContextManager):
    def __init__(self, m: AsyncMutex) -> None:
        self.m = m
        self.active=False
        pass

    async def __aenter__(self) -> Self:
        await self.m.m.acquire()
        self.active=True
        return self

    async def __aexit__(self,t,e,b) -> bool | None:
        self.active=False
        self.m.m.release()
        return None
    pass

class AsyncCondition:
    def __init__(self, m:AsyncMutex):
        self.m = m
        self.c = asyncio.Condition(m.m)

    async def wait_for(self, l:AsyncLock, timeout:Duration):
        assert l.m == self.m and l.active
        async with asyncio.timeout(timeout.value()):
            await self.c.wait()
        pass

    def notify_all(self, l:AsyncLock):
        assert l.m == self.m and l.active
        self.c.notify_all()
    pass

async def delay_cancellation(t: Awaitable[T])->T:
    """await {t}, delaying any cancel of caller until {t} completes"""
    async def f():
        return await t
    async with AsyncTask(f) as task:
        really_cancel=task.cancel
        def delay_cancel(msg:Any=None):
            assert not task.cancelled()
            pass
        try:
            setattr(task,'cancel',delay_cancel)
            try:
                # Because we await task here, a cancel on us (i.e. caller)
                # will try to cancel task as well as us, which is why
                # we have overridden task's cancel method.
                # So task does not get cancelled, but we still do...
                return await task
            except asyncio.CancelledError:
                try:
                    await task
                except:
                    pass
                raise
        finally:
            setattr(task,'cancel',really_cancel)
            pass
        pass
    pass

def _make_base_classes_to_enter(
        cls: Type[ACMT]
) -> list[Callable[[ACMT],contextlib.AbstractAsyncContextManager[None]]]:
    result: list[Callable[[ACMT],contextlib.AbstractAsyncContextManager[None]]] = []
    for base_class in cls.__bases__:
        if base_class is not AsyncCM and issubclass(base_class, contextlib.AbstractAsyncContextManager):
            result.append(
                _make_async_class_cm(base_class))  # type: ignore  # above assert...
            # ... says that base_class is a subclass of AbstractAsyncContextManager i.e. an ACMT,
            # but mypy 1.4.1 says "Argument 1 to "_make_async_class_cm" has incompatible
            # type "type[AbstractAsyncContextManager[Any]]"; expected "type[ACMT]"
            pass
        assert not issubclass(cls, CM), cls
        pass
    return result

def _make_async_class_cm(
        cls:Type[ACMT]
) -> Callable[[ACMT],contextlib.AbstractAsyncContextManager[None]]:
    def result(x:contextlib.AbstractAsyncContextManager) -> contextlib.AbstractAsyncContextManager[None]:
        return _AsyncClassCm(cls, x)
    return result
            
@dataclass
class _AsyncClassCm(contextlib.AbstractAsyncContextManager[None]):
    '''target object {x} subclass {cls} context management methods
       - so for x: X where X(Y), __ClassCM(Y, x) will call Y.__enter__
         and Y.__exit__ directly avoiding any X.__enter__/__exit__
         overrides'''
    cls:Type[contextlib.AbstractAsyncContextManager[Any]]
    x:Any

    async def __aenter__(self) -> None:
        x = self.x
        assert isinstance(x, self.cls)
        await self.cls.__aenter__(self.x)
    async def __aexit__(self, t, e, b) -> bool | None:
        return await self.cls.__aexit__(self.x, t, e, b)
    pass

def _make_attrs_to_enter(
        cls: Type[ACMT]
) -> dict[
    str,
    Callable[
        [ACMT],contextlib.AbstractAsyncContextManager
    ]
]:
    result: dict[
        str,
        Callable[
            [ACMT],contextlib.AbstractAsyncContextManager
        ]
    ] = {}
    for n, t in cls.__annotations__.items():
        if _is_subclass(n, t, contextlib.AbstractAsyncContextManager):
            result[n]=_make_async_attr_class_cm(n)
        elif _is_subclass(n, t, contextlib.AbstractContextManager):
            result[n]=_make_async_sync_attr_class_cm(n)
            pass
        pass
    return result
    
def _make_async_attr_class_cm(
        attr_name: str
) -> Callable[[contextlib.AbstractAsyncContextManager[T]],contextlib.AbstractAsyncContextManager]:
    def result(x:contextlib.AbstractAsyncContextManager[T]) -> contextlib.AbstractAsyncContextManager:
        return _AsyncAttrClassCm(x, attr_name)
    return result
            
@dataclass
class _AsyncAttrClassCm(contextlib.AbstractAsyncContextManager):
    '''target class {self.x}.{self.attr_name} async context management methods'''
    x:contextlib.AbstractAsyncContextManager
    attr_name:str

    async def __aenter__(self):
        return await getattr(self.x, self.attr_name).__aenter__()
    async def __aexit__(self, t, e, b) -> bool | None:
        return await getattr(self.x, self.attr_name).__aexit__(t, e, b)
        pass
    pass


def _make_async_sync_attr_class_cm(
        attr_name: str
) -> Callable[[contextlib.AbstractAsyncContextManager[T]],contextlib.AbstractAsyncContextManager]:
    def result(x:contextlib.AbstractAsyncContextManager[T]) -> contextlib.AbstractAsyncContextManager:
        return _AsyncSyncAttrClassCm(x, attr_name)
    return result

@dataclass
class _AsyncSyncAttrClassCm(contextlib.AbstractAsyncContextManager):
    '''target class {self.x}.{self.attr_name} non-async context management methods'''
    x:contextlib.AbstractAsyncContextManager
    attr_name:str

    async def __aenter__(self) -> None:
        return getattr(self.x, self.attr_name).__enter__()
    async def __aexit__(self, t, e, b) -> bool | None:
        return getattr(self.x, self.attr_name).__exit__(t, e, b)
    pass
