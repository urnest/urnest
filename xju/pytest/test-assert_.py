#!/usr/bin/env python3

# Copyright (c) 2018 Trevor Taylor
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
from xju.assert_ import Assert

Assert('fred').startswith('f')
Assert('fred').startsWith('f')
Assert('fred').starts_with('f')
Assert('fred').endswith('d')
Assert('fred').endsWith('d')
Assert('fred').ends_with('d')
Assert(7).isInstanceOf(int)
Assert(7).isNotInstanceOf(str)
Assert(7)>6
Assert(7)>=6
Assert(7)<=8
Assert(7)<8

try:
    Assert('fred').startsWith('g')
except Exception as e:
    assert "'fred' does not start with 'g'" in str(e),str(e)
else:
    assert False
    pass

try:
    Assert('fred').endsWith('wilfred')
except Exception as e:
    assert "'fred' does not end with 'wilfred'" in str(e),str(e)
else:
    assert False
    pass

try:
    Assert(7).isInstanceOf(str)
except Exception as e:
    assert "7 is not an instance of class str (it is of class int)" in str(e), str(e)
else:
    assert False
    pass

try:
    Assert(7).isNotInstanceOf(int)
except Exception as e:
    assert "7 is unexpectedly an instance of class int (it is of class int)" in str(e), str(e)
else:
    assert False
    pass

try:
    Assert(7)>10
except Exception as e:
    assert "7 is not greater than 10" in str(e), str(e)
else:
    assert False
    pass

try:
    Assert(7)>=10
except Exception as e:
    assert "7 is not greater than or equal to 10" in str(e), str(e)
else:
    assert False
    pass

try:
    Assert(7)<6
except Exception as e:
    assert "7 is not less than 6" in str(e), str(e)
else:
    assert False
    pass

try:
    Assert(7)<=6
except Exception as e:
    assert "7 is not less than or equal to 6" in str(e), str(e)
else:
    assert False
    pass

try:
    Assert(7)==6
except Exception as e:
    assert "7 is not equal to 6" in str(e), str(e)
else:
    assert False
    pass

try:
    Assert(7)!=7
except Exception as e:
    assert "7 is unexpectedly equal to 7" in str(e), str(e)
else:
    assert False
    pass
