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
from typing import Iterable,Sized,Container,Collection,Reversible,Protocol,cast,Type

class RunTimeStr:
    def __init__(self, s:str):
        self.value=s
        pass
    
    def __str__(self):
        return self.value
    
    def __getattr__(self, n, *args, **argv):
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
    pass


class StringLike(Container[str],Reversible[str],Protocol):
    def encode(self,encoding='utf-8', errors='strict'):
        ...
    pass

def stringlike(cls:Type[RunTimeStr]) -> StringLike:
    return cast(StringLike,cls)
