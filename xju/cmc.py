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
from typing import TypeVar, Iterable

# class decorator that adds context management __enter__ and __exit__
# that enter and exit all type-hinted attributes implementing context management
def cmlass():
    

K = TypeVar()
V = TypeVar('V', contextlib.ContextManager)


# REVISIT: incomplete, untested, get all methods from help(dict), expand
# constructors like dict for strong typing
class Dict(typing.Mapping[K, V], contextlib.ContextManager):
    def __init__(self, *args, **kwargs):
        self.x = dict(*args, **kwargs)

    def __enter__(self):
        with contextlib.ExitStack() as tentative:
            for v in self.values():
                tentative.enter_context(v)
            tentative.pop_all()
        return self
    def __exit__(self, t, e, b):
        with contextlib.ExitStack() as resources:
            for v in self.values():
                resources.push(v)

    def __setitem__(self, key:K, value:V) -> None:
        value.__enter__()
        super().__setitem__(key, value)

    def __delitem__(self, key:K):
        v = self[key]
        super().__delitem__(key)
        v.__exit__(*sys.exc_info())

    def clear(self):
        while len(self):
            self.popitem()

    def update(self, other: dict[K,V] | Iterable[Tuple[K,V]]):
        for k, v in other:
            self[k] = v
                
    def popitem(self):
        k, v = super().popitem()
        v.__exit__(*sys.exc_info())
        return k, v

    def pop(self, key:K, default=None) -> V:
        if default is None:
            v = super.pop(key)
            v.__exit__(*sys.exc_info())
            return v
        else:
            try:
                v = super.pop(key)
                v.__exit__(*sys.exc_info())
                return v
            except KeyError:
                return default

    def setdefault(self, key:K, default:V|None=None):
        if not key in self:
            self[key] = default
        return self[key]
