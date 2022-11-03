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
# Time based store.
#
# Provides storage as append-only "bucket" files, each covering a specified number of hours.
# Bucket file content is up to client. Bucket files are append-only to support incremental
# mirroring. Lookup is by time range. Total storage size is constrained by total byte count
# and number of buckets.
#
# Classes TStore, Writer and Reader form the primary interface.
#
from typing import Tuple,Dict,Literal,overload,Sequence,Callable,Any,cast
from xju.cmc.io import FileReader,FileWriter,FileMode,FilePosition
import os
from pathlib import Path
from xju.misc import ByteCount
from xju.time import Hours,Timestamp
from xju.cmc import cmclass,CM
from xju.xn import in_context,in_function_context,first_line_of as l1
import time
from time import struct_time
from calendar import timegm
import io
from bisect import bisect_left,bisect_right
from xju.assert_ import Assert
from xju import jsonschema
import json
from xju.newtype import Int,Str
import shutil

class BucketStartTag:pass
class BucketIDTag:pass

class BucketStart(Int[BucketStartTag]):pass
class BucketID(Str[BucketIDTag]):pass

class NoSuchBucket(Exception):
    bucket_start:BucketStart
    def __init__(self, bucket_start:BucketStart):
        super().__init__(self,f'no bucket starting at {bucket_start}')
        self.bucket_start = bucket_start
        pass
    pass

class BucketExists(Exception):
    bucket_start:BucketStart
    bucket_id:BucketID
    def __init__(self, bucket_start:BucketStart, bucket_id:BucketID):
        super().__init__(self,f'bucket at {bucket_start} exists with uid {bucket_id}')
        self.bucket_start = bucket_start
        self.bucket_id = bucket_id
        pass
    pass

@cmclass
class Reader(CM):
    '''TStore {self.storage_path} reader for bucket with start {self.bucket_start} and id {self.bucket_id}'''
    storage_path:Path
    bucket_start:BucketStart
    bucket_id:BucketID

    __file_reader:FileReader
    
    def __init__(self,
                 storage_path:Path,
                 bucket_start:BucketStart,
                 bucket_id:BucketID,
                 get_bucket_size:Callable[[],ByteCount],
                 file_reader:FileReader):
        self.storage_path=storage_path
        self.bucket_start=bucket_start
        self.bucket_id=bucket_id
        self.__get_bucket_size=get_bucket_size
        self.__file_reader=file_reader
        pass

    def __str__(self):
        return l1(Reader.__doc__).format(**vars())

    def seek_to(self, position:FilePosition):
        '''position TStore reader {self} so next read occurs {position} bytes from start of bucket data
           - returns self'''
        try:
            self.__file_reader.seek_to(position)
            return self
        except:
            raise in_function_context(Reader.seek_to,vars()) from None
        pass
    
    def seek_by(self, offset:ByteCount):
        '''position TStore reader {self} so next read occurs {offset} bytes from current position
           - returns self'''
        try:
            self.__file_reader.seek_by(offset)
            return self
        except:
            raise in_function_context(Reader.seek_by,vars()) from None
        pass
    
    def position(self) -> FilePosition:
        '''get current position'''
        try:
            return self.__file_reader.position()
        except Exception:
            raise in_function_context(Reader.position,vars()) from None
        pass

    def size(self) -> ByteCount:
        '''return size of bucket'''
        try:
            return self.__get_bucket_size()
        except:
            raise in_function_context(Reader.size,vars()) from None
        pass

    def read(self, max_bytes:ByteCount) -> bytes:
        '''read up to {max_bytes} of data from TStore reader {self} current position'''
        try:
            result=self.__file_reader.read(max_bytes)
            return result
        except Exception:
            raise in_function_context(Reader.read,vars()) from None
        pass
    pass


@cmclass
class Writer(CM):
    '''writer for TStore {self.storage_path}' bucket with start {self.bucket_start} and id {self.bucket_id}'''
    storage_path:Path
    bucket_start:BucketStart
    bucket_id:BucketID

    __file_writer:FileWriter
    
    def __init__(self,
                 storage_path:Path,
                 bucket_start:BucketStart,
                 bucket_id:BucketID,
                 get_bucket_size:Callable[[],ByteCount],
                 get_room:Callable[[],ByteCount],
                 file_writer:FileWriter,
                 appended:Callable[[ByteCount],Any]):
        self.storage_path=storage_path
        self.bucket_start=bucket_start
        self.bucket_id=bucket_id
        self.__get_bucket_size=get_bucket_size
        self.__get_room=get_room
        self.__file_writer=file_writer
        self.__appended=appended
        pass
    
    def size(self) -> ByteCount:
        '''return size of bucket'''
        try:
            return self.__get_bucket_size()
        except Exception:
            raise in_function_context(Writer.size,vars()) from None
        pass

    def append(self, data:bytes):
        '''TStore writer {self} append {len_data} bytes of data assuming there is room
           - assumes our bucket still in tstore
           - raises NoSuchBucket if bucket no longer exists
           - raises BucketExists if bucket now has different id'''
        len_data=len(data)
        try:
            Assert(self.__get_room()>=ByteCount(len(data)))
            self.__file_writer.seek_to(FilePosition(self.size().value()))
            self.__file_writer.output.write(data)
            self.__appended(ByteCount(len(data)))
        except Exception:
            raise in_function_context(Writer.append,vars()) from None
        pass
    pass

class TStore:
    '''{self.storage_path} ({self.hours_per_bucket} hours per bucket, max {self.max_buckets} buckets, max {self.max_size} bytes and file creation mode {self.file_creation_mode})'''
    storage_path:Path
    hours_per_bucket:Hours
    max_buckets: int
    max_size:ByteCount
    file_creation_mode:FileMode
    
    __buckets:Dict[BucketStart,BucketID]
    __bucket_sizes:Dict[Tuple[BucketStart,BucketID],ByteCount]
    __current_size:ByteCount

    def __str__(self):
        return l1(TStore.__doc__).format(**vars())

    def current_size(self) -> ByteCount:
        return self.__current_size

    def get_bucket_sizes(self) -> Dict[Tuple[BucketStart,BucketID],ByteCount]:
        return self.__bucket_sizes.copy()

    @overload
    def __init__(self,
                 storage_path:Path,
                 hours_per_bucket:Hours,
                 max_buckets: int,
                 max_size:ByteCount,
                 file_creation_mode:FileMode):
        '''create non-existent TStore at {storage_path} with mode {file_creation_mode}, {hours_per_bucket} hours per bucket, {max_buckets} buckets max,  {max_size} total bytes max
           - hours_per_bucket must be a factor of 24
           - raises FileExistsError if TStore exists'''

    @overload
    def __init__(self,
                 storage_path: Path):
        '''open existing TStore at {storage_path}
           - raises FileNotFoundError if TStore does not exist'''

    def __init__(self,
                 storage_path:Path,
                 hours_per_bucket=None,
                 max_buckets=None,
                 max_size=None,
                 file_creation_mode=None):
        self.storage_path=storage_path
        if (isinstance(hours_per_bucket,Hours) and
            isinstance(max_buckets,int) and
            isinstance(max_size,ByteCount) and
            isinstance(file_creation_mode,FileMode)):
            try:
                storage_path.mkdir(mode=file_creation_mode.value())
                try:
                    assert hours_per_bucket.value() in [1,2,3,4,6,8,12,24] # factor of 24
                    self.hours_per_bucket, self.max_buckets, \
                        self.max_size, self.file_creation_mode=write_attrs(storage_path,
                                                                           hours_per_bucket,
                                                                           max_buckets,
                                                                           max_size,
                                                                           file_creation_mode)
                    self.__buckets={}
                    self.__bucket_sizes={}
                    self.__current_size=ByteCount(0)
                except Exception:
                    shutil.rmtree(str(storage_path))
                    raise
            except Exception as e:
                raise in_context('create non-existent TStore at {storage_path} with mode {file_creation_mode}, {hours_per_bucket} hours per bucket, max buckets {max_buckets}, max size {max_size} bytes'.format(**vars())) from None
            pass
        else:
            try:
                self.hours_per_bucket, self.max_buckets, \
                    self.max_size, self.file_creation_mode=read_attrs(storage_path)
                self.__bucket_sizes = read_files(storage_path)
                self.__buckets={bucket_start:bucket_id
                                for bucket_start,bucket_id in self.__bucket_sizes}
                self.__current_size=sum(self.__bucket_sizes.values(),ByteCount(0))
            except Exception as e:
                raise in_context('open existing TStore at {storage_path}'.format(**vars())) from None
            pass
        pass
            
    def calc_bucket_start(self, timestamp:Timestamp) -> BucketStart:
        '''calculate start of TStore {self}'s bucket for timestamp {timestamp}
           - bucket need not exist'''
        x = time.gmtime(float(timestamp))
        h = self.hours_per_bucket*(Hours(x.tm_hour)//self.hours_per_bucket)
        return BucketStart(int(timegm(struct_time( (x.tm_year,x.tm_mon,x.tm_mday,int(h),0,0,0,0,0) ))))
        
    def get_buckets_of(self, begin:Timestamp, end:Timestamp) -> Sequence[Tuple[BucketStart,BucketID]]:
        '''get starts and ids of TStore {self}'s existing buckets covering time range [{begin},{end})'''
        begin_bucket=self.calc_bucket_start(begin)
        end_bucket=self.calc_bucket_start(end)
        all_buckets=[(bucket_start,bucket_id) for bucket_start,bucket_id in self.__buckets.items()]
        all_buckets.sort()
        all_starts=[bucket_start for bucket_start,bucket_id in all_buckets]
        return all_buckets[bisect_left(all_starts,begin_bucket):bisect_right(all_starts,end_bucket)]

    def list_unseen(self,seen:Dict[Tuple[BucketStart,BucketID], ByteCount]) -> Dict[Tuple[BucketStart,BucketID], ByteCount]:
        '''list sizes of TStore {self} buckets with unseen data having seen {seen}
           - note where an item in seen is past the end of our current
             data, result will include that bucket with its current size
           - where seen item does not exist (e.g. any more) it will appear in result 
             with size 0'''
        result:Dict[Tuple[BucketStart,BucketID], ByteCount]={}
        seen_keys=set(seen.keys())
        bucket_keys=set([(bucket_start,bucket_id) for bucket_start,bucket_id in self.__buckets.items()])
        # no longer exist
        for bucket_start,bucket_id in seen_keys-bucket_keys:
            result[(bucket_start,bucket_id)]=ByteCount(0)
            pass
        # still exist
        for bucket_start,bucket_id in set.intersection(seen_keys,bucket_keys):
            current_size=self.__bucket_sizes[(bucket_start,bucket_id)]
            if current_size and (current_size != seen[(bucket_start,bucket_id)]):
                result[(bucket_start,bucket_id)]=current_size
                pass
            pass
        # new
        for bucket_start,bucket_id in bucket_keys-seen_keys:
            current_size=self.__bucket_sizes[(bucket_start,bucket_id)]
            if current_size:
                result[(bucket_start,bucket_id)]=current_size
                pass
            pass
        return result

    def get_bucket(self,bucket_start:BucketStart) -> BucketID:
        '''get {self} existing bucket with start {bucket_start}
           - raises NoSuchBucket if bucket does not exist'''
        if bucket_start in self.__buckets:
            return self.__buckets[bucket_start]
        raise NoSuchBucket(bucket_start)

    def verify_bucket_exists(self,bucket_start:BucketStart,bucket_id:BucketID):
        '''verify that bucket with start {bucket_start} exists and has id {bucket_id}'''
        try:
            if bucket_start not in self.__buckets:
                raise NoSuchBucket(bucket_start)
            if (bucket_start,bucket_id) not in self.__bucket_sizes:
                raise BucketExists(bucket_start,self.__buckets[bucket_start])
            pass
        except Exception:
            raise in_function_context(TStore.verify_bucket_exists,vars()) from None
        pass
    
    def create_bucket(self,bucket_start:BucketStart,bucket_id:BucketID):
        '''create {self} bucket with start {bucket_start} and id {bucket_id}
           - bucket can then be read and writtern with Reader and Writer
           - raises BucketExists if bucket already exists with specified start
           - deletes bucket with oldest start to make room if already at max_buckets'''
        try:
            if bucket_start in self.__buckets:
                raise BucketExists(bucket_start,self.__buckets[bucket_start])
                pass
            self.__trim_buckets(self.max_buckets-1)
            path=get_path_of(self.storage_path,bucket_start,bucket_id,self.hours_per_bucket)

            make_path_dirs(path,self.storage_path,self.file_creation_mode,True)
            with FileWriter(path,
                            mode=self.file_creation_mode-FileMode(0o111),
                            must_not_exist=True):
                pass
            self.__buckets[bucket_start]=bucket_id
            self.__bucket_sizes[(bucket_start,bucket_id)]=ByteCount(0)
        except Exception as e:
            raise in_function_context(TStore.create_bucket,vars())
        pass

    def make_room_for(self, byte_count:ByteCount) -> None:
        '''make room in TStore {self} for {byte_count} bytes of data
           - deletes just enough buckets with oldest start times to make room'''
        Assert(byte_count)<self.max_size
        self.__trim_bytes(self.max_size-byte_count)
        pass

    def delete_bucket(self, bucket_start:BucketStart,bucket_id:BucketID):
        '''delete {self} bucket with start {bucket_start} and id {bucket_id}'''
        try:
            bucket_size=self.__bucket_sizes[(bucket_start,bucket_id)]
            path=get_path_of(self.storage_path,bucket_start,bucket_id,self.hours_per_bucket)
            path.unlink()
            path=path.parent
            while path != self.storage_path and len(list(path.iterdir()))==0:
                path.rmdir()
                path=path.parent
                pass
            self.__current_size=self.__current_size-bucket_size
            del self.__buckets[bucket_start]
            del self.__bucket_sizes[(bucket_start,bucket_id)]
        except Exception:
            raise in_function_context(TStore.delete_bucket,vars()) from None
        pass
            
    def __trim_buckets(self, max_buckets):
        '''trim TStore {self}'s buckets so that there are at most {max_buckets} of them
           - deletes buckets with oldest start'''
        try:
            Assert(max_buckets)>0
            if len(self.__buckets)>max_buckets:
                all_buckets=list(self.__buckets.items())
                all_buckets.sort()
                i=0
                while len(self.__buckets)>max_buckets and i<len(all_buckets):
                    self.delete_bucket(*all_buckets[i])
                    i+=1
                    pass
                pass
            pass
        except Exception as e:
            raise in_function_context(TStore.__trim_buckets,vars()) from None
        pass

    def __trim_bytes(self,ensure_at_most:ByteCount):
        '''trim {self} storage to ensure it is at most {ensure_at_most} bytes'''
        try:
            if self.__current_size > ensure_at_most:
                ordered=list(self.__bucket_sizes)
                ordered.sort()
                i=0
                while self.__current_size > ensure_at_most and i<len(ordered):
                    self.delete_bucket(*ordered[i])
                    i+=1
                pass
            pass
        except Exception as e:
            raise in_function_context(TStore.__trim_bytes,vars()) from None
        pass

    def get_bucket_size(self,bucket_start:BucketStart,bucket_id:BucketID)->ByteCount:
        return self.__bucket_sizes[(bucket_start,bucket_id)]

    def new_reader(self,bucket_start:BucketStart, bucket_id:BucketID)->Reader:
        '''create reader for TStore {self}'s bucket with start {bucket_start} and id {bucket_id}
           - raises NoSuchBucket if such a bucket does not yet exist
           - raises BucketExists if bucket with start {bucket_start} has different id'''
        try:
            self.verify_bucket_exists(bucket_start,bucket_id)
            def get_bucket_size():
                return self.get_bucket_size(bucket_start,bucket_id)
            return Reader(
                self.storage_path,
                bucket_start,
                bucket_id,
                get_bucket_size,
                FileReader(get_path_of(self.storage_path,bucket_start,bucket_id,self.hours_per_bucket)))
        except Exception as e:
            raise in_function_context(TStore.new_reader,vars()) from None
        pass

    def new_writer(self,bucket_start:BucketStart, bucket_id:BucketID)->Writer:
        '''create writer for TStore {self}'s bucket with start {bucket_start} and id {bucket_id}
           - bucket must exist
           - ensures store stays within max_buckets along the way'''
        try:
            self.verify_bucket_exists(bucket_start,bucket_id)
            def get_bucket_size():
                return self.get_bucket_size(bucket_start,bucket_id)
            def get_room():
                return self.max_size-self.current_size()
            def appended(number_of_bytes:ByteCount):
                self.__current_size=self.__current_size+number_of_bytes
                self.__bucket_sizes[(bucket_start,bucket_id)]=\
                    self.__bucket_sizes[(bucket_start,bucket_id)]+number_of_bytes
                pass
            return Writer(
                self.storage_path,
                bucket_start,
                bucket_id,
                get_bucket_size,
                get_room,
                FileWriter(
                    get_path_of(self.storage_path,bucket_start,bucket_id,self.hours_per_bucket)),
                appended)
        except Exception as e:
            raise in_function_context(TStore.new_writer,vars()) from None
        pass
    pass


attrs_schema=jsonschema.Schema({
    'hours_per_bucket':int,
    'max_buckets':int,
    'max_size':int,
    'file_creation_mode':int})

TSTORE_ATTRS='tstore.json'

def read_attrs(storage_path:Path,
               attrs_file=TSTORE_ATTRS) -> Tuple[Hours, int, ByteCount, FileMode]:
    '''read TStore attributes from {storage_path}/{attrs_file}'''
    try:
        with FileReader(storage_path/attrs_file) as f:
            x=json.loads(f.input.read().decode('utf-8'))
            attrs_schema.validate(x)
            hours_per_bucket=x['hours_per_bucket']
            max_buckets=x['max_buckets']
            max_size=x['max_size']
            file_creation_mode=x['file_creation_mode']
            Assert(hours_per_bucket).isInstanceOf(int)
            Assert(max_buckets).isInstanceOf(int)
            Assert(max_size).isInstanceOf(int)
            Assert(file_creation_mode).isInstanceOf(int)
            pass
        return (Hours(cast(int,hours_per_bucket)),
                cast(int,max_buckets),
                ByteCount(cast(int,max_size)),
                FileMode(cast(int,file_creation_mode)))
    except Exception as e:
        raise in_function_context(read_attrs,vars()) from None
    pass

def write_attrs(storage_path:Path,
                hours_per_bucket:Hours,
                max_buckets:int,
                max_size:ByteCount,
                file_creation_mode:FileMode,
                attrs_file=TSTORE_ATTRS) -> Tuple[Hours,int,ByteCount,FileMode]:
    '''write TStore attrs hours per bucket {hours_per_bucket}, ''' \
        '''max_buckets {max_buckets}, max size {max_size} bytes, file creation mode ''' \
        '''{file_creation_mode} to {storage_path}/{attrs_file}'''
    try:
        attrs=attrs_schema.validate({
            'hours_per_bucket':hours_per_bucket.value(),
            'max_buckets':max_buckets,
            'max_size':max_size.value(),
            'file_creation_mode':file_creation_mode.value()})
        with FileWriter(storage_path/attrs_file,
                        must_not_exist=True,
                        mode=file_creation_mode-FileMode(0o111)) as f:
            f.output.write(json.dumps(attrs).encode('utf-8'))
            pass
        return (hours_per_bucket,max_buckets,max_size,file_creation_mode)
    except Exception as e:
        raise in_function_context(write_attrs,vars()) from None
    pass

def get_path_of(storage_path:Path,
                bucket_start:BucketStart,
                bucket_id:BucketID,
                hours_per_bucket:Hours) -> Path:
    '''get path to {storage_path} bucket with start {bucket_start} and id {bucket_id} ''' \
        '''where storage has {hours_per_bucket} hours per bucket'''
    '''- bucket need not exist'''
    x = time.gmtime(int(bucket_start))
    h = hours_per_bucket*(Hours(x.tm_hour)//hours_per_bucket)
    s = struct_time( (x.tm_year,x.tm_mon,x.tm_mday,int(h),0,0,0,0,0) )
    return storage_path / f'{s.tm_year}'/f'{s.tm_mon:02}'/f'{s.tm_mday:02}'/f'{s.tm_hour:02}'/f'{bucket_id}.txt'

def read_files(storage_path:Path) -> Dict[Tuple[BucketStart,BucketID],ByteCount]:
    '''read tstore files in {storage_path}'''
    try:
        result={}
        for ydir in [y for y in storage_path.iterdir() if y.is_dir()]:
            year=int(ydir.name)
            try:
                for mdir in [m for m in ydir.iterdir() if m.is_dir()]:
                    month=int(mdir.name)
                    try:
                        for ddir in [d for d in mdir.iterdir() if d.is_dir()]:
                            mday=int(ddir.name)
                            try:
                                for hdir in [h for h in ddir.iterdir() if h.is_dir()]:
                                    hour=int(hdir.name)
                                    try:
                                        bucket_start=BucketStart(timegm(struct_time(
                                            (year,month,mday,hour,0,0,0,0,0))))
                                        bucket_ids=[BucketID(b.name).removesuffix('.txt')
                                                    for b in hdir.iterdir()
                                                    if b.is_file()]
                                        if len(bucket_ids)>1:
                                            raise Exception(
                                                f'expected at most one bucket data file, not {bucket_ids}')
                                        if len(bucket_ids):
                                            bucket_id=bucket_ids[0]
                                            result[(bucket_start,bucket_id)]=ByteCount(
                                                (hdir/f'{bucket_id}.txt').stat().st_size)
                                            pass
                                        pass
                                    except Exception:
                                        raise in_context('stat bucket files in hour {hdir.name} directory'.format(**vars()))
                                    pass
                                pass
                            except Exception:
                                raise in_context(
                                    f'read hour directories in mday {ddir.name} directory'.format(**vars()))
                            pass
                        pass
                    except Exception:
                        raise in_context(f'read day directories in month {mdir.name} directory'.format(**vars()))
                    pass
                pass
            except Exception:
                raise in_context(f'read month directories in year {ydir.name} directory'.format(**vars()))
            pass
        return result
    except Exception:
        raise in_function_context(read_files,vars())
    pass

def make_path_dirs(path:Path, root:Path, mode:FileMode, exist_ok:bool):
    '''make directories of {path} that are not in {root} (assumed to be a parent directory)'''
    try:
        rel=path.relative_to(root)
        for d in reversed(rel.parents):
            try:
                (root/d).mkdir(mode=mode.value(),exist_ok=exist_ok)
            except Exception as e:
                raise in_context(f'make directory {root/d}') from None
            pass
        pass
    except Exception:
        raise in_function_context(make_path_dirs,vars()) from None
    pass
