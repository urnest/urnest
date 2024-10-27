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
from typing import overload, Self, Final
import time
import asyncio
from dataclasses import dataclass
from typing import Sequence

class HoursTag:pass
class MinutesTag:pass
class SecondsTag:pass

class Hours(Int[HoursTag]): pass
class Minutes(Int[MinutesTag]): pass
class Seconds(Int[SecondsTag]): pass

class DurationTag:pass
class Duration(Float[DurationTag]):pass


@dataclass(unsafe_hash=True,order=True)
class Timestamp():
    __value:Final[float]
    def __init__(self, value:float|int):
        self.__value=float(value)
    def value(self):
        return self.__value
    def __str__(self):
        return str(self.__value)
    def __repr__(self):
        return repr(self.__value)
    def __format__(self,f:str)->str:
        return self.__value.__format__(f)
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
    def __sub__(self, t2:Self)->Duration:
        pass
    @overload
    def __sub__(self, duration:Union[Duration,Hours,Minutes,Seconds])->Self:
        pass
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

def sleep_for(x:Duration):
    time.sleep(x.value())
    pass

async def async_sleep_until(deadline: Timestamp) -> None:
    """sleep while we're not yet passed {deadline}"""
    await asyncio.sleep(max(0.0, (deadline-now()).value()))

async def async_sleep_until_first_of(a_deadline: Timestamp,
                                     *more_deadlines: Timestamp) -> None:
    deadlines:list[Timestamp]=[a_deadline]+list(more_deadlines)
    deadlines.sort()
    await async_sleep_until(deadlines.pop(0))
