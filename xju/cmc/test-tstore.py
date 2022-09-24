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
from xju.cmc.tstore import TStore,BucketID,BucketStart,BucketExists,NoSuchBucket,Writer,Reader

from typing import cast
from tempfile import TemporaryDirectory
from pathlib import Path
from xju.assert_ import Assert
from xju.time import Hours,now,Timestamp
from xju.cmc.io import FileMode,FilePosition
from xju.misc import ByteCount

r:Reader
w:Writer

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
    t1=now()
    s1=tstore.calc_bucket_start(t1)
    id1=BucketID('1')
    tstore.create_bucket(s1,id1)
    try:
        tstore.create_bucket(s1,id1)
    except BucketExists as e:
        Assert(f'').isIn(str(e))
        pass
    else:
        assert False, 'no BucketExists'
        pass
    # write 30 bytes @t1
    with tstore.new_writer(s1,id1) as w:
        thirty_bytes:bytes=b''.join([i.to_bytes(1,'little') for i in range(0,30)])
        w.append(thirty_bytes)

        # read back via get_buckets_of t1..t1+1
        buckets=tstore.get_buckets_of(t1,t1+Timestamp(1.0))
        Assert(buckets)==buckets[0:1]
        for bucket_start,bucket_id in buckets:
            with cast(Reader,tstore.new_reader(bucket_start,bucket_id)) as r:
                Assert(r.size())==ByteCount(30)
                Assert(r.read(ByteCount(15)))==thirty_bytes[0:15]
                r.seek_to(FilePosition(10))
                Assert(r.read(ByteCount(5)))==thirty_bytes[10:15]
                r.seek_by(ByteCount(8))
                Assert(r.read(ByteCount(13)))==thirty_bytes[23:]
                pass
            pass
        pass
    
    # list unseen having seen nothing
    Assert(tstore.list_unseen({}))=={ (s1,id1):ByteCount(30) }
    
    # list unseen having seen half
    Assert(tstore.list_unseen({(s1,id1):ByteCount(15)}))=={ (s1,id1):ByteCount(30) }

    # list unseen having seen all
    Assert(tstore.list_unseen({(s1,id1):ByteCount(30)}))=={}

    # list unseen having seen non-existent
    Assert(tstore.list_unseen({(s1,BucketID('2')):ByteCount(30)}))=={ (s1,BucketID('2')): ByteCount(0),
                                                                      (s1,id1):ByteCount(30)}

    tstore.verify_bucket_exists(s1,id1)
    try:
        tstore.verify_bucket_exists(s1,BucketID('2'))
    except BucketExists as e:
        Assert(f"bucket at {s1} exists with uid 1").isIn(str(e))
        pass
    else:
        assert False, f"BucketID('2') should not exist at {s1}"
        pass

    s2=tstore.calc_bucket_start(t1+Timestamp(3600))
    Assert(s1)<s2
    id2=BucketID('2')
    
    # verify can't write 1 byte
    Assert(tstore.current_size())==ByteCount(30)
    
    # delete bucket
    tstore.delete_bucket(s1,id1)

    Assert(tstore.current_size())==ByteCount(0)
    Assert(tstore.list_unseen({}))=={}
    try:
        tstore.verify_bucket_exists(s1,id1)
    except NoSuchBucket as e:
        Assert(f"no bucket starting at {s1}").isIn(str(e))
        pass
    else:
        assert False, f'bucket {id1} should not exist at {s1}'
        pass
    
    s3=tstore.calc_bucket_start(t1+2*Timestamp(3600))
    Assert(s3)>s2
    id3=BucketID('3')

    tstore.create_bucket(s1,id1)
    tstore.create_bucket(s2,id2)
    tstore.create_bucket(s3,id3)
    
    # write 10 bytes
    with tstore.new_writer(s1,id1) as w:
        w.append(b'0123456789')
        Assert(tstore.current_size())==ByteCount(10)
        pass
    
    # write 10 bytes @bucket_start(t1)+3600
    with tstore.new_writer(s2,id2) as w:
        w.append(b'abcdefghij')
        Assert(tstore.current_size())==ByteCount(20)
        pass

    # write 10 bytes @t1+2*3600
    with tstore.new_writer(s3,id3) as w:
        w.append(b'abcde01234')
        Assert(tstore.current_size())==ByteCount(30)
        pass

    # list unseen t1 - 3600..t1+3*3600 -> 3 buckets, 10 bytes each
    Assert(tstore.list_unseen({}))=={ (s1,id1): ByteCount(10),
                                      (s2,id2): ByteCount(10),
                                      (s3,id3): ByteCount(10) }
    

    # read data and verify
    with tstore.new_reader(s1,id1) as r:
        Assert(r.read(ByteCount(11)))==b'0123456789'
        pass
    with tstore.new_reader(s2,id2) as r:
        Assert(r.read(ByteCount(11)))==b'abcdefghij'
        pass
    with tstore.new_reader(s3,id3) as r:
        Assert(r.read(ByteCount(11)))==b'abcde01234'
        pass

    # make room for 1 byte
    tstore.make_room_for(ByteCount(1))

    # verify 2 buckets
    Assert(tstore.list_unseen({}))=={ (s2,id2): ByteCount(10),
                                      (s3,id3): ByteCount(10) }

    # make room for 10 bytes
    tstore.make_room_for(ByteCount(10))

    # verify 2 buckets
    Assert(tstore.list_unseen({}))=={ (s2,id2): ByteCount(10),
                                      (s3,id3): ByteCount(10) }

    # write 10 bytes to t2 bucket
    with tstore.new_writer(s2,id2) as w:
        w.append(b'poiuytrewq')
        Assert(w.size())==ByteCount(20)
        pass
    
    # verify 10 bytes + 20 bytes
    with tstore.new_reader(s2,id2) as r:
        Assert(r.read(ByteCount(21)))==b'abcdefghijpoiuytrewq'
        pass
    with tstore.new_reader(s3,id3) as r:
        Assert(r.read(ByteCount(11)))==b'abcde01234'
        pass

    # close - nothing to do

    # open existing
    tstore=TStore(d/'x.tstore')
    Assert(tstore.storage_path)==d/'x.tstore'
    Assert(tstore.hours_per_bucket)==Hours(1)
    Assert(tstore.max_buckets)==3
    Assert(tstore.max_size)==ByteCount(30)
    Assert(tstore.file_creation_mode)==FileMode(0o666)
    Assert(tstore.calc_bucket_start(t1))==s1
    Assert(tstore.calc_bucket_start(t1+Timestamp(3600)))==s2
    Assert(tstore.calc_bucket_start(t1+2*Timestamp(3600)))==s3
    id2=tstore.get_bucket(s2)
    id3=tstore.get_bucket(s3)
    Assert(tstore.current_size())==ByteCount(30)
    # verify 2 buckets
    Assert(tstore.list_unseen({}))=={ (s2,id2): ByteCount(20),
                                      (s3,id3): ByteCount(10) }
    # verify 10 bytes + 20 bytes
    with tstore.new_reader(s2,id2) as r:
        Assert(r.read(ByteCount(21)))==b'abcdefghijpoiuytrewq'
        pass
    with tstore.new_reader(s3,id3) as r:
        Assert(r.read(ByteCount(11)))==b'abcde01234'
        pass

    s4=tstore.calc_bucket_start(t1+3*Timestamp(3600))
    id4=BucketID('4')
    
    tstore.create_bucket(s4,id4)
    
    # verify 3 buckets
    Assert(tstore.list_unseen({}))=={ (s2,id2): ByteCount(20),
                                      (s3,id3): ByteCount(10),
                                      (s4,id4): ByteCount(0) }

    s5=tstore.calc_bucket_start(t1+5*Timestamp(3600))
    id5=BucketID('5')
    
    tstore.create_bucket(s5,id5)
    
    # verify 3 buckets
    Assert(tstore.list_unseen({}))=={ (s3,id3): ByteCount(10),
                                      (s4,id4): ByteCount(0),
                                      (s5,id5): ByteCount(0) }

    pass
