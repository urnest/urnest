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
from typing import Union,List,Dict
import json
from xju.newtype import Int, Str

class ByteCountTag:pass
class ByteCount(Int[ByteCountTag]):
    pass

def toJson(x:Union[str,int,float,bool,None,List,Dict],cls=json.JSONEncoder) -> str:
    '''JSON-encode {x}
       - produces multi-line format with 4-space indent'''
    return json.dumps(x,sort_keys=True,indent=4,separators=(',',': '),cls=cls)

def fromJson(x:str) -> Union[str,int,float,bool,None,List,Dict]:
    '''JSON-decode {x}'''
    return json.loads(x)


class UserNameTag: pass
class UserName(Str[UserNameTag]):
    pass
