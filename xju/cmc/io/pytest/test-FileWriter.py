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


from xju.cmc.io import FileWriter,FileReader,FileMode
from pathlib import Path
from xju.assert_ import Assert
from xju.xn import readable_repr
from xju.misc import ByteCount

try:
    with FileWriter(Path("xxx.txt")) as f:
        pass
except Exception as e:
    Assert(readable_repr(e))=="Failed to open file writer for xxx.txt with with create mode None, must not exist False, close-on-exec True because\n[Errno 2] No such file or directory: 'xxx.txt'."
else:
    assert False, f'should not be here with {f}'
    pass

with FileWriter(Path('xxx.txt'),mode=FileMode(0o666)) as f:
    f.output.write(b'fred')
    Assert(f.size())==ByteCount(4)
    f.output.write(b'ward')
    pass

try:
    with FileWriter(Path("xxx.txt"), mode=FileMode(0o666), must_not_exist=True) as f:
        pass
except FileExistsError as e:
    Assert(readable_repr(e))=="Failed to open file writer for xxx.txt with with create mode 0o666, must not exist True, close-on-exec True because\n[Errno 17] File exists: 'xxx.txt'."
else:
    assert False, f'should not be here with {f}'
    pass

with FileReader(Path('xxx.txt')) as f2:
    Assert(f2.input.readall())==b'fredward'
    pass

with FileWriter(Path("xxx.txt")) as f:
    f.seek_by(4)
    f.output.write(b'lea')
    f.seek_to(1)
    f.output.write(b'i')
    pass

assert not hasattr(f,'output')

with FileReader(Path('xxx.txt')) as f2:
    Assert(f2.input.readall())==b'fiedlead'
    pass
