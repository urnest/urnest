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
from xju.cmc.tstore import TStore

from tempfile import TemporaryDirectory
from pathlib import Path
from xju.assert_ import Assert
from xju.time import Hours
from xju.cmc.io import FileMode
from xju.misc import ByteCount

with TemporaryDirectory() as d_:
    d=Path(d_)
    # read non-existent
    try:
        tstore=TStore(d/'x.tstore')
    except FileNotFoundError as e:
        Assert(f"No such file or directory: '{d}/x.tstore/tstore.json'").isIn(str(e))
        pass
    else:
        assert False, tstore
        pass

    # create max 3 buckets, max 30 bytes
    tstore=TStore(d/'x.tstore',Hours(1),3,ByteCount(30),FileMode(0o666))
    
    # create exists
    try:
        x=TStore(d/'x.tstore',Hours(1),3,ByteCount(30),FileMode(0o666))
    except FileExistsError as e:
        Assert(f"File exists: '{d}/x.tstore'").isIn(str(e))
    else:
        assert False, x
        pass
    
# create bucket @t1
# write 30 bytes @t1

# read back via get_buckets_of t1..t1+1

# list unseen having seen nothing

# list unseen having seen half

# verify can't write 1 byte

# delete bucket

# write 10 bytes

# write 10 bytes @bucket_start(t1)+3600

# write 10 bytes @t1+2*3600

# list unseen t1 - 3600..t1+3*3600 -> 3 buckets, 10 bytes each

# read data and verify

# verify full

# make room for 1 byte

# verify 2 buckets

# make room for 10 bytes

# verify 2 buckets

# write 10 bytes to t2 bucket

# verify 10 bytes + 20 bytes

# close

# open existing

# verify 10 bytes + 20 bytes
