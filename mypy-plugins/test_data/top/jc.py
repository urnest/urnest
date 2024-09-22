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
from xju.json_codec import (
    JsonType,
    TypeScriptSourceCode,
    TypeScriptNamespace,
)

from typing import Literal, Any, Self, NewType, Generic, TypeVar, Callable, Union
from types import NoneType
from dataclasses import dataclass, field
import re
import sys
import ipaddress
from enum import Enum, StrEnum, IntEnum, auto

import xju.newtype
from xju.time import Timestamp
import xju
Int=int
Str=str
Bool=bool
Float=float
List=list
Set=set
Dict=dict
Bytes=bytes
Colour = NewType('Colour',str)
Milligrams = NewType('Milligrams',float)
Votes = NewType('Votes',int)
Enabled = NewType('Enabled',bool)
IntStrTuple=tuple[int,str]
class SurnameTag:pass
class Surname(xju.newtype.Str[SurnameTag]):
    pattern=re.compile(r'^\S*$')  # disallow whitespace
    pass
class AgeInYearsTag:pass
class AgeInYears(xju.newtype.Int[AgeInYearsTag]):pass
class MetresTag:pass
class Metres(xju.newtype.Float[MetresTag]):pass
class O(Enum):
    a='fred'
    b=7
    pass

OA = O
IntOrStr=str|int
Fred=Literal['fred']
FredOrJock=Literal['fred','jock']
Seven=Literal[7]
SevenOrEight=Literal[7,8]
LiterallyTrue=Literal[True]
LiterallyFalse=Literal[False]
LiterallyTrueOrFalse=Literal[True,False]
JockOr6=Literal[6,'jock']
@dataclass
class Address:
    street:'Street'
    suburb:'Suburb'
    postcode:'Postcode'
    _class: Literal["first", "second"]

class StreetTag:pass
class Street(xju.newtype.Str[StreetTag]):pass

class SuburbTag:pass
class Suburb(xju.newtype.Str[SuburbTag]):pass

class PostcodeTag:pass
class Postcode(xju.newtype.Int[PostcodeTag]):pass
class UnitTag: pass
class Unit(xju.newtype.Str[UnitTag]):
    pass
@dataclass
class ManagerName:
    a: str
    b: int
@dataclass
class UnitAddress(Address,ManagerName):
    unit:Unit
@dataclass
class ShirtSize:
    size: 'SizeCode'
    pass

Small=Literal['S']
Large=Literal['L']
SizeCode=Small|Large
AY = int|str
AX = AY
Q=TypeVar('Q')
class QQ(Generic[Q]):
    q: Q
