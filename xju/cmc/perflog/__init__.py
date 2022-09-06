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
from xju.xn import inContext
from time import gmtime,struct_time
from calendar import timegm

ColName=NewType('ColName',str)
ColType=Union[ Literal[str], Literal[int], Literal[float] ]
ByteCount=NewType('ByteCount',int)
Timestamp=NewType('Timestamp',float)
RelPath=NewType('RelPath',Path)
UID=NewType('UID',str)

@xju.cmc.cmclass
class PerfLog(xju.cmc.CM):
    '''{storage_path} size limited to {max_size} bytes with record schema {self.schema}, '''
    '''{self.hours_per_file} hours-per-file, file creation mode {self.file_creation_mode} '''
    '''and syncrhonous_writes {self.synchronous_writes}'''
    storage_path:Path,
    max_size:ByteCount,
    schema:Dict[ColName,ColType]
    hours_per_file:int
    synchronous_writes:bool
    file_creation_mode:int

    __files:Dict[ Timestamp, __File ]
    __writers:xju.cmc.Dict[ Timestamp, xju.io.FileWriter ]
    __current_size: ByteCount

    def __init__(self, storage_path:Path,
                 max_size:ByteCount,
                 schema:Dict[ColName,ColType],
                 hours_per_file:int=3,
                 synchronous_writes:bool=True,
                 file_creation_mode:int=0x666):
        self.storage_path=storage_path
        self.max_size=max_size
        self.schema=schema
        self.hours_per_file=hours_per_file
        self.synchronous_writes=synchronous_writes
        self.file_creation_mode=file_creation_mode

    def xju_cmc_post_enter(self):
        '''create/open PerfLog {self}
           - clears existing data if {self.schema} or {self.hours_per_file} have changed
           - trims existing data to self.{max_size}'''
        try:
            __private.apply_hours_per_file(storage_path,hours_per_file)
            __private.apply_schema(storage_path,schema)
            self.__files, self.__current_size = __private.enumerate_files(storage, hours_per_file)
            self.__trim()
        except Exception as e:
            raise inContext(l1(PerfLog.__enter__.__doc__).format(**vars())) from None
        pass

    def __str__(self) -> str:
        return l1(PerfLog.__doc__).format(**vars())
    
    def add(timestamp:float, record:List):
        '''add {record} with timestamp {timestamp}
           - verifies record conforms to {self.schema}
           - trims oldest data to stay under {self.max_size}
           - sync file system before returning if self.synchronous_writes'''
        try:
            file = self.__get_file_of(timestamp)
            data = encode_timestamped_record(timestamp-file.from_, record, self.schema)
            self.__trim(headroom=len(data))
            write=self.__get_writer(timestamp)
            writer.seek(writer.size())
            writer.output.write(data)
            if self.synchronous_writes:
                writer.sync()
                pass
            self.get_or_create_file(timestamp).size+=len(data)
            self.__current_size+=len(data)
        except Exception:
            raise inContext(l1(PerfLog.add.__doc__).format(**vars())) from None
        pass

    def fetch(from_:Timestamp,
              to:Timestamp,
              max_records:int,
              max_bytes:ByteCount,
              corruption_handler:Callable[[Exception],Any]) -> Iter[
                  Tuple[float, List[Union[str,int,float,None]]]]
        '''yield each record of PerfLog {self} with timestamp at or after {from_} excluding records with timestamp at or after {to} and all further records once {max_bytes} bytes have already been yielded or {max_records} records have been yielded
           - note that records returned are in addition order, which is not necessarily time order'''
        try:
            return __private.fetch(from_,to,max_records,max_bytes,
                                   self.__files,
                                   self.hours_per_file,
                                   corruption_handler)
        except Exception:
            raise inContext(l1(PerfLog.fetch.__doc__).format(**vars())) from None
        pass

    def get_some_unseen_data(
            self,
            seen:Dict[Tuple[Timestamp,UID], ByteCount],
            max_bytes:ByteCount,
            read_failed:Callable[[Timestamp,UID,Exception],Any]
    ) -> Tuple[Dict[Tuple[Timestamp,UID], bytes], #new_data
               List[Tuple[Timestamp,UID]]]:       #corrupt_files
        '''return up to {max_bytes} bytes of unseen data from {self} given have seen {seen}
           - new_data bytes end on record boundary i.e. always returns complete records
           - new_data bytes are incremental to corresponding seen if UIDs match, otherwise
             (if new_data has a different UID for same Timestamp) then
              that seen data series is old; we have started a new one for that series)
           - corrupt_files lists seen files that are corrupt (too large or don't end
             on a record boundary); delete them for subsequent refetch
           - read failures are passed to read_failed, skipping that file if read_failed returns
        '''
        try:
            new_data={}
            corrupt_files=[]
            new_data_size:ByteCount=0
            for from_, file in self.__files:
                if new_data_size >= max_bytes:
                    break
                start_at=seen.get( (from_,file.uid), 0)
                limit=min(max_bytes-new_data_size, file.size-start_at)
                if start_at > file.size:
                    corrupt_files.append( (from_,file.uid) )
                    continue
                if start_at == file.size:
                    continue
                try:
                    with FileReader(file.path) as f:
                        if start_at > 0:
                            f.seek_to(start_at-1)
                            b = f.read(1)
                            if b != b'\n':
                                corrupt_files.append( (from_.file.uid) )
                                continue
                            pass
                        data=f.read(limit)
                        data=data[:data.rfind(b'\n')+1]
                        new_data[(from_,file.uid)]=data
                        new_data_size+=len(data)
                        pass
                except Exception as e:
                    read_failed(from_,file.uid,e)
                    pass
                pass
            return new_data, corrupt_files
        except Exception as e:
            raise inContext(l1(PerfLog.get_some_unseen_data.__doc__).format(**vars()))
        pass
    
    def __get_writer(self, timestamp:Timestamp) -> xju::io:FileWriter:
        '''get PerfLog {self} writer for file of record with timestamp {timestamp}
           - creates any missing file and parent directories'''
        try:
            file, from_ = self.__get_file_of(timstamp)
            x = self.__writers.get(from_,None)
            if x is not None:
                return x
            self.__writers.popall() #only keep 1 writer
            return self.__writers.setdefault(
                from_,xju::io::FileWriter(f.path,mode=self.file_creation_mode))
        except Exception as e:
            raise inContext(l1(PerfLog.__get_writer.__doc__)).format(**vars()) from None
        pass

def validate_record(record:List, schema:schema:Dict[ColName,ColType]) -> List:
    '''validate record {record} against schema {schema}
       - returns record'''
    try:
        if len(record) != len(schema):
            raise Exception(f'schema expects {len(schema)} values but record has {len(record)}')
        for value,s in zip(record, schema.items()):
            col_name, col_type = s
            if not r.isinstance(col_type):
                raise Exception(
                    f'value {value!r} for {col_name} has type {type(r)} which is not a {col_type}')
            pass
    except Exception as e:
        raise inContext(l1(validate_record.__doc__).format(**vars())) from None
    pass

def encode_timestampeed_record(time_delta:float, record:List, schema:Dict[ColName,ColType]) -> bytes:
    '''encode record {record} and time delta {time_delta} assuming record conforms to schema {schema}'''
    try:
        assert time_delta>=0
        return (json.dumps([timestamp]+validate_record(record, schema))+'\n').encode('utf-8')
    except:
        raise inContext(l1(encode_record.__doc__).format(**vars())) from None
    pass

def decode_timestamped_record(data:bytes, schema:Dict[ColName,ColType]) -> Tuple[
        float, #time_delta
        List[Union[str,int,float,None]]]: #record
    '''decode time delta and record from {data!r} assuming record conforms to schema {schema}'''
    try:
        assert data.endswith(b'\n')
        x=json.loads(data.decode('utf-8')[:-1])
        if isinstance(x, list):
            if isinstance(x[0], float):
                time_delta=x[0]
                assert time_delta>0, time_delta
                values=x[1:]
                for i, t in enumerate(schema.values()):
                    assert isinstance(x[i],t), (i, x[i], t)
                    pass
                return time_delta, values
            else:
                raise Exception(f'time delta {x[0]} is of type {type(x[0])}, which is not a float')
            pass
        else:
            raise Exception(f'{x} is of type {type(x)}, which is not a list')
            pass
    except Exception as e:
        raise inContext(l1(decode_timestamped_record.__doc__).format(**vars())) from None
    pass

@dataclass
class __File:
    from_:Timestamp
    uid:UID,
    path:Path
    size:ByteCount
    pass

def __bucket_of(hours_per_file:int, timestamp:Timestamp) -> struct_time:
    x = time.gmtime(timestamp)
    h = int(x.tm_hour/hours_per_file)*hours_per_file
    return struct_time(x.tm_year,x.tm_mon,x.tm_mday,h,0,0,0,0,0)

def __bucket_path(storage_path:Path, bucket:struct_time) -> Path:
    return storage_path / f'{bucket.tm_year}'/f'{bucket.tm_month:02}'/f'{bucket.tm_mday:02}'/f'{bucket.tm_hour:02}'
    
def __home_of(storage_path:Path, hours_per_file:int, timestamp:Timestamp) -> Tuple[Path,Timestamp]:
    '''directory containing record with timestamp {timestamp} and its "from" timestmap'''
    bucket = __bucket_of(hours_per_file:Path,timestamp)
    return (__bucket_path(storage_path,bucket),timegm(bucket))

def __get_file_of(storage_path:Path,
                  files::Dict[ Timestamp, __File ],
                  hours_per_file:int,
                  timestamp:Timestamp) -> __File:
    '''get {storage_path} file of record with timestamp {timestamp}
           - creates any missing parent directories but not file itself'''
    try:
        bucket = __bucket_of(hours_per_file,timestamp)
        from_ = timegm(bucket)
        f = files.get(from_,None)
        if isinstance(f, __File):
            return f
        dir=__bucket_path(storage_path,bucket)
        os.makedirs(dir)
        while True:
            uid=uuid4()
            path=dir / f'{uid}.txt'
            if not path.exists():
                break
            pass
        return files.setdefault(from_,__File(from_,uid,path,0))
    except Exception as e:
        raise inContext(l1(__get_file_of.__doc__).format(**vars())) from None
    pass
    
def __fetch(from_:Timestamp,
            to:Timestamp,
            max_records:int,
            max_bytes:ByteCount,
            files:Dict[ Timestamp, __File ],
            hours_per_file:int,
            schema:Dict[ColName,ColType],
            corruption_handler:Callable[[Exception],Any]) -> Iter[
                Tuple[float, List[Union[str,int,float,None]]]]:
    records_yielded = 0
    bytes_yielded = 0
    sorted_timestamps=sorted(list(files.keys()))
    bucket_from=timegm(bucket_of(hours_per_file,from_))
    bucket_to=timegm(bucket_to(hours_per_file,to))
    for t in sorted_timestamps[lower_bound(sorted_timestamps,bucket_from):
                               upper_bound(sorted_timestamps,bucket_to)]:
        try:
            with open(files[t].path, 'r') as f:
                for l in f.readlines():
                    try:
                        assert l.endswith('\n')
                        timestamp, col_value, sizes=decode_record(l[:-1], schema)
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

def __trim(files::Dict[ Timestamp, __File ],
           current_size:ByteCount, ensure_below:ByteCount):
    '''trim {self} storage currently {current_size} bytes to ensure it is below {ensure_below}'''
    try:
        if current_size > ensure_below:
            for from_, file in sorted(list(self.__files.items())):
                if from_ in self.__writers:
                    self.__writers.pop(from_)
                    pass
                file.path.unlink()
                self.__current_size -= file.size
                self.__files.pop(from_)
                if self.__current_size + headroom <= self.max_bytes:
                    return
                pass
            pass
        pass
    except Exception as e:
        raise inContext(l1(PerfLog.__doc__).format(**vars())) from None
    pass
pass

