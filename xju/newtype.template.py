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
from typing import Generic,Tuple,Mapping,Optional,List,Literal,Union,Any

Tag=TypeVar('Tag',covariant=True)

def verify_same_type(x:Any,y:Any):
    if x.__class__ is not y.__class__:
        raise Exception(f"{x!r}'s type {x.__class__} is not the same as {y!r}'s type {y.__class__}")
    pass

def eq(x:Any,y:Any)->bool:
    verify_same_type(x,y)
    return x.value().__eq__(y.value())

class Int_(Generic[Tag]):
    __value:int

    def __init__(self, value:int):
        self.__value=value
        pass

    def value(self)->int:
        return self.__value

    pass

class Int(Generic[Tag],Int_[Tag]):
    def __eq__(self,other)->bool:
        '''equality test ignores possible subclass relationships, i.e. only valid
           for two object of exactly the same class; except that python insists
           supporting __eq__ for objects of any type, so this functions supports
           comparing any Int[X] with any non-Int but if you want to use Int[X] in
           a multi-level class hierarchy you'll need write your own __eq__ to suit
           your specific circumstances'''
        '''i.e. recommend stick to using Int[X] like:
              class Hours(Int[HoursTag]):pass
           ... and not inherit from Hours.
           If you choose to inherit from Hours, make sure you write your own __eq__'''
        return eq(self,other)

    def __ne__(self,other)->bool:
        return not eq(self,other)

    def __str__(self)->str:
        return str(self.value())

    def __repr__(self)->str:
        return repr(self.value())

    def __reduce__(self)->Tuple:
        return (self.__class__, (self.value(),))

    def __format__(self, format_spec:str)->str:
        return self.value().__format__(format_spec)

    def __float__(self)->float:
        return self.value().__float__()
    
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
            return self.__class__(q),self.__class__(r)
        if isinstance(x,float):
            return divmod(self.value(),x)
        else:
            return divmod(self.value(),x.value())
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
            return self.__class__(self.value()//x)
        elif isinstance(x,float):
            return self.value()//x
        else:
            return self.value()//x.value()
        pass

    def __truediv__(self, x:Union[float,int,Int_[Tag]]) -> float:
        if isinstance(x,float) or isinstance(x,int):
            return self.value()/x
        else:
            return self.value()/x.value()
        pass
    
    @overload
    def __mul__(self, x:int):  # -> Int[Tag]
        ...
    @overload
    def __mul__(self, x:float) -> float:
        ...
    def __mul__(self, x):
        if isinstance(x,int):
            return self.__class__(self.value()*x)
        else:
            return self.value()*x
        pass

    @overload
    def __rmul__(self, x:int):  # -> Int[Tag]
        ...
    @overload
    def __rmul__(self, x:float) -> float:
        ...
    def __rmul__(self, x):
        if isinstance(x,int):
            return self.__class__(x*self.value())
        else:
            return x*self.value()
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
            return self.__class__(self.value()%other)
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

class Float(Generic[Tag],Float_[Tag]):
    def __eq__(self,other)->bool:
        '''equality test ignores possible subclass relationships, i.e. only valid
           for two object of exactly the same class; except that python insists
           supporting __eq__ for objects of any type, so this functions supports
           comparing any Float[X] with any non-Float but if you want to use Float[X] in
           a multi-level class hierarchy you'll need write your own __eq__ to suit
           your specific circumstances'''
        '''i.e. recommend stick to using Float[X] like:
              class Timestamp(Float[TimestampTag]):pass
           ... and not inherit from Timestamp.
           If you choose to inherit from Timestamp, make sure you write your own __eq__'''
        return eq(self,other)

    def __ne__(self,other)->bool:
        return not eq(self,other)

    def __str__(self)->str:
        return str(self.value())

    def __repr__(self)->str:
        return repr(self.value())

    def __reduce__(self)->Tuple:
        return (self.__class__, (self.value(),))

    def __format__(self, format_spec:str)->str:
        return self.value().__format__(format_spec)

    def __int__(self)->int:
        return self.value().__int__()
    
    def __float__(self)->float:
        return self.value().__float__()
    
    def hex(self)->str:
        return self.value().hex()
    
    def conjugate(self):
        return self.value().conjugate()

    @overload
    def __divmod__(self, x:int) -> Tuple:  # Tuple[Self,Self]
        ...
    @overload
    def __divmod__(self, x:float) -> Tuple:  # Tuple[Self,Self]
        ...
    @overload
    def __divmod__(self, x:Float_[Tag]) -> Tuple:  # Tuple[float,float]
        ...
    def __divmod__(self, x):
        if isinstance(x,int) or isinstance(x,float):
            q,r=self.value().__divmod__(x)
            return Float[Tag](q),Float[Tag](r)
        else:
            return divmod(self.value(),x.value())
        pass

    @overload
    def __floordiv__(self, x:int):  # -> Float[Tag]
        ...
    @overload
    def __floordiv__(self, x:float):  # -> Float[Tag]
        ...
    @overload
    def __floordiv__(self, x:Float_[Tag]) -> float:
        ...
    def __floordiv__(self, x):
        if isinstance(x,int) or isinstance(x,float):
            return Float[Tag](self.value()//x)
        else:
            return self.value()//x.value()
        pass

    @overload
    def __truediv__(self, x:Union[float,int]): # -> Float[Tag]
        ...
    @overload
    def __truediv__(self, x:Float_[Tag]) -> float:
        ...
    def __truediv__(self, x):
        if isinstance(x,int) or isinstance(x,float):
            return Float[Tag](self.value()/x)
        else:
            return self.value()/x.value()
        pass
    
    @overload
    def __mul__(self, x:int):  # -> Float[Tag]
        ...
    @overload
    def __mul__(self, x:float):  # -> Float[Tag]
        ...
    def __mul__(self, x):
        return Float[Tag](self.value()*x)

    @overload
    def __rmul__(self, x:int):  # -> Float[Tag]
        ...
    @overload
    def __rmul__(self, x:float):  # -> Float[Tag]
        ...
    def __rmul__(self, x):
        return Float[Tag](x*self.value())

    @overload
    def __mod__(self, other:int):  #->Float[Tag]
        ...
    @overload
    def __mod__(self, other:float):  #->Float[Tag]
        ...
    @overload
    def __mod__(self, other:Float_[Tag])->float:
        ...
    def __mod__(self, other):
        if isinstance(other,int) or isinstance(other,float):
            return Float[Tag](self.value()%other)
        else:
            return self.value()%other.value()

    def __round__(self, ndigits:int):  #->Float[Tag]
        return Float[Tag](self.value().__round__(ndigits))

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
    def __eq__(self,other)->bool:
        '''equality test ignores possible subclass relationships, i.e. only valid
           for two object of exactly the same class; except that python insists
           supporting __eq__ for objects of any type, so this functions supports
           comparing any Str[X] with any non-Str but if you want to use Str[X] in
           a multi-level class hierarchy you'll need write your own __eq__ to suit
           your specific circumstances'''
        '''i.e. recommend stick to using Str[X] like:
              class FirstName(Str[FirstNameTag]):pass
           ... and not inherit from FirstName.
           If you choose to inherit from Timestamp, make sure you write your own __eq__'''
        return eq(self,other)

    def __ne__(self,other)->bool:
        return not eq(self,other)

    def __str__(self)->str:
        return str(self.value())

    def __repr__(self)->str:
        return repr(self.value())

    def __reduce__(self)->Tuple:
        return (self.__class__, (self.value(),))

    def __format__(self, format_spec:str)->str:
        return self.value().__format__(format_spec)

    def splitlines(self,keepends=False)->List:
        return [self.__class__(_) for _ in self.value().splitlines()]

    def encode(self,encoding:str='utf-8', errors:str='strict')->bytes:
        return self.value().encode()

    def __contains__(self,other:str)->bool:
        return self.value().__contains__(other)

    def zfill(self,width:int):
        return self.__class__(self.value().zfill(width))

    def format_map(self,mapping:Mapping):
        return self.__class__(self.value().format_map(mapping))

    def format(self,*args,**kwargs):
        return self.__class__(self.value().format(*args,**kwargs))
    
    def expandtabs(self,tabsize=8):
        return self.__class__(self.value().expandtabs(tabsize))

    def __getitem__(self,key):
        return self.value().__getitem__(key)

    # generated Str methods here...

    pass
