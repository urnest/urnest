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

from xju.time import Hours,Minutes,Seconds,Duration,Timestamp,now
from typing import cast
from xju.xn import readable_repr
from xju.assert_ import Assert

t1=now()
Assert(Timestamp(23.86).value())==23.86
Assert(str(Timestamp(23.86)))==str(23.86)
Assert(repr(Timestamp(23.86)))==repr(23.86)
t1=Timestamp(23.86)
Assert(f'{t1}')=='23.86'
t2=Timestamp(23.96)
Assert(t1)==Timestamp(23.86)
Assert(t1!=t2)==True
Assert(t1)<t2
Assert(t2)>t1
Assert(t1)<=t2
Assert(t1)<=t1
Assert(t2)>=t1
Assert(t2)>=t2

Assert(Hours(1)+Timestamp(23.86))==Timestamp(3600+23.86)
Assert(Timestamp(23.86)+Minutes(1))==Timestamp(60+23.86)
Assert(Timestamp(23.86)+Seconds(1))==Timestamp(1+23.86)
Assert(Timestamp(23.86)+Duration(1.2))==Timestamp(1.2+23.86)

Assert(Timestamp(23.86)-Hours(1))==Timestamp(-3600+23.86)
Assert(Timestamp(23.86)-Minutes(1))==Timestamp(-60+23.86)
Assert(Timestamp(23.86)-Seconds(1))==Timestamp(-1+23.86)
Assert(Timestamp(23.86)-Duration(1.2))==Timestamp(-1.2+23.86)
Assert(Timestamp(23.86)-Timestamp(1))==Duration(-1+23.86)

try:
    Assert(Timestamp(23.86)-cast(Hours,1))
except Exception as e:
    Assert(readable_repr(e))=="cannot subtract 1 of type <class 'int'> from Timestamp"
else:
    assert False
    pass

try:
    t1<8
except Exception as e:
    Assert(readable_repr(e))=="'<' not supported between instances of 'Timestamp' and 'int'"
else:
    assert False
    pass

Assert(float(Timestamp(23.86)))==23.86

Assert(now())>=t1
