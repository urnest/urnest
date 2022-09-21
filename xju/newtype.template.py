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
from typing import Generic,Tuple,Mapping,Optional,List,Literal,Union

Tag=TypeVar('Tag',covariant=True)

class Int_(Generic[Tag]):
    __value:int

    def __init__(self, value:int):
        self.__value=value
        pass

    def value(self)->int:
        return self.__value

    pass

# mypy does not understand @classmethod
def Int_from_bytes(b:bytes, byteorder:Literal['little','big'], *, signed=False):
    return Int[Tag](int.from_bytes(b,byteorder,signed=signed))

class Int(Generic[Tag],Int_[Tag]):
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

    from_bytes=Int_from_bytes

    def conjugate(self):
        return self.value().conjugate()

    @overload
    def __divmod__(self, x:int) -> Tuple:  # Tuple[Self,Self]
        ...
    @overload
    def __divmod__(self, x:float) -> Tuple[float,float]:
        ...
    @overload
    def __divmod__(self, x:Int_[Tag]) -> Tuple:  # Tuple[int,int]
        ...
    def __divmod__(self, x):
        if isinstance(x,int):
            q,r=self.value().__divmod__(x)
            return Int[Tag](q),Int[Tag](r)
        else:
            return self.value().__divmod__(x.value())
        pass

    @overload
    def __floordiv__(self, x:int):  # -> Int[Tag]
        ...
    @overload
    def __floordiv__(self, x:float) -> float:
        ...
    @overload
    def __floordiv__(self, x:Int_[Tag]) -> int:
        ...
    def __floordiv__(self, x):
        if isinstance(x,int):
            return Int[Tag](self.value().__floordiv__(x))
        else:
            return self.value().__floordiv__(x.value())
        pass

    def __truediv__(self, x:Union[float,int,Int_[Tag]]) -> float:
        if isinstance(x,float) or isinstance(x,int):
            return self.value()/x
        else:
            return self.value()/x.value()
        pass
    
    @overload
    def __mull__(self, x:int):  # -> Int[Tag]
        ...
    @overload
    def __mull__(self, x:float) -> float:
        ...
    def __mull__(self, x):
        if isinstance(x,int):
            return Int[Tag](self.value().__mull__(x))
        else:
            return self.value().__mull__(x.value())
        pass

    @overload
    def __mod__(self, other:int):  #->Int[Tag]
        ...
    @overload
    def __mod__(self, other:float)->float:
        ...
    @overload
    def __mod__(self, other:Int_[Tag])->int:
        ...
    def __mod__(self, other):
        if type(other) is int:
            return Int[Tag](self.value()%other)
        if type(other) is float:
            return self.value()%other
        else:
            return self.value()%other.value()

    # generated Int methods here...

    pass


class Float_(Generic[Tag]):
    __value:float

    def __init__(self, value:float):
        self.__value=value
        pass

    def value(self)->float:
        return self.__value

    pass

def Float_fromhex(s:str):
    return Float[Tag](float.fromhex(s))

class Float(Generic[Tag],Float_[Tag]):
    def __str__(self)->str:
        return str(self.value())

    def __repr__(self)->str:
        return repr(self.value())

    def __reduce__(self)->Tuple:
        return (Float[Tag], (self.value(),))

    def __format__(self, format_spec:str)->str:
        return self.value().__format__(format_spec)

    def __int__(self)->int:
        return self.value().__int__()
    
    def __float__(self)->float:
        return self.value().__float__()
    
    def hex(self)->str:
        return self.value().hex()
    
    fromhex=Float_fromhex

    # generated Float methods here...

    pass


class Str_(Generic[Tag]):
    __value:str
    
    def __init__(self, value:str):
        self.__value=value
        pass

    def value(self)->str:
        return self.__value
    pass

class Str(Generic[Tag],Str_[Tag]):
    def __str__(self)->str:
        return str(self.value())

    def __repr__(self)->str:
        return repr(self.value())

    def __reduce__(self)->Tuple:
        return (Str[Tag], (self.value(),))

    def __format__(self, format_spec:str)->str:
        return self.value().__format__(format_spec)

    def splitlines(self,keepends=False)->List:
        return [Str[Tag](_) for _ in self.value().splitlines()]

    def encode(self,encoding:str='utf-8', errors:str='strict')->bytes:
        return self.value().encode()

    def __contains__(self,other:str)->bool:
        return self.value().__contains__(other)

    def zfill(self,width:int):
        return Str[Tag](self.value().zfill(width))

    def format_map(self,mapping:Mapping):
        return Str[Tag](self.value().format_map(mapping))

    def rjust(self,width:int,fillchar=' '):
        return Str[Tag](self.value().rjust(width,fillchar))

    def format(self,*args,**kwargs):
        return Str[Tag](self.value().format(*args,**kwargs))
    
    def expandtabs(self,tabsize=8):
        return Str[Tag](self.value().expandtabs(tabsize))

    # generated Str methods here...

    pass
