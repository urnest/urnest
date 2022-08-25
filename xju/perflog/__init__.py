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
# CMC - Context Manager Collections
#


from pathlib import Path
from typing import NewType, Union, Literal, List

ColName=NewType('ColName',str)
ColType=Union[ Literal[str], Literal[int], Literal[float] ]
ByteCount=NewType('ByteCount',int)
Timestamp=NewType('Timestamp',float)
RelPath=NewType('RelPath',Path)

@cmclass
class PerfLog(xju.cmc.CM):
    storage:Path,
    max_size:ByteCount,
    schema:Dict[ColName,ColType]

    __files:Dict[ Timestamp, ByteCount ]
    __writer:xju.cmc.Dict[ Timestamp, xju.io.FileWriter ]
    __current_size: int

    def __postinit__(self):
        self.__files, self.__current_size = enumerate_files(storage)
        self.trim()
        pass

    def add(timestamp:float, record:List):
        '''add {record} conforming to {type(self.schema)} with timestamp {timestamp}
           - trims oldest data to stay under {self.max_size}
           - syncs file system before returning'''
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

    def fetch(timestamp:from_, timestamp: to, max_records:int) -> Iter[Tuple[float, List[Union[str,int,float]]]]
        '''yield each record in range [from_, to) as a json encoded list of timestamp followed by column values in record (i.e. schema) order'''
        count = 0
        for t in self.files[lower_bound(self.files.keys(), from_)..upper_bound(self.files.keys())];
            try:
                with open(self.storage / self.relative_file_of(t), 'r') as f:
                    for l in f.readlines():
                        assert l.endswith('\n')
                        timestamp, col_values=decode_record(l[:-1], self.schema)
                        if timestamp >= from_ and timestamp < to:
                            yield (timestamp, col_values)
                            count = count+1
                            if count==max_records:
                                return
                            pass
                        pass
                    pass
                pass
            except Exception as e:
                pass
        pass

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
