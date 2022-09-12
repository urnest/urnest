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
# time period, with each record being a sequence of optional int/float/str values
# to a specified schema
#
# perflog supports incremental mirroring of data (one-way-sync), e.g. supporting
# incremental remote backup
#
# perflog can perform all writes synchronously

from pathlib import Path
from typing import NewType, Union, Literal, List, Optional
import xju.cmc
from xju.misc import ByteCount
from xju.xn import in_context,in_function_context,first_line_of as l1
from time import gmtime,struct_time
from calendar import timegm

ColName=NewType('ColName',str)
ColType=Union[ Literal[str], Literal[Optional[str]],
               Literal[int], Literal[Optional[int]],
               Literal[float],Literal[Optional[float]] ]
ByteCount=NewType('ByteCount',int)
Timestamp=NewType('Timestamp',float)
RelPath=NewType('RelPath',Path)
UID=NewType('UID',str)

@xju.cmc.cmclass
class PerfLog(xju.cmc.CM):
    '''{storage_path} size limited to {selff.max_size()} bytes/{self.max_files()} files with ''' \
        '''record schema {self.schema}, {self.hours_per_bucket()} hours per file and ''' \
        '''filecreation mode 0o{self.file_creation_mode():o} ''' \
        '''and syncrhonous_writes {self.synchronous_writes}'''
    storage_path:Path,
    schema:Dict[ColName,ColType]
    synchronous_writes:bool

    __tstore:tstore.TStore

    def hours_per_file(self):
        return self.__tstore.hours_per_bucket
    def max_size(self):
        return self.__tstore.max_bytes
    def max_files(self):
        return self.__tstore.max_buckets
    
    @overload
    def __init__(self,
                 storage_path:Path,
                 schema:Dict[ColName,ColType],
                 synchronous_writes:bool,
                 hours_per_bucket:Hours,
                 max_buckets: int,
                 max_size:ByteCount,
                 file_creation_mode:FileMode):
        '''create non-existent PerfLog with schema {schema} at {storage_path} with mode ''' \
        '''0o{file_creation_mode:o}, {hours_per_bucket} hours per bucket, max buckets ''' \
        '''{max_buckets}, max size {max_size} bytes
           - hours_per_bucket must be a factor of 24
           - raises FileExistsError if perflog exists at {storage_path}'''
        pass
    @overload
    def __init__(self,
                 storage_path:Path,
                 schema:Literal[None]=None,
                 synchronous_writes:Literal[None]=None,
                 hours_per_bucket:Literal[None]=None,
                 max_buckets:Literal[None]=None,
                 max_size:Literal[None]=None,
                 file_creation_mode:Literal[None]=None):
        '''open existing PerfLog at {storage_path} reading attributes from its perflog.json file
           - raises FileNotFoundError if PerfLog does not exist'''
        pass
    def __init__(self,
                 storage_path:Path,
                 schema=Optional[Dict[ColName,ColType]]=None,
                 synchronous_writes:Optional[bool]=None,
                 hours_per_bucket:Optional[Hours]=None,
                 max_buckets:Optiona[int]=None,
                 max_size:Optional[ByteCount]=None,
                 file_creation_mode:Optional[FileMode]=None):
        self.storage_path = storage_path
        if (isinstance(schema,Dict[ColName,ColType]) and
            isinstance(synchronous_writes,bool) and
            isinstance(hours_per_bucket,Hours) and
            isinstance(max_buckets, int) and
            isinstance(max_size,ByteCount) and
            isinstance(file_creation_mode,FileMode)):
            try:
                self.schema=schema
                self.synchronous_writes=synchronous_writes
                os.makedirs(storage_path, file_creation_mode)
                write_attrs(storage_path / 'perflog.json', schema, synchronous_writes)
                try:
                    self.__tstore=tstore.TStore(storage_path / 'tstore',
                                                hours_per_bucket,
                                                max_buckets,
                                                max_size,
                                                file_creation_mode)
                finally:
                    os.unlink(storage_path / 'perflog.json')
                    pass
                pass
            except Exception as e:
                raise in_context(
                    '''create non-existent PerfLog with schema {schema} at {storage_path} with ''' \
                    '''mode 0o{file_creation_mode:o}, {hours_per_bucket} hours per bucket, max ''' \
                    '''buckets {max_buckets}, max size {max_size} bytes'''.format(**vars())) from None
            pass
        else:
            try:
                self.__tstore=tstore.TStore(storage_path / 'tstore')
                self.schema,self.synchronous_writes=read_attrs(storage_path / 'perflog.json')
            except Exception as e:
                raise in_context(
                    '''open existing PerfLog at {storage_path} reading attributes from its perflog.json file'''.format(**vars())) from None
            pass
        pass

    def __str__(self) -> str:
        return l1(PerfLog.__doc__).format(**vars())
    
    def fetch(begin:Timestamp,
              end:Timestamp,
              max_records:int,
              max_bytes:ByteCount,
              corruption_handler:Callable[[Exception],Any]) -> Iter[
                  Tuple[float, List[Union[str,int,float,None]]]]
        '''yield each record of PerfLog {self} with timestamp at or after {begin} excluding records with timestamp at or after {end} and all further records once {max_bytes} bytes have already been yielded or {max_records} records have been yielded
           - note that records returned are in addition order, which is not necessarily time order'''
        try:
            records_yielded = 0
            bytes_yielded = 0
            for bucket_start,bucket_id in self.__tstore.get_buckets_of(self, begin, end):
                with tstore.Reader(self.__tstore, (bucket_start,bucket_id)) as r:
                    for l in read_lines(r):
                        try:
                            if not l.endswith(b'\n'):
                                raise Exception(f'{l!r} does not end in \\n')
                            timestamp, col_values, size=decode_timestamped_record(l[:-1], schema)
                        except Exception as e:
                            corruption_handler(e)
                        else:
                            if timestamp >= begin and timestamp < end:
                                yield (timestamp, col_values)
                                records_yielded += 1
                                bytes_yielded += size
                                if records_yielded==max_records or bytes_yielded>=max_bytes:
                                    return
                                pass
                            pass
                        pass
                    pass
                except Exception as e:
                    corruption_handler(e)
                pass
            pass
        except Exception:
            raise in_function_context(PerfLog.fetch,vars()) from None
        pass

    def get_some_unseen_data(
            self,
            seen:Dict[Tuple[BucketStart,BucketID], ByteCount],
            max_bytes:ByteCount,
            read_failed:Callable[[BucketStart,BucketID,Exception],Any]
    ) -> Dict[Tuple[BucketStart,BucketID], (ByteCount,bytes)] #position,bytes
        '''return up to {max_bytes} bytes of unseen data from {self} having seen {seen}
           - new data ends on record boundary i.e. always returns complete records
           - new data might truncate seen data
           - new data size 0 means bucket does not exist (any longer)
           - read failures are passed to read_failed, skipping that file if read_failed returns
        '''
        try:
            result={}
            result_size=0
            for bucket, size in self.__tstore.list_unseen(seen).items():
                bucket_start, bucket_id=*bucket
                seen_size=seen.get(bucket,ByteCount(0))
                if seen_size>size:
                    seen_size=0
                    pass
                with tstore.Reader(self.__tstore, bucket) as r:
                    try:
                        if seen_size>0:
                            r.seek_to(seen_size-1)
                            b=r.read(1)
                            if b!=b'\n':
                                seen_size=0
                                pass
                            pass
                        read_size=size-seen_size
                        if read_size>max_bytes-result_size:
                            read_size=max_bytes-result_size
                            pass
                        r.seek_to(seen_size)
                        data=r.read(read_size)
                        result[bucket]=(seen_size,data)
                        result_size+=len(data)
                    except Exception as e:
                        read_failed(bucket_start,bucket_id,e)
                        pass
                    pass
                if result_size=max_bytes:
                    break
                pass
            return result
        except Exception as e:
            raise in_function_context(PerfLog.get_some_unseen_data,vars()) from None
        pass
    pass

class Recorder:
    '''record appender for PerfLog {self.perflog}'''
    perflog;PerfLog
    __writers:xju.cmc.Dict[ Tuple[BucketStart,BucketID], tstore.Writer ]

    def __init__(self, perflog:PerfLog):
        self.perflog=perflog
        pass

    def record(timestamp:Timestamp, record:List):
        '''record in PerfLog {self.perflog} at timestamp {timestamp} record {record!r}'''
        try:
            bucket_start=self.perflog.tstore.calc_bucket_start(timestamp)
            encoded_record=encode_timestamped_record(timestamp-bucket_start,
                                                     record,
                                                     self.perflog.schema)
            self.perflog.tstore.make_room_for(len(encoded_record))
            try:
                bucket_id=self.perflog.__tstore.get_bucket(bucket_start)
            except tstore.NoSuchBucket:
                bucket_id=BucketID(f'{time.time():.06f}')
                self.perflog.__tstore.create_bucket(bucket_start,bucket_id)
                pass
            if (bucket_start,bucket_id) not in self.__writers:
                # only keep one writer because most records will go into same or new bucket
                self.__writers.pop_all()
                self.__writers[(bucket_start,bucket_id)]=tstore.Writer(
                    self.perflog.__tstore,
                    bucket_start,
                    bucket_id)
                pass
            self.__writers[(bucket_start,bucket_id)].append(encoded_record)
        except Exception:
            raise in_function_context(Recorder.record,vars())
        pass
    pass


class Tracker:
    '''tracker updating PerfLog {self.perlog} with unseen data
       - tracker assumes {self.perflog} has same max bytes and max buckets as source'''
    perflog:PerfLog

    def __init__(self,perflog:Perflog):
        self.perflog=perflog
        pass

    def write_unseen_data(self, data:Dict[Tuple[BucketStart,BucketID], (ByteCount,bytes)]):
        '''write unseen data to PerfLog {self.perflog}'''
        try:
            for bucket_start, bucket_id in data:
                position,data=data[(bucket_start,bucket_id)]
                try:
                    try:
                        existing_id=self.perflog.__tstore.get_bucket(bucket_start)
                        if existing_id != bucket_id:
                            self.perflog.__tstore.delete_bucket(bucket_start,existing_id)
                            self.perflog.__tstore.get_bucket(bucket_start,bucket_id) # will raise
                            pass
                    except NoSuchBucket:
                        self.perflog.create_bucket(bucket_start,bucket_id)
                        pass
                    if position==0 and len(data)==0:
                        self.perflog.__tstore.delete_bucket(bucket_start,bucket_id)
                    else:
                        # there is room (as long as self.perflog has same max_bytes, max_buckets
                        # as source)
                        with tstore.Writer(self.perflog.tstore,bucket_start,bucket_id) as writer:
                            writer.seek_to(position)
                            writer.write(data)
                            writer.truncate()
                            pass
                        pass
                    pass
                except Exception:
                    raise in_context(f'write {len(data)} bytes of data at position {position} '
                                     f'of bucket with start {bucket_start} and id {bucket_id}')
                pass
            pass
        except Exception:
            raise in_function_context(Tracker.write_unseen_data,vars()) from None
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
        raise in_function_context(validate_record,vars()) from None
    pass

def encode_timestampeed_record(time_delta:float, record:List, schema:Dict[ColName,ColType]) -> bytes:
    '''encode record {record} and time delta {time_delta} assuming record conforms to schema {schema}'''
    try:
        assert time_delta>=0
        return (json.dumps([timestamp]+validate_record(record, schema))+'\n').encode('utf-8')
    except:
        raise in_function_context(encode_timestampeed_record,vars()) from None
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
        raise in_function_context(decode_timestamped_record,vars()) from None
    pass

    
def read_lines(r:Reader) -> Iter[bytes]:
    r'''read all \n-separated lines from PerLog reader {r}
        - yield one record at a time, including any terminating "\n"
          * where the file does not end in \n the last record yielded will have no "\n"'''
    try:
        buffered=io.BytesIO()
        next:bytes=r.read(1024)
        while len(next):
            recs=next.split(b'\n')
            while len(recs)>1:
                buffered.write(recs[0])
                buffered.write(b'\n')
                yield buffered.getvalue()
                buffered=io.BytesIO()
                recs=recs[1:]
                pass
            if len(next):
                buffered.write(recs[0])
                yield buffered.getvalue()
                pass
            pass
        pass
    except Exception as e:
        raise in_function_context(__read_records,vars()) from None
    pass
                
                
attrs_schema=xju.jsonschema.Schema({
    'schema':[ [str,OneOf('str','int','float','(str)','(int)','(float)')] ],
    'synchronous_writes':bool
})

str_col_type:Dict[str,ColType]={
    'str':str,
    'int':int,
    'float':float,
    '(str)':Optional[str],
    '(int)':Optional[int],
    '(float)':Optional[float]
}
col_type_str={ t:s for s,t in str_col_type.items() }

PERFLOG_ATTRS='perflog.json'

def read_attrs(storage_path:Path,
               attrs_file=PERFLOG_ATTRS) -> Tuple[Dict[ColName,ColType],  # schema
                                                    bool]:                  # synchronous_writes
    '''read PerfLog attrs from {storage_path}/{attrs_file}'''
    try:
        with xju.io.cmc.FileReader(storage_path / attrs_file) as f:
            attrs=attrs_schema.validate(json.loads(f.read()))
            if (isinstance(attrs,dict) and
                isinstance(_schema:=attrs['schema'],list) and
                isinstance(synchronous_writes:=attrs['synchronous_writes'],bool)):
                schema={ColName(col_name): str_col_type[type_spec] for col_name,type_spec in _schema}
                return schema,synchronous_writes
            assert False, f'should not be here: {attrs_} was validated against {attrs_schema}'
    except Exception as e:
        raise in_function_context(read_attrs,vars()) from None
    pass

def write_attrs(storage_path:Path, schema:Dict[ColName,ColType],
                synchronous_writes:bool,
                attrs_file=PERFLOG_ATTRS) -> ByteCount:
    '''write PerfLog attrs schema {schema}, synchronous_writes {synchronous_writes} to ''' \
    '''{storage_path}/{attrs_file}
       - returns number of bytes written'''
    try:
        x=attrs_schema.validate({
            'schema':[ [col_name,col_type_str[col_type]] for col_name, col_type in schema.items()],
            'synchronous_writes':synchronous_writes
        })
        y=json.dumps(x).encode('utf-8')
        with xju.io.cmc.FileWriter(storage_path / 'perflog.json',
                                   mode=file_creation_mode,
                                   must_not_exist=True) as f:
            f.write(y)
            pass
        return len(y)
    except Exception as e:
        raise in_function_context(write_attrs,vars()) from None
    pass
