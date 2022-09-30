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
from xju.cmc.perflog import trim_trailing_partial_record,FilePosition,Timestamp
from typing import List,Tuple

import os

from xju.assert_ import Assert
from xju.time import Hours,now,Duration
from xju.misc import ByteCount
from xju.cmc.io import FileMode,FileWriter
from pathlib import Path

def verify_permissions(path:Path, mode:FileMode):
    '''verify recursively that all files in {path} have at most permissions {mode} and that files are not executable'''
    path_mode=path.stat().st_mode & 0o7777
    if path.is_dir():
        assert (path_mode & ~mode.value())==0, (path, FileMode(path_mode), mode)
        for x in path.iterdir():
            verify_permissions(x, mode)
            pass
        pass
    else:
        assert (path_mode & ~(mode.value()&0o666))==0, (path, FileMode(path_mode), mode)
        pass
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
    raise e from None

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
read_fails:List[Exception]=[]
def abort_on_read_failed(e:Exception, r=read_fails):
    r.append(e)
    raise e

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

verify_permissions(d/'perflog', FileMode(0o770))

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
    
verify_permissions(d/'perflog_mirror', FileMode(0o770))

# test trim_trailing_partial_record
data={ (BucketStart(1),BucketID('11')) : (FilePosition(3),b'fred\njock\n'),
       (BucketStart(2),BucketID('13')) : (FilePosition(3),b'anne\nsally') }
trim_trailing_partial_record(data)
Assert(data)=={
    (BucketStart(1),BucketID('11')) : (FilePosition(3),b'fred\njock\n'),
    (BucketStart(2),BucketID('13')) : (FilePosition(3),b'anne\n') }

data={}
trim_trailing_partial_record(data)
Assert(data)=={}


# corruption handling
perflog=PerfLog(d/'perflog_corrupt',
                {ColName('Tx Rate'): 'float'},
                Hours(1),
                3,
                ByteCount(10000),
                FileMode(0o700))

t1=Timestamp(3600.0)
t2,t3,t4,t5,t6,t7,t8,t9,t10,t11=[t1+Duration(_) for _ in
                                 [50, 1000, 3599,
                                  3600, 3700, 3800,
                                  2*3600, 2*3700, 2*3800,2*3900]]

r1=[0.0]
r2=[7.6]
r3=[17.8]
r4=[2.0]
r5=[1.1]
r6=[3.0]
r7=[88.2]
r8=[88.3]
r9=[33.9]
r10= [99.7]
r11= [1.7]

with perflog.new_recorder() as recorder:
    recorder.record(t1,r1)
    recorder.record(t2,r2)
    recorder.record(t3,r3)
    recorder.record(t4,r4)
    recorder.record(t5,r5)
    recorder.record(t6,r6)
    recorder.record(t7,r7)
    recorder.record(t8,r8)
    recorder.record(t9,r9)
    recorder.record(t10,r10)
    recorder.record(t11,r11)
    pass


class PermChange:
    def __init__(self,path:Path,perm:FileMode):
        self.path=path
        self.perm=perm
        pass
    def __enter__(self):
        self.orig=self.path.stat().st_mode
        self.path.chmod(self.perm.value())
        pass
    def __exit__(self,t,e,b):
        self.path.chmod(self.orig)
        pass
    pass

corruptions:List[Exception]=[]
def capture_corruptions(e:Exception,corruptions=corruptions):
    corruptions.append(e)
    pass

# read fails in the middle
dm=Path(d/'perflog_corrupt/tstore/1970/01/01/02')
fm=[ p for p in dm.iterdir() if p.name.endswith('.txt') ]
Assert(fm)==fm[0:1]
with PermChange(fm[0],FileMode(0o000)):
    Assert(list(perflog.fetch(t1,t1+Hours(4),100000,ByteCount(100000),capture_corruptions)))==[
        (t1,r1),
        (t2,r2),
        (t3,r3),
        (t4,r4),
        (t8,r8),
        (t9,r9),
        (t10,r10),
        (t11,r11),
    ]
    pass
# fetch - read failed in middle
Assert(corruptions)==corruptions[0:1]
Assert('Permission denied').isIn(str(corruptions[0]))

# get unseen - read failed in middle
del corruptions[:]
with PermChange(fm[0],FileMode(0o000)):
    Assert(list(perflog.get_some_unseen_data({},ByteCount(100000),capture_corruptions)))!=[]
    pass
Assert(corruptions)==corruptions[0:1]
Assert('Permission denied').isIn(str(corruptions[0]))

# fetch - decode failed in middle
with FileWriter(fm[0]) as f:
    f.seek_to(FilePosition(12))
    f.output.write(b'xxx')
    pass

del corruptions[:]
Assert(list(perflog.fetch(t1,t1+Hours(4),100000,ByteCount(100000),capture_corruptions)))==[
    (t1,r1),
    (t2,r2),
    (t3,r3),
    (t4,r4),
    (t5,r5),
    (t7,r7),
    (t8,r8),
    (t9,r9),
    (t10,r10),
    (t11,r11),
]
Assert(corruptions)==corruptions[0:1]
Assert('3.0').isIn(str(corruptions[0]))
Assert('Expecting value: line 1 column 2').isIn(str(corruptions[0]))

# get_some_unseen_data does not decode records
del corruptions[:]
Assert(list(perflog.get_some_unseen_data({},ByteCount(100000),capture_corruptions)))!=[]
Assert(corruptions)==[]
