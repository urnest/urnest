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
from xju.cmc.perflog import PerfLog,ColName,Recorder,Tracker

from xju.assert_ import Assert
from xju.time import Hours,now,Duration
from xju.misc import ByteCount
from xju.cmc.io import FileMode
from tempfile import TemporaryDirectory
from pathlib import Path

with TemporaryDirectory() as d_:
    d=Path(d_)
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
                    ByteCount(120),
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
    with perflog.new_recorder() as recorder:
        recorder.record(t2, [7.6,700,None,'p',None,None])
        recorder.record(t3, [17.8,1000,'peer','p',9.9,888])
        recorder.record(t3, [0,0,'peer','p',9.9,888])
        recorder.record(t4, [2.0,665,'peer','p',9.9,888])
        recorder.record(t5, [1.1,655,'peer','p',9.9,888])
        recorder.record(t6, [3.0,765,'peer','p',9.9,888])
        recorder.record(t6, [6.3,10,'peer','p',9.9,888])
        recorder.record(t7, [88,70,'peer','p',9.9,888])
        recorder.record(t8, [88,70,'peer','p',9.9,888])
        recorder.record(t9, [33,55,'peer','p',9.9,888])
        recorder.record(t10, [99.7,77,'peer','p',9.9,888])
        recorder.record(t11, [1.7,36,'peer','p',9.9,888])
        
# fetch all

# fetch some - whole hour

# fetch some covering hour plus part either side

# fetch some - max bytes limited

# fetch some - max records limited

# get unseen - seen none

# get unseen - seen some

# get unseen - see all

# get unseen - max byes limited

# tracker - write unseen, multiple buckets

# read back records

# get unseen - read failed in middle

# get unseen - decode failed in middle
