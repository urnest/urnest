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
from xju.cmc.perflog import PerfLog,ColName,Recorder,Tracker,BucketStart,BucketID
from typing import List,Tuple

import os

from xju.assert_ import Assert
from xju.time import Hours,now,Duration
from xju.misc import ByteCount
from xju.cmc.io import FileMode
from pathlib import Path

def verify_permissions(path:Path, mode:FileMode):
    '''verify recursively that all files in {path} have at most permissions {mode} and that files are not executable'''
    # REVISIT
    pass

d=Path(os.getcwd())
# create

perflog=PerfLog(d/'perflog',
                {ColName('Tx Rate'): 'float',
                 ColName('Tx Bytes'): 'int',
                 ColName('Peer'): '(str)',
                 ColName('Protocol'): 'str',
                 ColName('mag'): '(float)',
                 ColName('Peer Id'):'(int)'},
                Hours(2),
                3,
                ByteCount(1200),
                FileMode(0o700))

# create - exists
try:
    x=PerfLog(d/'perflog',
              {ColName('Tx Rate'): 'float',
               ColName('Tx Bytes'): 'int',
               ColName('Peer'): '(str)',
               ColName('Protocol'): 'str',
               ColName('mag'): '(float)',
               ColName('Peer Id'):'(int)'},
              Hours(2),
              3,
              ByteCount(120),
              FileMode(0o700))
except FileExistsError as e:
    Assert(f"File exists: '{d}/perflog'").isIn(str(e))
else:
    assert False,x
    pass


# fetch (nothing)
t1=now()-Hours(10)
t20=now()+Hours(10)

def abort_on_corruption(e:Exception):
    raise e

Assert(list(perflog.fetch(t1,t20,1,ByteCount(1),abort_on_corruption)))==[]

# record 10 records over 3 hours
t2=t1+Hours(10)
t3,t4,t5,t6,t7,t8,t9,t10,t11=[t2+Duration(_) for _ in
                              [50, 1000, 3599,
                               3600, 3700, 3800,
                               2*3600, 2*3700, 2*3800]]


recorder:Recorder
r2=[7.6,700,None,'p',None,None]
r3=[17.8,1000,'peer','p',9.9,888]
r4=[2.0,665,'peer','p',9.9,888]
r5=[1.1,655,'peer','p',9.9,888]
r6=[3.0,765,'peer','p',9.9,888]
r6=[6.3,10,'peer','p',9.9,888]
r7=[88,70,'peer','p',9.9,888]
r8=[88,70,'peer','p',9.9,888]
r9=[33,55,'peer','p',9.9,888]
r10= [99.7,77,'peer','p',9.9,888]
r11= [1.7,36,'peer','p',9.9,888]

with perflog.new_recorder() as recorder:
    recorder.record(t2, r2)
    recorder.record(t3, r3)
    recorder.record(t4, r4)
    recorder.record(t5, r5)
    recorder.record(t6, r6)
    recorder.record(t7, r7)
    recorder.record(t8, r8)
    recorder.record(t9, r9)
    recorder.record(t10, r10)
    recorder.record(t11, r11)
    pass

verify_permissions(d/'perflog', FileMode(0o700))

# fetch all

Assert(list(perflog.fetch(t1,t20,100000,ByteCount(100000),abort_on_corruption)))==[
    (t2,r2),
    (t3,r3),
    (t4,r4),
    (t5,r5),
    (t6,r6),
    (t7,r7),
    (t8,r8),
    (t9,r9),
    (t10,r10),
    (t11,r11),
]
    
# fetch some - whole hour
Assert(list(perflog.fetch(t2,t2+Hours(1),100000,ByteCount(100000),abort_on_corruption)))==[
    (t2,r2),
    (t3,r3),
    (t4,r4),
    (t5,r5),
]
    

# fetch some covering hour plus part either side
# fetch some - whole hour
Assert(list(perflog.fetch(t2+Duration(1500),t2+Duration(2*3700),100000,ByteCount(100000),abort_on_corruption)))==[
    (t5,r5),
    (t6,r6),
    (t7,r7),
    (t8,r8),
    (t9,r9),
]
    


# fetch some - max bytes limited
Assert(list(perflog.fetch(t1,t20,100000,ByteCount(200),abort_on_corruption)))==[
    (t2,r2),
    (t3,r3),
    (t4,r4),
    (t5,r5),
]
    

# fetch some - max records limited
Assert(list(perflog.fetch(t1,t20,2,ByteCount(100000),abort_on_corruption)))==[
    (t2,r2),
    (t3,r3),
]
    

# get unseen - seen none
read_fails:List[Tuple[BucketStart,BucketID,Exception]]=[]
def abort_on_read_failed(bucket_start:BucketStart, bucket_id:BucketID, e:Exception, r=read_fails):
    r.append( (bucket_start,bucket_id,e) )
    pass

perflog_mirror=PerfLog(d/'perflog_mirror',
                       perflog.schema,
                       perflog.hours_per_bucket,
                       perflog.max_buckets,
                       perflog.max_size,
                       FileMode(0o770))

tracker:Tracker
tracker=perflog_mirror.new_tracker()
while True:
    unseen=perflog.get_some_unseen_data(tracker.get_seen(),ByteCount(256),abort_on_read_failed)
    if unseen=={}:
        break
    tracker.write_unseen_data(unseen)
    pass

verify_permissions(d/'perflog', FileMode(0o600))

# read back records
Assert(list(perflog_mirror.fetch(t1,t20,100000,ByteCount(100000),abort_on_corruption)))==[
    (t2,r2),
    (t3,r3),
    (t4,r4),
    (t5,r5),
    (t6,r6),
    (t7,r7),
    (t8,r8),
    (t9,r9),
    (t10,r10),
    (t11,r11),
]
    

# re-open read back records
perflog_mirror=PerfLog(d/'perflog_mirror')

Assert(list(perflog_mirror.fetch(t1,t20,100000,ByteCount(100000),abort_on_corruption)))==[
    (t2,r2),
    (t3,r3),
    (t4,r4),
    (t5,r5),
    (t6,r6),
    (t7,r7),
    (t8,r8),
    (t9,r9),
    (t10,r10),
    (t11,r11),
]
    

# test trim_trailing_partial_record

# get unseen - read failed in middle

# get unseen - decode failed in middle

# test mirroring to zip
