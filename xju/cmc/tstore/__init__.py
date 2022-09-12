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
from typing import NewType,Tuple,Dict,Literal,overload
from xju.io import FileReader,FileWriter,FileMode
import os
from pathlib import Path
from xju.misc import ByteCount,Hours
from xju.cmc import cmclass
from xju.io import FileReader,FileWriter,FileMode
from xju.xn import in_context,in_function_context
import time
from time import struct_time
from calendar import timegm
import io

BucketStart=NewType('BucketStart',int)
BucketID=NewType('BucketID',str)

class NoSuchBucket(Exception):
    bucket_start:BucketStart
    def __init__(self, bucket_start:BucketStart):
        super.init(f'no bucket {bucket_id} starting at {bucket_start}')
        self.bucket_start = bucket_start
        pass
    pass

class BucketAlreadyExists(Exception):
    bucket_start:BucketStart
    bucket_id:BucketID
    def __init__(self, bucket_start:BucketStart, bucket_id:BucketID):
        super.init(f'bucket at {bucket_start} already exists (it has uid {bucket_id})')
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
    
    @overload
    def __init__(self,
                 storage_path:Path,
                 hours_per_bucket:Hours,
                 max_buckets: int,
                 max_size:ByteCount,
                 file_creation_mode:FileMode):
        '''create non-existent TStore at {storage_path} with mode 0o{file_creation_mode:o}, {hours_per_bucket} hours per bucket, max buckets {max_buckets}, max size {max_size} bytes
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
                 hours_per_bucket,
                 max_buckets,
                 max_size,
                 file_creation_mode):
        try:
            self.storage_path=storage_path
            if (isinstance(hours_per_bucket,Hours) and
                isinstance(max_buckes,int) and
                isinstance(max_size,ByteCount) and
                isinstance(mode,FileMode)):
                try:
                    assert hours_per_bucket in [1,2,3,4,6,8,12,24] # factor of 24
                    self.hours_per_bucket, self.max_buckets, \
                        self.max_size, self.file_creation_mode=write_attrs(storage_path,
                                                                             hours_per_bucket,
                                                                             max_buckets,
                                                                             max_size,
                                                                             file_creation_mode)
                except Exception as e:
                    raise in_context('create non-existent TStore at {storage_path} with mode 0o{file_creation_mode:o}, {hours_per_bucket} hours per bucket, max buckets {max_buckets}, max size {max_size} bytes').format(**vars())) from None
                pass
            else:
                try:
                    self.hours_per_bucket, self.max_buckets, \
                        self.max_size, self.file_creation_mode=read_attrs(storage_path)
                except Exception as e:
                    raise in_context('open existing TStore at {storage_path} reading attributes from its tstore.json file').format(**vars())) from None
                pass
            pass
        pass

    def calc_bucket_start(self, Timestamp timestamp) -> BucketStart:
        '''calculate start of TStore {self}'s bucket for timestamp {timestamp}
           - bucket need not exist'''
        x = time.gmtime(timestamp)
        h = int(x.tm_hour/self.hours_per_bucket)*self.hours_per_bucket
        return timegm(struct_time(x.tm_year,x.tm_mon,x.tm_mday,h,0,0,0,0,0))
        
    def get_buckets_of(self, Timestamp begin, Timestamp end) -> Sequence[Tuple[BucketStart,UID]]:
        '''get starts and ids of TStore {self}'s existing buckets covering time range [{begin},{end})'''
        begin_bucket=self.calc_bucket_start(begin)
        end_bucket=self.calc_bucket_start(end)
        all_buckets=[bucket_start,bucket_id for bucket_start,bucket_id in self.__buckets.items()]
        all_buckets.sort()
        all_starts=[start for bucket_start,bucket_id in all_buckets]
        return all_buckets[bisect_left(begin_bucket,all_starts):bisect_right(end_bucket,all_starts)]

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
            current_size=self.__bucket_sizes[(bucket_start,buckeet_id)]
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
                raise BucketAlreadyExists(bucket_start,self.__buckets[bucket_start])
            pass
        except Exception:
            raise in_function_context(TStore.verify_bucket_exists,vars()) from None
        pass
    
    def create_bucket(self,bucket_start:BucketStart,bucket_id:BucketID):
        '''create {self} bucket with start {bucket_start} and id {bucket_id}
           - bucket can then be read and writtern with Reader and Writer
           - raises BucketAlreadyExists if bucket already exists with specified start'''
        try:
            if bucket_start in self.__buckets:
                raise BucketAlreadyExists(bucket_start,self.__buckets[bucket_start])
                pass
            self.__trim_buckets(self.max_buckets-1)
            path=get_path_of(self.storage_path,bucket_start,bucket_id,self.hours_per_bucket)
            os.makedirs(path.parent)
            with xju.cmc.io.FileWriter(path,
                                       mode=self.store.file_creation_mode,
                                       must_not_exist=True):
                pass
            self.__buckets[bucket_start]=bucket_id
            self.__bucket_sizes[(bucket_start,bucket_id)]=0
        except Exception as e:
            raise in_function_context(TStore.create_bucket,vars())
        pass

    def make_room_for(self, byte_count:ByteCount):
        '''make room in TStore {self} for {byte_count} bytes of data
           - deletes just enough buckets with oldest start times to make room'''
        Assert(byte_count)<self.max_bytes
        self.__trim_bytes(self.max_bytes-byte_count)
        pass

    def delete_bucket(self, bucket_start:BucketStart,bucket_id:BucketId):
        '''delete {self} bucket with start {bucket_start} and id {bucket_id}
           - synchronises deletion to disk'''
        try:
            bucket_size=self.bucket_sizes[(bucket_start,bucket_id)]
            path=get_path_of(self.storage_path,bucket_start,bucket_id,self.hours_per_bucket)
            with xju.cmc.FileReader(path) as f, xju.cmc.FileWriter(path.parent) as d:
                f.unlink()
                d.sync()
                pass
            self.current_size-=bucket_size
            del self.__buckets[(bucket_start,bucket_id)]
            del self.__bucket_sizes[(bucket_start,bucket_id)]
        except Exception:
            raise in_function_context(TStore.delete_bucket,vars()) from None
        pass
            
    def __trim_buckets(self, max_buckets):
        '''trim TStore {self}'s buckets so that there are at most {max_buckets} of them
           - deletes buckets with oldest start'''
        try:
            xju.Assert(max_buckets)>0
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

    def __trim_bytes(ensure_at_most:ByteCount):
        '''trim {self} storage currently {self.__current_size} bytes to ensure it is at most ''' \
            '''{at_most} bytes'''
        try:
            if current_size > ensure_at_most:
                for bucket,size in sorted(list(self.__bucket_sizes)):
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


@xju.cmc.cmclass
class Reader(xju.cmc.CM):
    '''TStore {self.store} reader for bucket with start {bucket_start} and id {bucket_id}'''
    store:Store
    bucket_start:BucketStart
    bucket_id:BucketID

    __impl:xju.cmc.io.FileReader
    
    def __init__(self, store:Store, bucket_start:BucketStart, bucket_id:BucketId):
        '''create reader for TStore {self.store}'s bucket with start {bucket_start} and id {bucket_id}
           - raises NoSuchBucket if such a bucket does not yet exist
           - raises BucketAlreadyExists if bucket with start {bucket_start} has different id'''
        try:
            self.store=store
            self.bucket_start=bucket_start
            self.bucket_id=bucket_id
            self.store.verify_bucket_exists(bucket_start,bucket_id)
            self.__impl=xju.cmc.io.FileReader(
                get_path_of(self.store.storage_path,bucket_start,bucket_id,self.store.hours_per_bucket))
        except Exception as e:
            raise in_function_context(Reader.__init__,vars()) from None
        pass

    def __str__(self):
        return l1(Reader.__doc__).format(**vars())

    def seek_to(self, position:ByteCount):
        '''position TStore reader {self} so next read occurs {position} bytes from start of bucket data
           - returns self'''
        try:
            self.__impl.seek(offset, io.SEEK_SET)
            return self
        except:
            raise in_function_context(Reader.seek_to,vars()) from None
        pass
    
    def seek_by(self, offset:ByteCount):
        '''position TStore reader {self} so next read occurs {offset} bytes from current position
           - returns self'''
        try:
            self.input.seek(offset, io.SEEK_CUR)
            return self
        except:
            raise in_function_context(Reader.seek_by,vars()) from None
        pass
    
    def size(self) -> ByteCount:
        '''return size of bucket'''
        try:
            return self.store.bucket_sizes((self.bucket_start,self.bucket_id))
        except:
            raise in_function_context(Reader.size,vars()) from None
        pass

    def read(self, max_bytes:ByteCount) -> bytes:
        '''read up to {max_bytes} of data from TStore reader {self} current position'''
        try:
            return self.__impl.read(max_bytes)
        except Exception:
            raise in_function_context(Reader.read,vars()) from None
        pass
    pass


@xju.cmc.cmclass
class Writer(xju.cmc.CM):
    '''writer for TStore {self.store}' bucket with start {self.bucket_start} and id {self.bucket_id}'''
    store:Store
    bucket_start:BucketStart
    bucket_id:BucketID

    __impl:xju.cmc.io.FileWriter

    def __init__(self,store:Store,bucket_start:BucketStart,bucket_id:BucketID):
        '''create writer for TStore {self.store}'s bucket with start {bucket_start} and id {bucket_id}
           - bucket must exist
           - ensures store stays within max_buckets along the way'''
        try:
            self.store=store
            self.bucket_start=bucket_start
            self.bucket_id=bucket_id
            self.store.verify_bucket_exists(bucket_start,bucket_id)
            self.__impl=xju.cmc.io.FileWriter(
                get_path_of(self.store.storage_path,bucket_start,bucket_id,self.store.hours_per_bucket))
        except Exception as e:
            raise in_function_context(Writer.__init__,vars()) from None
        pass

    def seek_to(self, position:ByteCount):
        '''position TStore reader {self} so next write occurs {position} bytes from start of bucket data
           - returns self'''
        try:
            self.__impl.seek(position, io.SEEK_SET)
            return self
        except Exception:
            raise in_function_context(Writer.seek_to,vars()) from None
        pass
    
    def seek_by(self, offset:ByteCount):
        '''position TStore reader {self} so next write occurs {offset} bytes from current position
           - returns self'''
        try:
            self.input.seek(offset, io.SEEK_CUR)
            return self
        except Exception:
            raise in_function_context(Writer.seek_by,vars()) from None
        pass

    def size(self) -> ByteCount:
        '''return size of bucket'''
        try:
            self.store.verify_bucket_exists(self.bucket_start,self.bucket_id)
            return self.store.__bucket_sizes[(self.bucket_start,self.bucket_id)]
        except Exception:
            raise in_function_context(Writer.size,vars()) from None
        pass

    def position(self) -> ByteCount:
        '''return TStore writer {self}'s current position'''
        try:
            return self.__impl.position()
        except Exception:
            raise in_function_context(Writer.position,vars()) from None
        pass

    def append(self, data:bytes):
        '''TStore writer {self} append {len(data)} bytes of data assuming there is room
           - assumes our bucket still in tstore
           - raises NoSuchBucket if bucket no longer exists
           - raises BucketAlreadyExists if bucket now has different id'''
        try:
            Assert(len(data))<=self.store.max_bytes-self.store.current_size
            self.__impl.seek_to(self.size())
            self.__impl.write(data)
            self.store.current_size+=len(data)
            self.store.__buckets[(self.bucket_start,self.bucket_id)]+=len(data)
        except Exception:
            raise in_function_context(Writer.append,vars()) from None
        pass
    pass

    def write(self, data:bytes):
        '''TStore writer {self} write {len(data)} bytes of data at current position ''' \
        '''assuming there is room
           - assumes our bucket still in tstore
           - raises NoSuchBucket if bucket no longer exists
           - raises BucketAlreadyExists if bucket now has different id'''
        try:
            new_size=max(self.size(),self.position()+len(data))
            delta=new_size-self.size()
            Assert(delta)<=self.store.max_bytes-self.store.current_size
            self.__impl.write(data)
            self.store.current_size+=delta
            self.store.__buckets[(self.bucket_start,self.bucket_info)]+=delta
        except Exception:
            raise in_function_context(Writer.write,vars()) from None
        pass

    def truncate(self):
        '''TStore writer {self} truncate bucket at current position'''
        try:
            bucket_size=self.size()
            self.__impl.truncate()
            self.store.__current_size-=bucket_size
            self.store.__bucket_sizes[(self.bucket_start,self.bucket_id)]=0
        except Exception:
            raise in_function_context(Writer.truncate,vars())
        pass
    pass

attrs_schema=xju.json_schema.Schema({
    'hours_per_bucket':int,
    'max_buckets':int,
    'max_size':int,
    'file_creation_mode':int})

TSTORE_ATTRS='tstore.json'

def read_attrs(storage_path:Path,
               attrs_file=TSTORE_ATTRS) -> Tuple[Hours, BucketCount, ByteCount, FileMode]:
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
                        BucketCount(max_buckets),
                        ByteCount(max_size),
                        FileMode(file_creation_mode))
            else:
                assert False, f'{x} was validated against {__attrs_schema}, so should not be here'
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
        with FileWriter(self.storage_path/attrs_file,
                        must_not_exist=must_not_exist,
                        mode=file_creation_mode) as f:
            f.write(to_json(attrs).encode('utf-8'))
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
    x = time.gmtime(bucket_start)
    h = int(x.tm_hour/hours_per_bucket)*hours_per_bucket
    s = struct_time(x.tm_year,x.tm_mon,x.tm_mday,h,0,0,0,0,0)
    return storage_path / f'{s.tm_year}'/f'{s.tm_month:02}'/f'{s.tm_mday:02}'/f'{s.tm_hour:02}'/f'{bucket_id}.txt'