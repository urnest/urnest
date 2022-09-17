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
# Utilities for creating distinct types mirroring a subset of a basic
# type (str, int, float, bytes).
#
# Unlike typing.NewType:
#   - works with isinstance (at runtime)
#   - many methods of the basic type are provided directly, e.g. for
#     new int type A, A+A->A

from typing import Iterable,Sized,Container,Collection,Reversible,Protocol,cast,Type,overload,TypeVar
from dataclasses import dataclass

@dataclass
class RunTimeStr:
    value:str
    def __str__(self):
        return self.value
    
    def __getattr__(self, n, *args, **argv):
        return getattr(self.value,n)
        if n=='__init__':
            self.__dict__.__init__(self,*args,**argv)
        else:
            # probably also want to override repr?
            return getattr(self.value,n)
        pass
    def __len__(self):
        return len(self.value)
    def __getitem__(self, key):
        return self.value.__getitem__(key)
    def __iter__(self):
        return self.value.__iter__()
    def __reversed__(self):
        return self.value.__reversed__()
    def __contains__(self, item):
        return self.value.__contains__(item)
    def __hash__(self) -> int:
        return self.value.__hash__()
    def __lt__(self, other) -> bool:
        return self.value.__lt__(other.value)
    def __le__(self, other) -> bool:
        return self.value.__le__(other.value)
    def __eq__(self, other) -> bool:
        return self.value.__eq__(other.value)
    def __ne__(self, other) -> bool:
        return self.value.__ne__(other.value)
    def __gt__(self, other) -> bool:
        return self.value.__gt__(other.value)
    def __ge__(self, other) -> bool:
        return self.value.__ge__(other.value)
    pass

S=TypeVar('S',bound=RunTimeStr,covariant=True)

class StringLike(Container[str],Reversible[str],Protocol[S]):
    @overload
    def count(self,sub:str) -> int:
        ...
    @overload
    def count(self,sub:str, start:int) -> int:
        ...
    @overload
    def count(self,sub:str, start:int, end:int) -> int:
        ...
    def encode(self,encoding='utf-8', errors='strict') -> bytes:
        ...
    @overload
    def endswith(self,suffix:str) -> bool:
        ...
    @overload
    def endswith(self,suffix:str, start:int) -> bool:
        ...
    @overload
    def endswith(self,suffix:str, start:int, end:int) -> bool:
        ...
    @overload
    def find(self,sub:str) -> int:
        ...
    @overload
    def find(self,sub:str, start:int) -> int:
        ...
    @overload
    def find(self,sub:str, start:int, end:int) -> int:
        ...
    def isalnum(self) -> bool:
        ...
    def isalpha(self) -> bool:
        ...
    def isascii(self) -> bool:
        ...
    def isdecimal(self) -> bool:
        ...
    def isdigit(self) -> bool:
        ...
    def isidentifier(self) -> bool:
        ...
    def islower(self) -> bool:
        ...
    def isnumeric(self) -> bool:
        ...
    def isprintable(self) -> bool:
        ...
    def isspace(self) -> bool:
        ...
    def istitle(self) -> bool:
        ...
    def isupper(self) -> bool:
        ...
    @overload
    def rfind(self,sub:str) -> int:
        ...
    @overload
    def rfind(self,sub:str, start:int) -> int:
        ...
    @overload
    def rfind(self,sub:str, start:int, end:int) -> int:
        ...
    @overload
    def rindex(self,sub:str) -> int:
        ...
    @overload
    def rindex(self,sub:str, start:int) -> int:
        ...
    @overload
    def rindex(self,sub:str, start:int, end:int) -> int:
        ...
    @overload
    def startswith(self,prefix:str, start:int, end:int) -> bool:
        ...
    @overload
    def startswith(self,prefix:str) -> bool:
        ...
    @overload
    def startswith(self,prefix:str, start:int) -> bool:
        ...
    def __hash__(self) -> int:
        ...
    def __lt__(self, other) -> bool:
        ...
    def __le__(self, other) -> bool:
        ...
    def __eq__(self, other) -> bool:
        ...
    def __ne__(self, other) -> bool:
        ...
    def __gt__(self, other) -> bool:
        ...
    def __ge__(self, other) -> bool:
        ...

    pass

def stringlike(cls:Type[S]) -> StringLike[S]:
    return cast(StringLike[S],cls)
