#!/usr/bin/env python3

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

"""
mirrors most of xju/json_codec.py.test but with all the definitions imported from
a module
"""
   
from sys import path
import ipaddress
from os.path import dirname
if path[0]==dirname(__file__): path.pop(0)

from xju.json_codec import (
    codec,
    JsonType,
    AnyJsonCodecImpl,
    SelfCodecImpl,
    encode_attr_as,
    TypeScriptSourceCode,
    TypeScriptNamespace,
    CustomClassCodec,
    PythonAttrName,
    JsonAttrName
)

from typing import cast, Type, Literal, Any, Self, NewType, Generic, TypeVar, Callable, Union
from types import NoneType
from dataclasses import dataclass, field
import re
import sys
import ipaddress
from enum import Enum, StrEnum, IntEnum, auto

from xju.assert_ import Assert
import xju.newtype
from xju.xn import readable_repr
from xju.time import Timestamp
import xju

from top.jc import (
    Int,Str,Bool,Float,List,Set,Dict,Bytes,
    Colour, Milligrams, Votes, Enabled,
    IntStrTuple, IntOrStr,Fred,FredOrJock,Seven,SevenOrEight,
    LiterallyTrue,LiterallyTrueOrFalse,JockOr6,
    Address,Suburb,Postcode,Surname,
    UnitAddress,Street,Unit,
    ShirtSize,AX,QQ,AgeInYears,Metres,Q,OA,O,
)
x:Any

# int,str,bool,float,None map directly

Assert(codec(Int).decode(3))==3
Assert(codec(Str).decode('fred'))=='fred'
Assert(codec(Bool).decode(True))==True
Assert(codec(Float).decode(5.5))==5.5

Assert(codec(Colour).decode('red'))==Colour('red')
Assert(codec(Milligrams).decode(7.6))==Milligrams(7.6)
Assert(codec(Votes).decode(7))==Votes(7)
Assert(codec(Enabled).decode(True))==Enabled(True)
Assert(codec(Enabled).encode(Enabled(True)))==True
Assert(codec(List[Int]).decode([5, 6]))==[5, 6]
Assert(codec(List).decode([5, 'fred']))==[5, 'fred']
Assert(codec(Set[Int]).decode([5, 6]))==set([5, 6])
Assert(codec(Set).decode([5, 'fred']))==set([5, 'fred'])
Assert(codec(Bytes).encode(b'\x1b\x17'))==[27,23]
tuple_codec=codec(IntStrTuple)
Assert(tuple_codec.decode([5, 'fred']))==(5, 'fred')
Assert(codec(Dict).decode({'fred':5, 'jock': 'nine'}))=={'fred':5, 'jock':'nine'}
Assert(codec(Dict[Str,Int]).decode({'fred':5, 'jock': 9}))=={'fred':5, 'jock':9}
Assert(codec(dict[Surname,int]).encode({Surname('fred'):5, Surname('jock'): 9}))=={'fred':5, 'jock':9}
Assert(codec(dict[Surname,int]).decode({'fred':5, 'jock':9}))=={Surname('fred'):5, Surname('jock'): 9}
Assert(codec(AgeInYears).decode(3))==AgeInYears(3)
Assert(codec(Metres).decode(3.8))==Metres(3.8)
Assert(codec(Surname).decode('Ang'))==Surname('Ang')
Assert(codec(Literal[O.b]).decode(7))==O.b
Assert(codec(Literal[O.a]).decode('fred'))==O.a
Assert(codec(Literal[OA.a]).encode(O.a))=='fred'
Assert(codec(O).decode('fred'))==O.a
union_codec=codec(IntOrStr)
Assert(union_codec.decode('fred'))=='fred'
Assert(codec(Fred).decode('fred'))=='fred'
Assert(codec(FredOrJock).decode('fred'))=='fred'
Assert(codec(SevenOrEight).decode(7))==7
Assert(codec(LiterallyTrue).decode(True))==True
Assert(codec(LiterallyTrueOrFalse).decode(True))==True
encode_attr_as(Address, PythonAttrName("_class"), JsonAttrName("class"))
Assert(codec(Address).decode({
    'street':'alba',
    'suburb': 'bocca',
    'postcode': 3365,
    'class': 'second'}))==Address(Street('alba'),Suburb('bocca'),Postcode(3365),'second')
Assert(codec(ShirtSize).decode(
    {
        'size': 'S',
    }
))==ShirtSize('S')
ac=codec(AX)
a1 = (ac.encode(7),
      ac.encode("fred"))
ra1:AX = ac.decode(7)
ra1 = ac.decode("fred")
q:QQ=QQ() # is this QQ[Any]?
q.q=3  # yes, no mypy error here
