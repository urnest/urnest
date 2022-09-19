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
from typing import Generic,Tuple

Tag=TypeVar('Tag',covariant=True)

class OtherInt(Generic[Tag]):
    __value:int

    def __init__(self, value:int):
        self.__=value
        pass

    def value(self)->int:
        return self.__value

    pass

class Int(Generic[Tag],OtherInt[Tag]):
    def __str__(self)->str:
        return str(self.value())

    def __repr__(self)->str:
        return repr(self.value())

    def __reduce__(self)->Tuple:
        return (Int[Tag], (self.value(),))

    def __format__(self, format_spec:str)->str:
        return self.value().__format__(format_spec)

    def __float__(self)->float:
        return self.value().__float__()
    
    def to_bytes(self,length,byteorder,*,signed=False)->bytes:
        return self.value().to_bytes(length,byteorder,signed=signed)

    @classmethod
    def from_bytes(bytes, byteorder, *, signed=False):
        return Int[Tag](int.from_bytes(bytes,byteorder,signed))

    # generated Int methods here...

    pass


class OtherStr(Protocol[Tag]):
    def value(self)->str:
        ...
    pass

class Str(Generic[Tag],OtherStr[Tag]):
    def __init__(self, value:str):
        self.__value=value
        pass

    def value(self)->str:
        return self.__value
    
    def __str__(self)->str:
        return self.__value

    def __lt__(self, other:OtherStr[Tag]) -> bool:
        return self.value()<other.value()
    pass
