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
# CMC - Context Manager Collections
#
import contextlib
from typing import TypeVar, Iterable, Dict as _Dict, overload, Tuple, Sequence, Union, Optional
from typing import ItemsView, KeysView
from typing import Mapping, Type, Generic

from xju.assert_ import Assert
from collections import OrderedDict
od = OrderedDict

def class_cm(cls):
    @contextlib.contextmanager
    def result(x):
        yield cls.__enter__(x)
        return cls.__exit__(x, *sys.exc_info())
    return result


T = TypeVar('T')

class CC(Generic[T], contextlib.AbstractContextManager):
    __resources:Optional[contextlib.AbstractContextManager] = None
    __base_classes_to_enter = [ base_class for base_class in T.__bases__
                                if issubclass(base_class, contextlib.AbstractContextManager) ]
    __attrs_to_enter = [ n for n, t in T.__annotations__.items()
                         if issubclass(t, contextlib.AbstractContextManager)]
    def __enter__(self) -> CC:
        with contextlib.ExitStack() as tentative:
            for base_class in self.base_classes_to_enter:
                cm = class_cm(base_class)
                tentative.enter_context(cm)
            for n in self.attrs_to_enter:
                tentative.enter_context(getattr(self, n))
                pass
            self.__resources = tentative.pop_all()
        return self
    def __exit__(self) -> None:
        with contextlib.ExitStack() as to_free:
            for r in self.__resources:
                to_free.push(r)
        pass
    pass

# class decorator that adds context management __enter__ and __exit__
# that enter and exit all type-hinted attributes implementing context management
def cmclass(cls:Type[T]) -> Type[CC[T]]:
    return CC[T]

K = TypeVar('K')
V = TypeVar('V', bound=contextlib.AbstractContextManager)

# REVISIT: incomplete, untested, get all methods from help(dict), expand
# constructors like dict for strong typing
class Dict(Mapping[K, V], contextlib.AbstractContextManager):
    entered = False

    @overload
    def __init__(self):
        '''new empty Dict'''
        pass
    @overload
    def __init__(self, x:_Dict[K,V]):
        '''initialise with value from {x} assuming those values have not been "entered"'''
        pass
    @overload
    def __init__(self, x:Iterable[Tuple[K,V]]):
        '''initialise with value from {x} assuming those values have not been "entered"'''
        pass
    def __init__(self, *args, **kwargs):
        self.x:_Dict[K,V] = OrderedDict(*args, **kwargs)
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
        '''- "enters" the new value and "exits" any old value'''
        old = self.x.get(key)
        if old is not value:
            self.x[key]=value
            if self.entered:
                if old is None:
                    value.__enter__()
                else:
                    with contextlib.ExitStack() as f:
                        f.push(old)
                    pass
                pass
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
        pass
    @overload
    def pop(self, key:K, default:T) -> Union[V,T]:
        '''pop and "exit" value of {key} if self has a value for it
        otherwise return default'''
        pass
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
        pass
    @overload
    def setdefault(self, key:K, default:None) -> Optional[V]:
        '''same as get(key,None)'''
        pass
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
        pass
    @overload
    def update(self, x:Iterable[Tuple[K,V]]):
        '''update self from {x} assuming its values are not yet entered'''
        pass
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
