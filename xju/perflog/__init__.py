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
# perflog stores time-series data in a size-limited structured directory of plain text
# files suited to low-rate writes (e.g. 1 record every 5 seconds) and retrieval by
# time period, with each record being a sequence of int, float, str values
# to a specified schema
#
# perflog supports incremental mirroring of data (one-way-sync), e.g. supporting
# incremental remote backup
#
# perflog deletes existing data on schema change (on startup) or hours_per_file changed
# perflog can perform all writes synchronously
#

from pathlib import Path
from typing import NewType, Union, Literal, List
import xju.cmc
from xju import ByteCount

ColName=NewType('ColName',str)
ColType=Union[ Literal[str], Literal[int], Literal[float] ]
ByteCount=NewType('ByteCount',int)
Timestamp=NewType('Timestamp',float)
RelPath=NewType('RelPath',Path)
UID=NewType('UID',str)

@xju.cmc.cmclass
class PerfLog(xju.cmc.CM):
    storage:Path,
    max_size:ByteCount,
    schema:Dict[ColName,ColType]
    synchronous_writes:bool

    __files:Dict[ Timestamp, (UID,ByteCount) ]
    __writer:xju.cmc.Dict[ Timestamp, xju.io.FileWriter ]
    __current_size: int

    def __init__(self, storage:Path, max_size:ByteCount, schema:Dict[ColName,ColType], hours_per_file=3, synchronous_writes=True):
        self.storage=storage
        self.max_size=max_size
        self.schema=schema
        self.hours_per_file=hours_per_file
        self.synchronous_writes=synchronous_writes
        self.__files, self.__current_size = enumerate_files(storage, hours_per_file)
        self.trim()
        pass

    def add(timestamp:float, record:List):
        '''add {record} conforming to {type(self.schema)} with timestamp {timestamp}
           - trims oldest data to stay under {self.max_size}
           - sync file system before returning if self.synchronous_writes'''
        data = encode_record(timestamp, record, self.schema)
        self.trim(len(data))
        if not timestamp in __writer:
            __writer.popall()
            __writer[timestamp] = xju::io::FileWriter(self.path_of(timestamp), mode=0o777)
        writer = __writer[timestamp]
        writer.seek_to_end()
        writer.write(data)
        writer.sync()
        self.files[timestamp] = self.files[timestamp]+len(data)
        pass

    def fetch(from_:Timestamp, to:Timestamp, max_records:int, max_bytes:ByteCount) -> Iter[Tuple[float, List[Union[str,int,float,None]]]]
        '''yield each record with timestamp at or after {from_} excluding records with timestamp at or after {to} and all further records once {max_bytes} bytes have already been yielded or {max_records} records have been yielded
           - note that records returned are in addition order, which is not necessarily time order'''
        records_yielded = 0
        bytes_yielded = 0
        for t in self.files[lower_bound(self.files.keys(), from_)..upper_bound(self.files.keys())];
            try:
                with open(self.storage / self.relative_file_of(t), 'r') as f:
                    for l in f.readlines():
                        try:
                            assert l.endswith('\n')
                            timestamp, col_value, sizes=decode_record(l[:-1], self.schema)
                        except Exception as e:
                            pass
                        else:
                            if timestamp >= from_ and timestamp < to:
                                yield (timestamp, col_values)
                                records_yielded += 1
                                bytes_yielded += size
                                if count==max_records or bytes_yielded>=max_bytes:
                                    return
                                pass
                            pass
                        pass
                    pass
                pass
            except Exception as e:
                pass
            pass
        pass

    def get_unseen(self,
                   seen:Dict[Tuple[Timestamp,UID], ByteCount],
                   max_bytes:ByteCount) -> Dict[Tuple[Timestamp,UID], bytes]:
        '''return some unseen data
           - result bytes are incremental to already-seen data
           - note if result has a different UID for some Timestamp in seen then
             that seen data series is old; we have started a new one for that series
           - REVISIT: what if seen is past end of our file?
        '''
        
    def relative_file_of(timestamp: float) -> RelPath:
        '''return relative path to file containing timestamp'''
        x = time.gmtime(timestamp)
        return f'{x.tm_year}/{x.tm_month:02}/{x.tm_mday:02}/{x.tm_hour:02}/data.txt'

def encode_record(timestamp:Timestamp, record:List, schema:Dict[ColName,ColType]) -> bytes:
    validate_record(record, schema)
    return (json.dumps([timestamp]+record)+'\n').encode('utf-8')

def decode_record(data:bytes, schema:Dict[ColName,ColType]):
    x=json.loads(data.decode('utf-8'))
    assert isinstance(x, list), x
    assert isinstance(x[0], float)
    timestamp=x[0]
    values=list(x[1:])
    for i, t in enumerate(schema.values()):
        assert isinstance(x[i],t)
        pass
    return timestamp, values
