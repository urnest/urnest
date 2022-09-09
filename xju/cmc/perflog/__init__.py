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
from typing import NewType, Union, Literal, List, Optional
import xju.cmc
from xju.misc import ByteCount
from xju.xn import in_context,in_function_context,firstLineOf as l1
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
    '''{storage_path} size limited to {max_size} bytes with record schema {self.schema}, ''' \
    '''{self.hours_per_file} hours-per-file, file creation mode {self.file_creation_mode} ''' \
    '''and syncrhonous_writes {self.synchronous_writes}'''
    storage_path:Path,
    schema:Dict[ColName,ColType]
    synchronous_writes:bool

    __tstore:tstore.TStore
    __writers:xju.cmc.Dict[ Timestamp, tstore.Writer ]

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
                 schema,
                 synchronous_writes,
                 hours_per_bucket,
                 max_buckets,
                 max_size,
                 file_creation_mode):
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
                __write_attrs(storage_path / 'perflog.json', schema, synchronous_writes)
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
                self.schema,self.synchronous_writes=__read_attrs(storage_path / 'perflog.json')
            except Exception as e:
                raise in_context(
                    '''open existing PerfLog at {storage_path} reading attributes from its perflog.json file'''.format(**vars())) from None
            pass
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
            raise in_function_context(PerfLog.add,vars()) from None
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
                    corruption_handler(e)
                pass
            pass
        except Exception:
            raise in_function_context(PerfLog.fetch,vars()) from None
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
            raise in_function_context(PerfLog.get_some_unseen_data,vars()) from None
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
            raise in_function_context(PerfLog.__get_writer,vars()) from None
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
        raise in_function_context(encode_record,vars()) from None
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
        raise in_function_context(__get_file_of,vars()) from None
    pass
    

__attrs_schema=xju.jsonschema.Schema({
    'schema':[ [str,OneOf('str','int','float','(str)','(int)','(float)')] ],
    'synchronous_writes':bool
})

__str_col_type:Dict[str,ColType]={
    'str':str,
    'int':int,
    'float':float,
    '(str)':Optional[str],
    '(int)':Optional[int],
    '(float)':Optional[float]
}
__col_type_str={ t:s for s,t in __str_col_type.items() }

__PERFLOG_ATTRS='perflog.json'

def __read_attrs(storage_path:Path,
                 attrs_file=__PERFLOG_ATTRS) -> Tuple[Dict[ColName,ColType],  # schema
                                                      bool]:                  # synchronous_writes
    '''read PerfLog attrs from {storage_path}/{attrs_file}'''
    try:
        with xju.io.cmc.FileReader(storage_path / attrs_file) as f:
            attrs=__attrs_schema.validate(json.loads(f.read()))
            if (isinstance(attrs,dict) and
                isinstance(_schema:=attrs['schema'],list) and
                isinstance(synchronous_writes:=attrs['synchronous_writes'],bool)):
                schema={ColName(col_name): __str_col_type[type_spec] for col_name,type_spec in _schema}
                return schema,synchronous_writes
            assert False, f'should not be here: {attrs_} was validated against {__attrs_schema}'
    except Exception as e:
        raise in_context(__read_attrs,vars()) from None
    pass

def __write_attrs(storage_path:Path, schema:Dict[ColName,ColType],
                  synchronous_writes:bool,
                  attrs_file=__PERFLOG_ATTRS):
    '''write PerfLog attrs schema {schema}, synchronous_writes {synchronous_writes} to ''' \
    '''{storage_path}/{attrs_file}'''
    try:
        x=__attrs_schema.validate({
            'schema':[ [col_name,__col_type_str[col_type]] for col_name, col_type in schema.items()],
            'synchronous_writes':synchronous_writes
        })
        with xju.io.cmc.FileWriter(storage_path / 'perflog.json',
                                   mode=file_creation_mode,
                                   must_not_exist=True) as f:
            f.write(json.dumps(x))
            pass
    except Exception as e:
        raise in_context(__write_attrs,vars()) from None
    pass
