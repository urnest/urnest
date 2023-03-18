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
# CMC - Context Management Classic i.e. non-async
#
from __future__ import annotations

import asyncio
import sys
import contextlib
from typing import TypeVar, Iterable, Dict as _Dict, overload, Tuple, Sequence, Union, Optional
from typing import ItemsView, KeysView
from typing import _GenericAlias # type: ignore  # mypy 1.0.0 :-(
from types import GenericAlias
from typing import Mapping, Type, List, Generic, Any, Callable, Awaitable
from collections import OrderedDict
from collections.abc import Coroutine
import builtins
import threading
from xju.time import Duration
from xju.xn import in_function_context, in_context
from xju.assert_ import Assert

T = TypeVar('T', bound=contextlib.AbstractContextManager)

def cmclass(cls:Type[T]) -> Type[T]:
    '''Class decorator that adds context management __enter__ and __exit__
       that enter and exit all type-hinted attributes implementing contextlib.AbstractContextManager
       then any existing __enter__.
       Note that to satisfy mypy, the decorated class must already
       implement contextlib.AbstractContextManager and the preferred way to do that is
       to inherit from xju.cmc.CM - see cmc.py.test for examples.
    '''
    base_classes_to_enter = [ base_class for base_class in cls.__bases__
                              if issubclass(base_class, contextlib.AbstractContextManager) ]
    attrs_to_enter = [ n for n, t in cls.__annotations__.items()
                         if is_subclass(n, t, contextlib.AbstractContextManager)]
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
              orig_enter=orig_enter) -> Type[T]:
        with contextlib.ExitStack() as tentative:
            for base_class in base_classes_to_enter:
                if base_class is not CM:
                    cm = __ClassCm(base_class, self)
                    tentative.enter_context(cm)
                    pass
                pass
            for n in attrs_to_enter:
                tentative.enter_context(getattr(self, n))
                pass
            assert getattr(self,resources_attr_name, None) is None, f'{cls.__module__}.{cls.__name__} has a f{resources_attr_name} attribute already. Perhaps it is inherited multiple times (which is not allowed by xju.cmc.cmclass decorator)?'
            orig_enter(self)
            setattr(self,resources_attr_name,tentative.pop_all())
        return self
    orig_exit=cls.__dict__.get('__exit__',lambda self,t,e,b: None)
    def exit(self,t,e,b,resources_attr_name=resources_attr_name) -> None:
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
    def __exit__(self, t, e, b) -> None:
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

    def __exit__(self, t, e, b):
        '''"exits" all values in order they were inserted'''
        if self.entered:
            with contextlib.ExitStack() as resources:
                for k,v in self.x.items():
                    resources.push(v)
                    pass
                pass
            pass
        pass

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
    def pop(self, key:K, default:T) -> Union[V,T]:
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

class __ClassCm(contextlib.AbstractContextManager):
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
    def __exit__(self, t, e, b):
        return self.cls.__exit__(self.x, t, e, b)
    pass


def is_subclass(n:str, t1, t2:type):
    '''check whether {n}'s type {t1} is a sub-class of {t2}'''
    try:
        Assert(t2).isInstanceOf(type)
        if issubclass(type(t1),GenericAlias):
            return is_subclass(n,t1.__origin__,t2)
        elif issubclass(type(t1),_GenericAlias):
            return is_subclass(n,t1.__origin__,t2)
        elif isinstance(t1,type):
            return issubclass(t1, t2)
        else:
            return False
        pass
    except Exception:
        raise in_function_context(is_subclass,vars()) from None
    pass

ResultType=TypeVar('ResultType')
class Task(contextlib.AbstractAsyncContextManager,Generic[ResultType]):
    '''asyncio Task context manager that guarentees Task awaited before exit
       - note that normally, application will have waited for the task
         within the context; cancel-on-exit ensures clean up on exception
         e.g. when using gather() with its default return_exceptions=False'''
    coroutine: Coroutine[Any,Any,ResultType]
    task: None | asyncio.Task[ResultType]
    def __init__(self, coroutine: Coroutine[Any,Any,ResultType]):
        self.coroutine=coroutine
        self.task=None
    async def __aenter__(self) -> asyncio.Task[ResultType]:
        '''create and return task'''
        assert self.task is None
        self.task=asyncio.create_task(self.coroutine)
        return self.task
    async def __aexit__(self, t, e, b):
        '''cancel and await task'''
        assert self.task is not None
        task=self.task
        self.task=None
        task.cancel()
        try:
            await task
        except asyncio.CancelledError:
            pass

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

    def __exit__(self, t, e, b):
        '''stop (via {self.stop}()) and wait for (i.e. join) {self}'''
        try:
            self.stop()
            self.__t.join()
        except Exception:
            raise in_function_context(Thread.__exit__, vars())
        pass

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

    def __enter__(self):
        self.m.m.acquire()
        self.active=True
        return self

    def __exit__(self,t,e,b):
        self.active=False
        self.m.m.release()
        pass

    pass

class Condition:
    def __init__(self, m:Mutex):
        self.m = m
        self.c = threading.Condition(m.m)

    def wait_for(self, l:Lock, timeout:Duration):
        assert l.m == self.m and l.active
        self.c.wait(timeout=timeout.value())
        pass

    def notify_all(self, l:Lock):
        assert l.m == self.m and l.active
        self.c.notify_all()
    pass

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
    async def __aenter__(self):
        self.entered=True
        return self
    async def __aexit__(self,t,e,b) -> None:
        self.entered=False
        # pop entries to avoid RuntimeWarning for coroutines never awaited noting
        # they were never started
        while not self.q.empty():
            x=self.q.get_nowait()
            assert isinstance(x,Coroutine)
            x.close()
        pass

    async def run(self) -> None:
        '''execute queue entries in order logging exceptions'''
        assert self.entered, f'AsyncServiceQueue context not entered'
        while True:
            x=await self.q.get()
            await self._execute(x)
        pass

    async def _execute(self,x:Coroutine[Any,Any,None]) -> None:
        try:
            await x
        except Exception:
            e=in_function_context(AsyncServiceQueue._execute,vars())
            assert isinstance(e,Exception)
            self.log_exception(e)
            pass
        pass
        
    pass
