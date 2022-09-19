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

    
    def __abs__(self):
        return Int[Tag](self.value().__abs__())
    def __invert__(self):
        return Int[Tag](self.value().__invert__())
    def __neg__(self):
        return Int[Tag](self.value().__neg__())
    def __pos__(self):
        return Int[Tag](self.value().__pos__())
    def __trunc__(self):
        return Int[Tag](self.value().__trunc__())
    def __round__(self):
        return Int[Tag](self.value().__round__())
    def __ceil__(self):
        return Int[Tag](self.value().__ceil__())
    def __floor__(self):
        return Int[Tag](self.value().__floor__())
    def conjugate(self):
        return Int[Tag](self.value().conjugate())
    def __int__(self)->int:
        return self.value().__int__()
    def __sizeof__(self)->int:
        return self.value().__sizeof__()
    def bit_count(self)->int:
        return self.value().bit_count()
    def bit_length(self)->int:
        return self.value().bit_length()
    def __index__(self)->int:
        return self.value().__index__()
    def __hash__(self)->int:
        return self.value().__hash__()
    def __bool__(self)->bool:
        return self.value().__bool__()
    def __ror__(self,n:int):
        return Int[Tag](self.value().__ror__(n))
    def __rrshift__(self,n:int):
        return Int[Tag](self.value().__rrshift__(n))
    def __lshift__(self,n:int):
        return Int[Tag](self.value().__lshift__(n))
    def __rlshift__(self,n:int):
        return Int[Tag](self.value().__rlshift__(n))
    def __rshift__(self,n:int):
        return Int[Tag](self.value().__rshift__(n))
    def __gt__(self,other:OtherInt[Tag])->bool:
        return self.value().__gt__(other.value())
    def __lt__(self,other:OtherInt[Tag])->bool:
        return self.value().__lt__(other.value())
    def __le__(self,other:OtherInt[Tag])->bool:
        return self.value().__le__(other.value())
    def __ge__(self,other:OtherInt[Tag])->bool:
        return self.value().__ge__(other.value())
    def __eq__(self,other)->bool:
        if other.__class__==self.__class__:
            return self.value().__eq__(other.value())
        return False
    def __ne__(self,other)->bool:
        if other.__class__==self.__class__:
            return self.value().__ne__(other.value())
        return False
    def __add__(self,other:OtherInt[Tag]):
        return Int[Tag](self.value().__add__(other.value()))
    def __sub__(self,other:OtherInt[Tag]):
        return Int[Tag](self.value().__sub__(other.value()))
    def __rsub__(self,other:OtherInt[Tag]):
        return Int[Tag](self.value().__rsub__(other.value()))
    def __radd__(self,other:OtherInt[Tag]):
        return Int[Tag](self.value().__radd__(other.value()))
    def __and__(self,other:OtherInt[Tag]):
        return Int[Tag](self.value().__and__(other.value()))
    def __or__(self,other:OtherInt[Tag]):
        return Int[Tag](self.value().__or__(other.value()))
    def __rfloordiv__(self,other:OtherInt[Tag]):
        return Int[Tag](self.value().__rfloordiv__(other.value()))
    def __xor__(self,other:OtherInt[Tag]):
        return Int[Tag](self.value().__xor__(other.value()))
    def as_integer_ratio(self)->Tuple[int,int]:
        return self.value().as_integer_ratio()
    @overload
    def __mod__(self, other:float):
        ...
    @overload
    def __mod__(self, other:OtherInt[Tag])->float:
        ...
    def __mod__(self, other):
        if type(other) is int or type(other) is float:
            return Int[Tag](int(self.value().__mod__(other)))
        else:
            return self.value().__mod__(other)
    @overload
    def __divmod__(self, other:float):
        ...
    @overload
    def __divmod__(self, other:OtherInt[Tag])->float:
        ...
    def __divmod__(self, other):
        if type(other) is int or type(other) is float:
            return Int[Tag](int(self.value().__divmod__(other)))
        else:
            return self.value().__divmod__(other)
    @overload
    def __floordiv__(self, other:float):
        ...
    @overload
    def __floordiv__(self, other:OtherInt[Tag])->float:
        ...
    def __floordiv__(self, other):
        if type(other) is int or type(other) is float:
            return Int[Tag](int(self.value().__floordiv__(other)))
        else:
            return self.value().__floordiv__(other)
    @overload
    def __truediv__(self, other:float):
        ...
    @overload
    def __truediv__(self, other:OtherInt[Tag])->float:
        ...
    def __truediv__(self, other):
        if type(other) is int or type(other) is float:
            return Int[Tag](int(self.value().__truediv__(other)))
        else:
            return self.value().__truediv__(other)
    @overload
    def __mul__(self, other:float):
        ...
    @overload
    def __mul__(self, other:OtherInt[Tag])->float:
        ...
    def __mul__(self, other):
        if type(other) is int or type(other) is float:
            return Int[Tag](int(self.value().__mul__(other)))
        else:
            return self.value().__mul__(other)
    @overload
    def __rdivmod__(self, other:float):
        ...
    @overload
    def __rdivmod__(self, other:OtherInt[Tag])->float:
        ...
    def __rdivmod__(self, other):
        if type(other) is int or type(other) is float:
            return Int[Tag](int(self.value().__rdivmod__(other)))
        else:
            return self.value().__rdivmod__(other)
    @overload
    def __rtruediv__(self, other:float):
        ...
    @overload
    def __rtruediv__(self, other:OtherInt[Tag])->float:
        ...
    def __rtruediv__(self, other):
        if type(other) is int or type(other) is float:
            return Int[Tag](int(self.value().__rtruediv__(other)))
        else:
            return self.value().__rtruediv__(other)
    @overload
    def __rmul__(self, other:float):
        ...
    @overload
    def __rmul__(self, other:OtherInt[Tag])->float:
        ...
    def __rmul__(self, other):
        if type(other) is int or type(other) is float:
            return Int[Tag](int(self.value().__rmul__(other)))
        else:
            return self.value().__rmul__(other)
    @overload
    def __rmod__(self, other:float):
        ...
    @overload
    def __rmod__(self, other:OtherInt[Tag])->float:
        ...
    def __rmod__(self, other):
        if type(other) is int or type(other) is float:
            return Int[Tag](int(self.value().__rmod__(other)))
        else:
            return self.value().__rmod__(other)

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

