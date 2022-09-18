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
from typing import Generic
from dataclasses import dataclass


Tag=TypeVar('Tag',covariant=True)

class OtherInt(Protocol[Tag]):
    def value(self)->int:
        ...
    pass

class Int(Generic[Tag],OtherInt[Tag]):
    def __init__(self, value:int):
        self.__value=value
        pass

    def value(self)->int:
        return self.__value

    def __str__(self)->str:
        return str(self.__value)

    def __lt__(self, other:OtherInt[Tag]) -> bool:
        return self.value().__lt__(other.value())
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
