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
from typing import Tuple,Dict,Literal,overload,Sequence
from xju.cmc.io import FileReader,FileWriter,FileMode,FilePosition,FilePositionDelta
import os
from pathlib import Path
from xju.misc import ByteCount
from xju.time import Hours,Timestamp
from xju.cmc import cmclass,CM
from xju.xn import in_context,in_function_context
import time
from time import struct_time
from calendar import timegm
import io
from bisect import bisect_left,bisect_right
from xju.assert_ import Assert
from xju import jsonschema
import json
from xju.newtype import Int,Str

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

class TStore:
    '''{self.storage_path} ({self.hours_per_bucket} hours per bucket, max {self.max_buckets} buckets, max {self.max_size} bytes and file creation mode 0o{self.file_creation_mode:o})'''
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

    def current_size(self):
        return self.__current_size

    @overload
    def __init__(self,
                 storage_path:Path,
                 hours_per_bucket:Hours,
                 max_buckets: int,
                 max_size:ByteCount,
                 file_creation_mode:FileMode):
        '''create non-existent TStore at {storage_path} with mode 0o{file_creation_mode:o}, {hours_per_bucket} hours per bucket, {max_buckets} buckets max,  {max_size} total bytes max
           - hours_per_bucket must be a factor of 24
           - raises FileExistsError if TStore exists'''
        pass
    @overload
    def __init__(self,
                 storage_path: Path,
                 hours_per_bucket:Literal[None]=None,
                 max_buckets: Literal[None]=None,
                 max_size:Literal[None]=None,
                 file_creation_mode:Literal[None]=None):
        '''open existing TStore at {storage_path} reading attributes from its tstore.json file
           - raises FileNotFoundError if TStore does not exist'''
        pass
    def __init__(self,
                 storage_path:Path,
                 hours_per_bucket=None,
                 max_buckets=None,
                 max_size=None,
                 file_creation_mode=None):
        self.storage_path=storage_path
        if (isinstance(hours_per_bucket,int) and
            isinstance(max_buckets,int) and
            isinstance(max_size,int) and
            isinstance(file_creation_mode,int)):
            try:
                assert hours_per_bucket in [1,2,3,4,6,8,12,24] # factor of 24
                self.hours_per_bucket, self.max_buckets, \
                    self.max_size, self.file_creation_mode=write_attrs(storage_path,
                                                                       Hours(hours_per_bucket),
                                                                       max_buckets,
                                                                       ByteCount(max_size),
                                                                       FileMode(file_creation_mode))
            except Exception as e:
                raise in_context('create non-existent TStore at {storage_path} with mode 0o{file_creation_mode:o}, {hours_per_bucket} hours per bucket, max buckets {max_buckets}, max size {max_size} bytes'.format(**vars())) from None
            pass
        else:
            try:
                self.hours_per_bucket, self.max_buckets, \
                    self.max_size, self.file_creation_mode=read_attrs(storage_path)
                self.__bucket_sizes = read_files(storage_path)
                self.__buckets={bucket_start:bucket_id
                                for bucket_start,bucket_id in self.__bucket_sizes}
            except Exception as e:
                raise in_context('open existing TStore at {storage_path} reading attributes from its tstore.json file'.format(**vars())) from None
            pass
        pass
            
    def calc_bucket_start(self, timestamp:Timestamp) -> BucketStart:
        '''calculate start of TStore {self}'s bucket for timestamp {timestamp}
           - bucket need not exist'''
        x = time.gmtime(float(timestamp))
        h = self.hours_per_bucket*(Hours(x.tm_hour)//self.hours_per_bucket)
        return BucketStart(int(timegm(struct_time( (x.tm_year,x.tm_mon,x.tm_mday,int(h),0,0,0,0) ))))
        
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
            if current_size != seen[(bucket_start,bucket_id)]:
                result[(bucket_start,bucket_id)]=current_size
                pass
            pass
        # new
        for bucket_start,bucket_id in bucket_keys-seen_keys:
            current_size=self.__bucket_sizes[(bucket_start,bucket_id)]
            result[(bucket_start,bucket_id)]=current_size
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
           - raises BucketExists if bucket already exists with specified start'''
        try:
            if bucket_start in self.__buckets:
                raise BucketExists(bucket_start,self.__buckets[bucket_start])
                pass
            self.__trim_buckets(self.max_buckets-1)
            path=get_path_of(self.storage_path,bucket_start,bucket_id,self.hours_per_bucket)
            os.makedirs(path.parent)
            with FileWriter(path,
                            mode=self.file_creation_mode,
                            must_not_exist=True):
                pass
            self.__buckets[bucket_start]=bucket_id
            self.__bucket_sizes[(bucket_start,bucket_id)]=ByteCount(0)
        except Exception as e:
            raise in_function_context(TStore.create_bucket,vars())
        pass

    def make_room_for(self, byte_count:ByteCount):
        '''make room in TStore {self} for {byte_count} bytes of data
           - deletes just enough buckets with oldest start times to make room'''
        Assert(byte_count)<self.max_size
        self.__trim_bytes(self.max_size-byte_count)
        pass

    def delete_bucket(self, bucket_start:BucketStart,bucket_id:BucketID):
        '''delete {self} bucket with start {bucket_start} and id {bucket_id}
           - synchronises deletion to disk'''
        try:
            bucket_size=self.__bucket_sizes[(bucket_start,bucket_id)]
            path=get_path_of(self.storage_path,bucket_start,bucket_id,self.hours_per_bucket)
            with FileReader(path) as f, FileWriter(path.parent) as d:
                f.unlink()
                d.sync()
                pass
            self.__current_size=ByteCount(self.__current_size-bucket_size)
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
            if len(self.__buckets>max_buckets):
                all_buckets=list(self.__buckets.items())
                all_buckets.sort()
                for bucket_start,bucket_id in all_buckets:
                    self.delete_bucket(bucket_start,bucket_id)
                    if len(self.__buckets<=max_buckets):
                        return
                    pass
                pass
            pass
        except Exception as e:
            raise in_function_context(TStore.__trim_buckets,vars()) from None
        pass

    def __trim_bytes(self,ensure_at_most:ByteCount):
        '''trim {self} storage currently {self.__current_size} bytes to ensure it is at most ''' \
            '''{at_most} bytes'''
        try:
            if self.__current_size > ensure_at_most:
                for bucket,size in sorted(list(self.__bucket_sizes.items())):
                    self.delete_bucket(*bucket)
                    if self.__current_size <= ensure_at_most:
                        return
                    pass
                pass
            pass
        except Exception as e:
            raise in_function_context(TStore.__trim_bytes,vars()) from None
        pass
    pass


@cmclass
class Reader(CM):
    '''TStore {self.store} reader for bucket with start {bucket_start} and id {bucket_id}'''
    store:TStore
    bucket_start:BucketStart
    bucket_id:BucketID

    __impl:FileReader
    
    def __init__(self, store:TStore, bucket_start:BucketStart, bucket_id:BucketID):
        '''create reader for TStore {self.store}'s bucket with start {bucket_start} and id {bucket_id}
           - raises NoSuchBucket if such a bucket does not yet exist
           - raises BucketExists if bucket with start {bucket_start} has different id'''
        try:
            self.store=store
            self.bucket_start=bucket_start
            self.bucket_id=bucket_id
            self.store.verify_bucket_exists(bucket_start,bucket_id)
            self.__impl=FileReader(
                get_path_of(self.store.storage_path,bucket_start,bucket_id,self.store.hours_per_bucket))
        except Exception as e:
            raise in_function_context(Reader.__init__,vars()) from None
        pass

    def __str__(self):
        return l1(Reader.__doc__).format(**vars())

    def seek_to(self, position:FilePosition):
        '''position TStore reader {self} so next read occurs {position} bytes from start of bucket data
           - returns self'''
        try:
            self.__impl.seek_to(position)
            return self
        except:
            raise in_function_context(Reader.seek_to,vars()) from None
        pass
    
    def seek_by(self, offset:ByteCount):
        '''position TStore reader {self} so next read occurs {offset} bytes from current position
           - returns self'''
        try:
            self.__impl.seek_by(FilePositionDelta(offset.value()))
            return self
        except:
            raise in_function_context(Reader.seek_by,vars()) from None
        pass
    
    def size(self) -> ByteCount:
        '''return size of bucket'''
        try:
            return self.store.__bucket_sizes[(self.bucket_start,self.bucket_id)]
        except:
            raise in_function_context(Reader.size,vars()) from None
        pass

    def read(self, max_bytes:ByteCount) -> bytes:
        '''read up to {max_bytes} of data from TStore reader {self} current position'''
        try:
            result=self.__impl.input.read(max_bytes.value())
            if isinstance(result,bytes):
                return result
            assert False
        except Exception:
            raise in_function_context(Reader.read,vars()) from None
        pass
    pass


@cmclass
class Writer(CM):
    '''writer for TStore {self.store}' bucket with start {self.bucket_start} and id {self.bucket_id}'''
    store:TStore
    bucket_start:BucketStart
    bucket_id:BucketID

    __impl:FileWriter

    def __init__(self,store:TStore,bucket_start:BucketStart,bucket_id:BucketID):
        '''create writer for TStore {self.store}'s bucket with start {bucket_start} and id {bucket_id}
           - bucket must exist
           - ensures store stays within max_buckets along the way'''
        try:
            self.store=store
            self.bucket_start=bucket_start
            self.bucket_id=bucket_id
            self.store.verify_bucket_exists(bucket_start,bucket_id)
            self.__impl=FileWriter(
                get_path_of(self.store.storage_path,bucket_start,bucket_id,self.store.hours_per_bucket))
        except Exception as e:
            raise in_function_context(Writer.__init__,vars()) from None
        pass

    def size(self) -> ByteCount:
        '''return size of bucket'''
        try:
            self.store.verify_bucket_exists(self.bucket_start,self.bucket_id)
            return self.store.__bucket_sizes[(self.bucket_start,self.bucket_id)]
        except Exception:
            raise in_function_context(Writer.size,vars()) from None
        pass

    def append(self, data:bytes):
        '''TStore writer {self} append {len(data)} bytes of data assuming there is room
           - assumes our bucket still in tstore
           - raises NoSuchBucket if bucket no longer exists
           - raises BucketExists if bucket now has different id'''
        try:
            Assert(len(data))<=self.store.max_size-self.store.__current_size
            self.__impl.seek_to(FilePosition(self.size().value()))
            self.__impl.output.write(data)
            self.store.__current_size=self.store.__current_size+ByteCount(len(data))
            self.store.__bucket_sizes[(self.bucket_start,self.bucket_id)]=\
                self.store.__bucket_sizes[(self.bucket_start,self.bucket_id)]+ByteCount(len(data))
        except Exception:
            raise in_function_context(Writer.append,vars()) from None
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
            x=json.loads(f.read().decode('utf-8'))
            attrs_schema.validate(x)
            hours_per_bucket=x['hours_per_bucket']
            max_buckets=x['max_buckets']
            max_size=x['max_size']
            file_creation_mode=x['file_creation_mode']
            if (isinstance(hours_per_bucket,int) and
                isinstance(max_buckets,int) and
                isinstance(max_size,int) and
                isinstance(file_creation_mode,int)):
                return (Hours(hours_per_bucket),
                        max_buckets,
                        ByteCount(max_size),
                        FileMode(file_creation_mode))
            else:
                assert False, f'{x} was validated against {attrs_schema}, so should not be here'
                pass
            pass
    except Exception as e:
        raise in_function_context(read_attrs,vars()) from None
    pass

def write_attrs(storage_path:Path,
                hours_per_bucket:Hours,
                max_buckets:int,
                max_size:ByteCount,
                file_creation_mode:FileMode,
                attrs_file=TSTORE_ATTRS):
    '''write TStore attrs hours per bucket {hours_per_bucket}, ''' \
        '''max_buckets {max_buckets}, max size {max_size} bytes, file creation mode ''' \
        '''0o{file_creation_mode:o} to {storage_path}/{attrs_file}'''
    try:
        attrs=attrs_schema.validate({
            'hours_per_bucket':hours_per_bucket,
            'max_buckets':max_buckets,
            'max_size':max_size,
            'file_creation_mode':file_creation_mode})
        with FileWriter(storage_path/attrs_file,
                        must_not_exist=True,
                        mode=file_creation_mode) as f:
            f.write(json.dumps(attrs).encode('utf-8'))
            pass
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
                                        bucket_ids=[BucketID(b.name) for b in hdir.iterdir()
                                                    if b.is_file()]
                                        if len(bucket_ids)>1:
                                            raise Exception(
                                                f'expected at most one bucket data file, not {bucket_ids}')
                                        if len(bucket_ids):
                                            bucket_id=bucket_ids[0]
                                            result[(bucket_start,bucket_id)]=ByteCount(
                                                (hdir/str(bucket_id)).stat().st_size)
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
