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
from xju.newtype import Int,Float
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

class TimestampBase:pass
class Timestamp(TimestampBase):
    __value:float
    def __init__(self,value:float):
        self.__value=value
        pass
    def value(self):
        return __value
    def __add__(self, duration:Duration):
        return Timestamp(self.value()+duration.value())
    @overload
    def __sub__(self, t2:TimestampBase)->Duration:
        ...
    @overload
    def __sub__(self, duration:Duration):
        ...
    def __sub__(self, x):
        if isinstance(x,Duration):
            return Timestamp(self.value()-x.value())
        elif isinstance(x,Timestamp):
            return Duration(self.value()-x.value())
        x_type=type(x)
        assert False, f'cannot subtract {x} of type {x_type} from Timestamp'
    def __float__(self):
        return self.__value
    pass

def now()->Timestamp:
    return Timestamp(time.time())
