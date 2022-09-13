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
import xju.cmc.tstore


# read non-existent


# create max 3 buckets, max 30 bytes

# create exists

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
