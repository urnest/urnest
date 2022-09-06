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
from typing import NewType
from xju.io import FileReader,FileWriter,FileMode

BucketStart=NewType('BucketStart',int)
BucketUID=NewType('BucketUID',str)

@dataclass
class BucketInfo:
    uid:BucketUID
    size:ByteCount
    pass

class NoSuchBucketYet(Exception):
    bucket_start:BucketStart
    def __init__(self, bucket_start:BucketStart, bucket_id:BucketUID):
        super.init(f'no bucket {bucket_id} starting at {bucket_start} yet')
        self.bucket_start = bucket_start
        self.bucket_id = bucket_id
        pass
    pass

class BucketAlreadyExists(Exception):
    bucket_start:BucketStart
    def __init__(self, bucket_start:BucketStart, bucket_info:BucketInfo):
        super.init(f'no bucket {bucket_id} starting at {bucket_start} yet')
        self.bucket_start = bucket_start
        self.bucket_info = bucket_info
        pass
    pass

Seen=Dict[Tuple[BucketStart,BucketUID], ByteCount]

class TStore:
    '''{self.storage_path} ({self.hours_per_bucket} hours per bucket, max {self.max_buckets} buckets, max {self.max_size} bytes and file creation mode 0o{self.file_creation_mode:o})'''
    storage_path:Path
    hours_per_bucket:Hours
    max_buckets: int
    max_size:ByteCount
    file_creation_mode:FileMode
    
    __buckets:Dict[BucketStart,BucketInfo]
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
                 max_buckets: Literao[None]=None,
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
                        self.max_size, self.file_creation_mode=__write_attrs(storage_path,
                                                                             hours_per_bucket,
                                                                             max_buckets,
                                                                             max_size,
                                                                             file_creation_mode)
                except Exception as e:
                    raise inContext('create non-existent TStore at {storage_path} with mode 0o{file_creation_mode:o}, {hours_per_bucket} hours per bucket, max buckets {max_buckets}, max size {max_size} bytes').format(**vars())) from None
                pass
            else:
                try:
                    self.hours_per_bucket, self.max_buckets, \
                        self.max_size, self.file_creation_mode=__read_attrs(storage_path)
                except Exception as e:
                    raise inContext('open existing TStore at {storage_path} reading attributes from its tstore.json file').format(**vars())) from None
                pass
            pass
        pass

    def calc_bucket_start(self, Timestamp timestamp) -> BucketStart:
        '''calculate start of TStore {self}'s bucket for timestamp {timestamp}
           - bucket need not exist'''
        x = time.gmtime(timestamp)
        h = int(x.tm_hour/self.hours_per_file)*self.hours_per_file
        return timegm(struct_time(x.tm_year,x.tm_mon,x.tm_mday,h,0,0,0,0,0))
        
    def get_bucket_of(self, Timestamp timestamp) -> Tuple[BucketStart, BucketInfo]:
        '''Get start and id of TStore {self}'s bucket for timestamp {timestamp}
           - raises NoSuchBucketYet if bucket does not exist'''
        info = self.__buckets.get(self.calc_bucket_start(timestamp), None)
        if isinstance(info, BucketInfo):
            return info.uid
        raise NoSuchBucketYet(timestamp)

    def get_buckets_of(self, Timestamp begin, Timestamp end) -> Sequence[Tuple[BucketStart,UID]]:
        '''get starts and ids of TStore {self}'s existing buckets covering time range [{begin},{end})'''
        begin_bucket=self.calc_bucket_start(begin)
        end_bucket=self.calc_bucket_start(end)
        all_starts=list(self.__buckets.items())
        all_starts.sort()
        return all_starts[bisect_left(begin_bucket,all_starts):bisect_right(end_bucket,all_starts)]

    def list_unseen(self,seen:Seen) -> Dict[Tuple[BucketStart,BucketUID], Position]:
        '''list start positions of unseen TStore {self} data having seen {seen}
           - note where an item in seen has position past the end of our current
             data, result will include that bucket with Position 0 so caller can
             pick up corrected data'''
        pass

    def get_bucket(self,bucket_start:BucketStart,bucket_id:BucketUID) -> Path:
        '''get {self} existing bucket {bucket_id} (start {bucket_start})
           - raises NoSuchBucketYet if bucket does not exist'''
        if bucket_start in self.__buckets:
            info = self.__buckets[bucket_start]
            if info.uid==bucket_id:
                return self.__get_path_of(bucket_start,bucket_id)
            pass
        raise NoSuchBucketYet(bucket_start,bucket_id)

    def create_bucket(self,bucket_start:BucketStart,bucket_id:BucketUID) -> Path:
        '''create {self} bucket {bucket_id} (start {bucket_start})
           - raises BucketAlreadyExists if bucket already exists'''
        try:
            if bucket_start in self.__buckets:
                info = self.__buckets[bucket_start]
                if info.uid==bucket_id:
                    raise BucketAlreadyExists(bucket_start,bucket_info)
                self.__delete_bucket(bucket_start,info)
                assert not bucket_start in self.__buckets
                pass
            self.__trim_buckets(self.max_buckets-1)
            info = self.__buckets.set_default(bucket_start,Info(bucket_id,ByteCount(0)))
            path=self.__get_path_of(bucket_start, bucket_id)
            os.makedirs(path)
            return path
        except Exception as e:
            
    def __get_path_of(self, bucket_start:BucketStart, bucket_id:BucketUID) -> Path:
        '''get path to TStore {self}'s bucket with start {bucket_start} and id {bucket_id}'''
        x = time.gmtime(bucket_start)
        h = int(x.tm_hour/hours_per_file)*hours_per_file
        s = struct_time(x.tm_year,x.tm_mon,x.tm_mday,h,0,0,0,0,0)
        return self.storage_path / f'{s.tm_year}'/f'{s.tm_month:02}'/f'{s.tm_mday:02}'/f'{s.tm_hour:02}'/f'{bucket_id}.txt'

    def __trim_buckets(self, max_buckets):
        '''trim TStore {self}'s buckets so that there are at most {max_buckets} of them
           - deletes buckets with oldest start'''
        try:
            xju.Assert(max_buckets)>0
            if len(self.__buckets>max_buckets):
                all_starts=list(self.__buckets.items())
                all_starts.sort()
                for bucket_start,bucket_info in all_starts:
                    self.__delete_bucket(bucket_start,bucket_info)
                    if len(self.__buckets<=max_buckets):
                        break
                    pass
                pass
        except Exception as e:
            raise inContext(l1(TStore.__trim_buckets.__doc__).format(**vars()))
        pass
    pass

@xju.cmc.cmclass
class Reader(xju.cmc.CM):
    '''TStore {self.store} bucket {self.bucket[1]} (start {self.bucket[0]}) reader'''
    store:Store
    bucket:Tuple[BucketStart,UID]

    __impl:xju.cmc.io.FileReader
    
    def __init__(self, store:Store, bucket:Tuple[BucketStart,UID]):
        '''create reader for TStore {self.store}'s bucket with start {bucket[0]} and id {bucket[1]}
           - raises NoSuchBucketYet if such a bucket does not yet exist'''
        try:
            self.store=store
            self.bucket=bucket
            self.__impl=xju.cmc.io.FileReader(self.store.get_bucket(*bucket))
        except Exception as e:
            raise inContext(l1(Reader.__init__.__doc__).format(**vars()))
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
            raise inContext(l1(Reader.seek_to.__doc__).format(**vars())) from None
        pass
    
    def seek_by(self, offset:ByteCount):
        '''position TStore reader {self} so next read occurs {offset} bytes from current position
           - returns self'''
        try:
            self.input.seek(offset, io.SEEK_CUR)
            return self
        except:
            raise inContext(l1(Reader.seek_by.__doc__).format(**vars())) from None
        pass
    
    def size(self) -> ByteCount:
        '''return size of bucket'''
        try:
            return self.__impl.size()
        except:
            raise inContext(l1(Reader.size.__doc__).format(**vars())) from None
        pass

    def read(self, max_bytes:ByteCount) -> bytes:
        '''read up to {max_bytes} of data from current position'''
        pass
    pass


@xju.cmc.cmclass
class Writer(xju.cmc.CM):
    '''TStore {self.store} bucket {self.bucket[0]} (start {self.bucket[1]}) writer'''
    store:Store
    bucket:Tuple[BucketStart,UID]

    __impl:xju.cmc.io.FileWriter

    def __init__(self,store:Store,bucket:tuple[BucketStart,UID]):
        '''create writer for TStore {self.store}'s bucket with start {bucket[0]} and id {bucket[1]}
           - creates bucket if it does not exist yet
           - deletes any existing bucket with same start but different id
           - ensures store stays within max_buckets along the way'''
        try:
            self.store=store
            self.bucket=bucket
            try:
                self.__impl(xju.cmc.io.FileWriter(self.store.create_bucket(*self.bucket),
                                                  mode=self.store.file_creation_mode,
                                                  must_not_exist=True))
            except BucketAlreadyExists as e:
                self.__impl(xju.cmc.io.FileWriter(self.store.get_bucket(*self.bucket)))
                pass
        pass
        except Exception as e:
            raise inContext(l1(Reader.__init__.__doc__).format(**vars()))
        pass

    def seek_to(self, position:ByteCount):
        '''position TStore reader {self} so next write occurs {position} bytes from start of bucket data
           - returns self'''
        try:
            self.__impl.seek(position, io.SEEK_SET)
            return self
        except:
            raise inContext(l1(Writer.seek_to.__doc__).format(**vars())) from None
        pass
    
    def seek_by(self, offset:ByteCount):
        '''position TStore reader {self} so next write occurs {offset} bytes from current position
           - returns self'''
        try:
            self.input.seek(offset, io.SEEK_CUR)
            return self
        except:
            raise inContext(l1(Writer.seek_by.__doc__).format(**vars())) from None
        pass
    
    def size(self) -> ByteCount:
        '''return size of bucket'''
        try:
            return self.__impl.size()
        except:
            raise inContext(l1(Writer.size.__doc__).format(**vars())) from None
        pass

    def write(self, data:bytes):
        @@@
    pass

__attrs_schema=xju.json_schema.Schema({
    'hours_per_bucket':int,
    'max_buckets':int,
    'max_size':int,
    'file_creation_mode':int})

def __read_attrs(storage_path:Path) -> Tuple[Hours, BucketCount, ByteCount, FileMode]:
    '''read existing TStore {storage_path} attributes from its attrs.json file'''
    try:
        with FileReader(self.storage_path/'attrs.json') as f:
            x=json.loads(f.read().decode('utf-8'))
            __attrs_schema.validate(x)
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
        raise inContext(l1(__read_attrs.__doc__).format(**vars())) from None
    pass


        pass
                with FileWriter(self.storage_path/'hours_per_bucket.txt',
                                must_not_exist=must_not_exist,
                                mode=file_creation_mode) as f:
                    f.write(f'{hours_per_bucket}')
                    pass
                pass
            self.__buckets, self.__current_size=__read_files(storage_path,hours_per_bucket)
        except Exception as e:
            if isinstance(mode,FileMode):
                if must_not_exist:
                    raise inContext('create non-existent TStore at {storage_path} with mode 0o{file_creation_mode:o}, {hours_per_bucket} hours per bucket, max buckets {max_buckets}, max size {max_size} bytes').format(**vars())) from None
                else:
                    raise inContext('create or open existing writable (with mode 0o{file_creation_mode}) TStore at {storage_path} with {hours_per_bucket} hours per bucket, max buckets {max_buckets}, max size {max_size} bytes').format(**vars())) from None
            else:
            pass
