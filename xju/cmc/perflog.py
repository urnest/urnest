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
# time period, with each record being a sequence of optional int/float/str/bool/json values
# to a specified schema.
#
# perflog supports incremental mirroring of data (one-way-sync), e.g. supporting
# incremental remote backup/mirroring
#
# Classes PerfLog, Recorder and Tracker form the primary interface.
#

from pathlib import Path
from typing import Union, Literal,List,Optional,Dict,Tuple,overload,Callable,Any,Generator,cast
from xju.misc import ByteCount
from xju.xn import in_context,in_function_context,first_line_of as l1
from xju.cmc.tstore import TStore,Reader,Writer,BucketStart,BucketID,NoSuchBucket,BucketExists
from xju.cmc.io import FilePosition,FileMode
from xju.time import Timestamp,now,Hours,Duration
from xju.cmc import cmclass,CM,Dict as CMDict
from xju.cmc.io import FileReader,FileWriter
from xju.newtype import Str
from xju.assert_ import Assert
from xju import jsonschema
from xju.jsonschema import OneOf
import os
import json
import io
import shutil

class ColNameType:pass
class ColName(Str[ColNameType]):pass

ColType=Literal[
    'str','int','float','bool',          # not allowed to be None
    '(str)','(int)','(float)','(bool)',  # optional, i.e. value can be None
    'json'                               # any json-ifiable value
]

# mypy can't do isinstance on instance of generic (even though all types
# involved are concrete; workaround is to use a concrete subclass
class Writers(CMDict[ Tuple[BucketStart,BucketID], Writer ]):pass

@cmclass
class Recorder(CM):
    '''recorder recording records to PerfLog {self.storage_path}'''
    storage_path:Path
    schema:Dict[ColName,ColType]

    __tstore:TStore
    __writers:Writers

    def __init__(self,
                 storage_path:Path,
                 schema:Dict[ColName,ColType],
                 tstore:TStore):
        self.storage_path=storage_path
        self.schema=schema
        self.__tstore=tstore
        self.__writers=Writers()
        pass

    def __str__(self):
        return l1(Recorder.__doc__).format(**vars())

    def record(self,timestamp:Timestamp, record:List):
        '''{self} record at timestamp {timestamp} record {record!r}'''
        try:
            bucket_start=self.__tstore.calc_bucket_start(timestamp)
            encoded_record=encode_timestamped_record(timestamp-Timestamp(bucket_start.value()),
                                                     record,
                                                     self.schema)
            self.__tstore.make_room_for(ByteCount(len(encoded_record)))
            try:
                bucket_id=self.__tstore.get_bucket(bucket_start)
            except NoSuchBucket:
                t=now()
                bucket_id=BucketID(f'{t:0.06f}')
                self.__tstore.create_bucket(bucket_start,bucket_id)
                pass
            if (bucket_start,bucket_id) not in self.__writers:
                # only keep one writer because most records will go into same or new bucket
                self.__writers.clear()
                self.__writers[(bucket_start,bucket_id)]=self.__tstore.new_writer(
                    bucket_start,
                    bucket_id)
                pass
            self.__writers[(bucket_start,bucket_id)].append(encoded_record)
        except Exception:
            raise in_function_context(Recorder.record,vars())
        pass
    pass


class Tracker:
    '''tracker updating PerfLog {self.storage_path} with unseen data
       - tracker assumes PerfLog has same max bytes and max buckets as source'''
    storage_path:Path
    schema:Dict[ColName,ColType]

    __tstore:TStore

    def __init__(self,
                 storage_path:Path,
                 tstore:TStore):
        self.storage_path=storage_path
        self.__tstore=tstore
        pass

    def __str__(self):
        return l1(Tracker.__doc__).format(**vars())

    def get_seen(self) -> Dict[Tuple[BucketStart,BucketID], ByteCount]:
        return self.__tstore.get_bucket_sizes()

    def write_unseen_data(self, unseen:Dict[Tuple[BucketStart,BucketID],
                                            Tuple[FilePosition,bytes]]):
        '''{self} write unseen data to PerfLog {self.storage_path}
           - caller must ensure that unseen data with non-zero position matches
             current data size for that bucket
           - caller must ensure that underlying tstore max_size, max_buckets and hours_per_bucket
             matches that of the source data'''
        try:
            writer:Writer
            for bucket_start, bucket_id in unseen:
                position,data=unseen[(bucket_start,bucket_id)]
                data_len=len(data)
                try:
                    try:
                        existing_id=self.__tstore.get_bucket(bucket_start)
                        if existing_id != bucket_id:
                            self.__tstore.delete_bucket(bucket_start,existing_id)
                            self.__tstore.get_bucket(bucket_start) # will raise
                    except NoSuchBucket:
                        self.__tstore.create_bucket(bucket_start,bucket_id)
                        pass
                    if position==FilePosition(0):
                        self.__tstore.delete_bucket(bucket_start,bucket_id)
                    if len(data):
                        try:
                            self.__tstore.create_bucket(bucket_start,bucket_id)
                        except BucketExists:
                            pass
                        # there is room (as long as __tstore has same max_bytes, max_buckets
                        # as source)
                        with self.__tstore.new_writer(bucket_start,bucket_id) as writer:
                            Assert(FilePosition(0)+writer.size())==position
                            writer.append(data)
                        pass
                    pass
                except Exception:
                    raise in_context(f'write {data_len} bytes of data at position {position} '
                                     f'of bucket with start {bucket_start} and id {bucket_id}')
                pass
            pass
        except Exception:
            raise in_function_context(Tracker.write_unseen_data,vars()) from None
        pass


class PerfLog:
    '''{self.storage_path} size limited to {self.max_size} bytes/{self.max_buckets} files with ''' \
        '''record schema {self.schema}, {self.hours_per_bucket} hours per file and ''' \
        '''filecreation mode {self.file_creation_mode}'''
    storage_path:Path
    schema:Dict[ColName,ColType]
    hours_per_bucket:Hours
    max_size:ByteCount
    max_buckets:int
    file_creation_mode:FileMode
    
    __tstore:TStore

    @overload
    def __init__(self,
                 storage_path:Path,
                 schema:Dict[ColName,ColType],
                 hours_per_bucket:Hours,
                 max_buckets: int,
                 max_size:ByteCount,
                 file_creation_mode:FileMode):
        '''create non-existent PerfLog with schema {schema} at {storage_path} with mode ''' \
        '''{file_creation_mode}, {hours_per_bucket} hours per bucket, max buckets ''' \
        '''{max_buckets}, max size {max_size} bytes
           - hours_per_bucket must be a factor of 24
           - raises FileExistsError if perflog exists at {storage_path}'''

    @overload
    def __init__(self,storage_path:Path):
        '''open existing PerfLog at {storage_path} reading attributes from its perflog.json file
           - raises FileNotFoundError if PerfLog does not exist'''

    def __init__(self,
                 storage_path:Path,
                 schema:Optional[Dict[ColName,ColType]]=None,
                 hours_per_bucket:Optional[Hours]=None,
                 max_buckets:Optional[int]=None,
                 max_size:Optional[ByteCount]=None,
                 file_creation_mode:Optional[FileMode]=None):
        self.storage_path = storage_path
        if (isinstance(schema,dict) and
            isinstance(hours_per_bucket,Hours) and
            isinstance(max_buckets, int) and
            isinstance(max_size,ByteCount) and
            isinstance(file_creation_mode,FileMode)):
            try:
                self.schema=schema
                self.hours_per_bucket=hours_per_bucket
                self.max_size=max_size
                self.max_buckets=max_buckets
                self.file_creation_mode=file_creation_mode
                storage_path.mkdir(mode=file_creation_mode.value())
                try:
                    write_attrs(storage_path, schema, file_creation_mode)
                    self.__tstore=TStore(storage_path / 'tstore',
                                         hours_per_bucket,
                                         max_buckets,
                                         max_size,
                                         file_creation_mode)
                except Exception:
                    shutil.rmtree(storage_path)
                    raise
                pass
            except Exception as e:
                raise in_context(
                    '''create non-existent PerfLog with schema {schema} at {storage_path} with ''' \
                    '''mode {file_creation_mode}, {hours_per_bucket} hours per bucket, max ''' \
                    '''buckets {max_buckets}, max size {max_size} bytes'''.format(**vars())) from None
            pass
        else:
            try:
                self.__tstore=TStore(storage_path / 'tstore')
                self.hours_per_bucket=self.__tstore.hours_per_bucket
                self.max_size=self.__tstore.max_size
                self.max_buckets=self.__tstore.max_buckets
                self.file_creation_mode=self.__tstore.file_creation_mode
                self.schema=read_attrs(storage_path)
            except Exception as e:
                raise in_context(
                    '''open existing PerfLog at {storage_path} reading attributes from its perflog.json file'''.format(**vars())) from None
            pass
        pass

    def __str__(self) -> str:
        return l1(PerfLog.__doc__).format(**vars())
    
    def fetch(self,
              begin:Timestamp,
              end:Timestamp,
              max_records:int,
              max_bytes:ByteCount,
              corruption_handler:Callable[[Exception],Any]) -> Generator[
                  Tuple[float, List[Union[str,int,float,bool,None,List,Dict]]],None,None]:
        '''yield each record of PerfLog {self} with timestamp at or after {begin} excluding records with timestamp at or after {end} and all further records once {max_bytes} bytes have already been yielded or {max_records} records have been yielded
           - note that records returned are in addition order, which is not necessarily time order
           - note max_bytes is applied to the pre-decode (i.e. stored) record data'''
        try:
            records_yielded = 0
            bytes_yielded = ByteCount(0)
            r:Reader
            for bucket_start,bucket_id in self.__tstore.get_buckets_of(begin, end):
                try:
                    with self.__tstore.new_reader(bucket_start,bucket_id) as r:
                        ts=Timestamp(float(bucket_start.value()))
                        for l in read_lines(r):
                            try:
                                if not l.endswith(b'\n'):
                                    raise Exception(f'{l!r} does not end in \\n')
                                time_delta, col_values, size=decode_timestamped_record(l,self.schema)
                            except Exception as e:
                                corruption_handler(e)
                            else:
                                timestamp=ts+time_delta
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
            read_failed:Callable[[Exception],Any]
    ) -> Dict[Tuple[BucketStart,BucketID], Tuple[FilePosition,bytes]]:
        '''return up to {max_bytes} bytes of unseen data from {self} having seen {seen}
           - new data might end with a partial record i.e. not record-aware
           - new data might truncate seen data
           - new data size 0 means bucket does not exist (any longer)
           - read failures are passed to read_failed, skipping that file if read_failed returns
        '''
        try:
            result={}
            result_size=ByteCount(0)
            for bucket, size in self.__tstore.list_unseen(seen).items():
                if size==ByteCount(0):
                    result[bucket]=(FilePosition(0),b'')
                    continue
                bucket_start, bucket_id=bucket
                seen_size=seen.get(bucket,ByteCount(0))
                if seen_size>size:
                    seen_size=ByteCount(0)
                    pass
                r:Reader
                try:
                    with self.__tstore.new_reader(bucket_start,bucket_id) as r:
                        read_size=size-seen_size
                        if read_size>max_bytes-result_size:
                            read_size=max_bytes-result_size
                            pass
                        r.seek_to(FilePosition(seen_size.value()))
                        data=r.read(read_size)
                        result[bucket]=(FilePosition(seen_size.value()),data)
                        result_size+=ByteCount(len(data))
                        pass
                except Exception as e:
                    read_failed(e)
                    pass
                if result_size==max_bytes:
                    break
                pass
            return result
        except Exception as e:
            raise in_function_context(PerfLog.get_some_unseen_data,vars()) from None
        pass

    def new_recorder(self):
        return Recorder(self.storage_path,self.schema,self.__tstore)

    def new_tracker(self):
        return Tracker(self.storage_path,self.__tstore)

    pass

def validate_record(record:List, schema:Dict[ColName,ColType]) -> List:
    '''validate record {record} against schema {schema}
       - returns record'''
    try:
        if len(record) != len(schema):
            raise Exception(f'schema expects {len(schema)} values but record has {len(record)}')
        for value,s in zip(record, schema.items()):
            col_name, col_type = s
            try:
                validate_col(value,col_type)
            except Exception:
                raise in_context(f'validate {col_name} value')
            pass
        return record
    except Exception as e:
        raise in_function_context(validate_record,vars()) from None
    pass

def encode_timestamped_record(time_delta:Duration, record:List, schema:Dict[ColName,ColType]) -> bytes:
    '''encode record {record} and time delta {time_delta} assuming record conforms to schema {schema}'''
    try:
        assert time_delta>=Duration(0)
        return (json.dumps([time_delta.value()]+validate_record(record, schema))+'\n').encode('utf-8')
    except:
        raise in_function_context(encode_timestamped_record,vars()) from None
    pass

def decode_timestamped_record(data:bytes, schema:Dict[ColName,ColType]) -> Tuple[
        Duration, #time_delta
        List[Union[str,int,float,bool,None,List,Dict]], #record
        ByteCount]: # len(data)
    '''decode time delta and record from {data!r} assuming record conforms to schema {schema}'''
    try:
        t:type
        Assert(data).endswith(b'\n')
        x=json.loads(data.decode('utf-8')[:-1])
        record:List[Union[str,int,float,bool,None,List,Dict]]=[]
        if isinstance(x, list):
            if isinstance(x[0], float):
                time_delta=Duration(x[0])
                assert time_delta>=Duration(0), time_delta
                values=x[1:]
                for i, col_type_spec in enumerate(schema.values()):
                    record.append(validate_col(values[i],col_type_spec))
                    pass
                return time_delta, record, ByteCount(len(data))
            else:
                t=type(x[0])
                raise Exception(f'time delta {x[0]!r} is of type {t}, which is not a float')
            pass
        else:
            t=type(x)
            raise Exception(f'{x!r} is of type {t}, which is not a list')
            pass
    except Exception as e:
        raise in_function_context(decode_timestamped_record,vars()) from None
    pass

    
def read_lines(r:Reader) -> Generator[bytes,None,None]:
    r'''read all \n-separated lines from PerLog reader {r}
        - yield one record at a time, including any terminating "\n"
          * where the file does not end in \n the last record yielded will have no "\n"'''
    try:
        buffered=io.BytesIO()
        next:bytes=r.read(ByteCount(1024))
        while len(next):
            recs=next.split(b'\n')
            while len(recs)>1:
                buffered.write(recs[0])
                buffered.write(b'\n')
                yield buffered.getvalue()
                buffered=io.BytesIO()
                recs=recs[1:]
                pass
            buffered.write(recs[0])
            next=r.read(ByteCount(1024))
            pass
        rem=buffered.getvalue()
        if len(rem):
            yield rem
        pass
    except Exception as e:
        raise in_function_context(read_lines,vars()) from None
    pass
                
                
attrs_schema=jsonschema.Schema({
    'schema':[ [str,OneOf('str','int','float','bool','(str)','(int)','(float)','(bool)','json')] ]
})

def validate_str(value)->str:
    if isinstance(value,str):
        return value
    t=type(value)
    raise Exception(f'{value!r} (of type {t}) is not a str')

def validate_int(value)->int:
    if isinstance(value,int):
        return value
    t=type(value)
    raise Exception(f'{value!r} (of type {t}) is not a int')

def validate_float(value)->float:
    if isinstance(value,float):
        return value
    if isinstance(value,int):
        return value
    t=type(value)
    raise Exception(f'{value!r} (of type {t}) is not a float')

def validate_bool(value)->bool:
    if isinstance(value,bool):
        return value
    t=type(value)
    raise Exception(f'{value!r} (of type {t}) is not a bool')

def validate_optional_str(value)->Optional[str]:
    if value is None:
        return value
    return validate_str(value)

def validate_optional_int(value)->Optional[int]:
    if value is None:
        return value
    return validate_int(value)

def validate_optional_float(value)->Optional[float]:
    if value is None:
        return value
    return validate_float(value)

def validate_optional_bool(value)->Optional[bool]:
    if value is None:
        return value
    return validate_bool(value)

def validate_col(value:Any,col_type:ColType)->Union[str,int,float,None,bool,List,Dict]:
    '''verify that value of type value.__class__ is valid for column type {col_type}'''
    try:
        if col_type=='str':
            return validate_str(value)
        if col_type=='int':
            return validate_int(value)
        if col_type=='float':
            return validate_float(value)
        if col_type=='bool':
            return validate_bool(value)
        if col_type=='(str)':
            return validate_optional_str(value)
        if col_type=='(int)':
            return validate_optional_int(value)
        if col_type=='(float)':
            return validate_optional_float(value)
        if col_type=='(bool)':
            return validate_optional_bool(value)
        if col_type=='json':
            # assume caller will effectively validate
            return cast(Union[str,int,float,None,bool,List,Dict],value)
        assert False, f'unknown col type {col_type}'
    except Exception as e:
        raise in_function_context(validate_col,vars())
    pass

PERFLOG_ATTRS='perflog.json'

def read_attrs(storage_path:Path,
               attrs_file=PERFLOG_ATTRS) -> Dict[ColName,ColType]:  # schema
    '''read PerfLog attrs from {storage_path}/{attrs_file}'''
    try:
        with FileReader(storage_path / attrs_file) as f:
            attrs=attrs_schema.validate(json.loads(f.read(f.size())))
            Assert(attrs).isInstanceOf(dict)
            schema={ColName(col_name): cast(ColType,col_type)
                    for col_name,col_type in cast(List[Tuple[str,str]],attrs['schema'])}
            pass
        return schema
    except Exception as e:
        raise in_function_context(read_attrs,vars()) from None
    pass

def write_attrs(storage_path:Path,
                schema:Dict[ColName,ColType],
                file_creation_mode:FileMode,
                attrs_file=PERFLOG_ATTRS) -> ByteCount:
    '''write PerfLog attrs schema {schema} to ''' \
    '''{storage_path}/{attrs_file}
       - returns number of bytes written'''
    try:
        col_name:ColName
        col_type:ColType
        x=attrs_schema.validate({
            'schema':[ [col_name.value(),col_type] for col_name, col_type in schema.items()]
        })
        y=json.dumps(x).encode('utf-8')
        
        with FileWriter(storage_path / attrs_file,
                        mode=file_creation_mode-FileMode(0o111),
                        must_not_exist=True) as f:
            f.output.write(y)
            pass
        return ByteCount(len(y))
    except Exception as e:
        raise in_function_context(write_attrs,vars()) from None
    pass

def trim_trailing_partial_record(
        data:Dict[Tuple[BucketStart,BucketID], Tuple[FilePosition,bytes]])->None:
    '''trim data so it ends with a complete record
       - note you can use this to ensure Tracker always writes complete records'''
    if not len(data):
        return
    last_bucket=list(data.keys())[-1]
    position,d=data[last_bucket]
    trimmed=d[0:d.rfind(b'\n')+1]
    if len(trimmed)==0:
        del data[last_bucket]
    else:
        data[last_bucket]=(position,trimmed)
        pass
    pass
