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

from xju.cmc.io import FileReader
from pathlib import Path
from xju.assert_ import Assert
from xju.xn import readableRepr

try:
    with FileReader(Path("xxx.txt")) as f:
        pass
except Exception as e:
    Assert(readableRepr(e))=="Failed to open xxx.txt reader with close-on-exec True because\n[Errno 2] No such file or directory: 'xxx.txt'."
else:
    assert False, f'should not be here with {f}'
    pass

with open('xxx.txt','w') as f2:
    f2.write('fredward')
    pass

with FileReader(Path("xxx.txt")) as f:
    Assert(f.input.readall().decode('utf-8'))=="fredward"
    Assert(f.size())==8
    Assert(f.seek_by(-2).input.read(2))==b'rd'
    Assert(f.seek_to(2).input.read(3))==b'edw'
    # seeking past end is not an error, even if it makes no sense
    Assert(f.seek_to(9).input.tell())==9

    try:
        f.seek_to(-1)
    except Exception as e:
        Assert(readableRepr(e).replace('\n',' ')).matches(
            'Failed to position so next write occurs -1 bytes from start of file because.* Invalid argument.')
    else:
        assert False, 'should not b here'
    pass

assert not hasattr(f,'input')
