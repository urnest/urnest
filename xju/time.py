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
from xju.newtype import Int,Float,Union
from typing import overload
import time

class HoursTag:pass
class MinutesTag:pass
class SecondsTag:pass

class Hours(Int[HoursTag]): pass
class Minutes(Int[MinutesTag]): pass
class Seconds(Int[SecondsTag]): pass

class DurationTag:pass
class Duration(Float[DurationTag]):pass

class TimestampBase:pass  # until typing.Self
class Timestamp(TimestampBase):
    __value:float
    def __init__(self,value:float):
        self.__value=value
        pass
    def value(self):
        return self.__value
    def __format__(self,f:str)->str:
        return f.format(self.__value)
    def __add__(self, x:Union[Duration,Hours,Minutes,Seconds]):
        if isinstance(x,Hours):
            return Timestamp(self.value()+x.value()*3600)
        if isinstance(x,Minutes):
            return Timestamp(self.value()+x.value()*60)
        if isinstance(x,Seconds):
            return Timestamp(self.value()+x.value())
        return Timestamp(self.value()+x.value())
    def __radd__(self, duration:Union[Duration,Hours,Minutes,Seconds]):
        return self.__add__(duration)

    @overload
    def __sub__(self, t2:TimestampBase)->Duration:
        ...
    @overload
    def __sub__(self, duration:Union[Duration,Hours,Minutes,Seconds]):  # -> Timestamp
        ...
    def __sub__(self, x):
        if isinstance(x,Duration):
            return Timestamp(self.value()-x.value())
        if isinstance(x,Hours):
            return Timestamp(self.value()-x.value()*3600)
        if isinstance(x,Minutes):
            return Timestamp(self.value()-x.value()*60)
        if isinstance(x,Seconds):
            return Timestamp(self.value()-x.value())
        if isinstance(x,Timestamp):
            return Duration(self.value()-x.value())
        x_type=type(x)
        assert False, f'cannot subtract {x} of type {x_type} from Timestamp'
    def __float__(self):
        return self.__value
    pass

def now()->Timestamp:
    return Timestamp(time.time())
