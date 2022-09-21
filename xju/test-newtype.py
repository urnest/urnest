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

from xju.newtype import Str,Int
from xju.assert_ import Assert

from typing import Dict
import pickle

class FirstNameType:pass
class LastNameType:pass

class FirstName(Str[FirstNameType]):pass
class LastName(Str[LastNameType]):pass

def full_name(first_name:FirstName,last_name:LastName)->str:
    return f'{first_name} {last_name}'

first_name=FirstName('fred')
last_name=LastName('jones')

Assert(full_name(first_name,last_name))=='fred jones'
#Assert(first_name).startsWith('fr')
Assert(first_name).isInstanceOf(FirstName)
Assert(first_name).isNotInstanceOf(LastName)

another_first_name=FirstName('louise')

Assert(first_name)<another_first_name
#z:Dict[FirstName,int]={ first_name: 1, another_first_name:2}
#Assert(z[first_name])==1

a=FirstName('robertalertaberta')

#assert a.count('be')==2,a
#assert a.count('ert',4)==2,a
#assert a.count('e',4,10)==1,a

#Assert(a.endswith('rta'))==True
#Assert(a.endswith('rta',14))==True
#Assert(a.endswith('rta',15))==False
#Assert(a.endswith('rta',3,7))==True
#Assert(a.endswith('rta',5,7))==False


class HoursTag: pass
class MinutesTag: pass

Hours=Int[HoursTag]
Minutes=Int[MinutesTag]

h1=Hours(7)
h2=Hours(8)
Assert(h1)<=h2
Assert(h1)!=h2
Assert(h2)>h1
Assert(h2)>=h1
Assert(h2)>=h2
Assert(h2)<=h2

Assert(str(h1))=='7'
Assert(repr(h1))=='7'
Assert(pickle.loads(pickle.dumps(h1)))==h1
Assert('{:02d}'.format(h1))=='07'
Assert(h1.__float__())==7.0
Assert(Hours.from_bytes(h1.to_bytes(4,'big'),'big'))==h1
Assert(abs(Hours(-5)))==Hours(5)
Assert(~h1)==Hours(~h1.value())
Assert(h1.conjugate())==h1.value().conjugate()
Assert(int(h1))==7
Assert(h1.__sizeof__())==h1.value().__sizeof__()
Assert(h1.bit_count())==h1.value().bit_count()
Assert(h1.bit_length())==h1.value().bit_length()
Assert(h1.__index__())==h1.value().__index__()
Assert(h1.__hash__())==h1.value().__hash__()
Assert(h1.__bool__())==h1.value().__bool__()
Assert(h1.__ror__(3))==Hours(h1.value().__ror__(3))
Assert(h1.__rrshift__(3))==Hours(h1.value().__rrshift__(3))
Assert(h1.__lshift__(3))==Hours(h1.value().__lshift__(3))
Assert(h1.__lshift__(3))==Hours(h1.value().__lshift__(3))
Assert(h1.__rlshift__(3))==Hours(h1.value().__rlshift__(3))
Assert(h1.__rshift__(3))==Hours(h1.value().__rshift__(3))
Assert(h1+h2)==Hours(15)
Assert(h1-h2)==Hours(-1)
Assert(h1.__rsub__(h2))==Hours(1)
Assert(h1&h2)==Hours(7&8)
Assert(h1|h2)==Hours(7|8)
Assert(h1.__xor__(h2))==Hours(7^8)
Assert(h1.as_integer_ratio())==h1.value().as_integer_ratio()
Assert(h1%0.5)==h1.value()%0.5
Assert(h1%h2)==h1.value()%h2.value()
Assert(h1%2)==Hours(h1.value()%2)
