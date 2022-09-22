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

# python io lib equivalents that provide proper context management, i.e.
# constructors that do not acquire resources and hence do not throw
# (the __enter__ methods acquire resources, e.g. open files)

import os
from typing import overload
import typing
from typing import Literal, Optional, Union
import pathlib
import io
import contextlib
import fcntl
from dataclasses import dataclass
from xju.xn import in_function_context, first_line_of as l1
from xju.misc import ByteCount
from xju.newtype import Int

class FilePositionTag:pass
class FilePositionDeltaTag:pass
class FileModeTag:pass
class FilePosition(Int[FilePositionTag]):pass
class FilePositionDelta(Int[FilePositionDeltaTag]):pass
class FileMode(Int[FileModeTag]):pass

class FileReader(contextlib.AbstractContextManager):
    '''{self.path} reader with close-on-exec {self.close_on_exec}'''
    path:pathlib.Path
    close_on_exec:bool
    input:io.RawIOBase

    def __init__(self, path: pathlib.Path, close_on_exec:bool=True):
        self.path = path
        self.close_on_exec = close_on_exec
        pass
    
    def __str__(self):
        return l1(FileReader.__doc__).format(**vars())

    def __enter__(self):
        '''open {self}'''
        try:
            flags=os.O_RDONLY
            if self.close_on_exec:
                if getattr(os, 'O_CLOEXEC'):
                    flags=flags|os.O_CLOEXEC
            self.__fd = os.open(self.path, flags, 0)
            self.input = io.FileIO(self.__fd, closefd=False)
            return self
        except Exception:
            raise in_function_context(FileReader.__enter__,vars()) from None
        pass

    def __exit__(self, t, e, b):
        '''close {self}'''
        try:
            del self.input
            os.close(self.__fd)
            del self.__fd
        except Exception:
            raise in_function_context(FileReader.__exit__,vars()) from None
        pass

    def seek_to(self, position:FilePosition):
        '''position so next read occurs {position} bytes from start of file
           - returns self'''
        try:
            self.input.seek(int(position), io.SEEK_SET)
            return self
        except Exception:
            raise in_function_context(FileReader.seek_to,vars()) from None
        pass
    
    def seek_by(self, offset:FilePositionDelta):
        '''position so next read occurs {offset} bytes from current position
           - returns self'''
        try:
            self.input.seek(int(offset), io.SEEK_CUR)
            return self
        except Exception:
            raise in_function_context(FileReader.seek_by,vars()) from None
        pass
    
    def size(self) -> ByteCount:
        '''return size of file'''
        try:
            return ByteCount(os.fstat(self.__fd).st_size)
        except Exception:
            raise in_function_context(FileReader.size,vars()) from None
        pass

    def position(self) -> FilePosition:
        '''get current position'''
        try:
            return FilePosition(self.input.seek(0, io.SEEK_CUR))
        except Exception:
            raise in_function_context(FileReader.position,vars()) from None
        pass

    def fd(self) -> int:
        return self.__fd
    pass


class FileWriter(contextlib.AbstractContextManager):
    '''file writer for {self.path} with with create mode {self.mode}, must not exist {self.must_not_exist}, close-on-exec {self.close_on_exec}'''
    path:pathlib.Path
    mode:Optional[FileMode]
    must_not_exist:bool
    close_on_exec:bool
    output:io.RawIOBase

    @overload
    def __init__(self, path: pathlib.Path, mode:FileMode, must_not_exist:Literal[True]=True, close_on_exec:bool=True):
        '''non-existent {path} writer creating with mode {mode}, with close-on-exec {close_on_exec}
           - raises FileExistsError if {path} exists'''
        pass
    @overload
    def __init__(self, path: pathlib.Path, mode:Literal[None]=None, must_not_exist:Literal[False]=False, close_on_exec:bool=True):
        '''existing {path} writer, with close-on-exec {close_on_exec}'''
        pass
    @overload
    def __init__(self, path: pathlib.Path, mode:FileMode, must_not_exist:Literal[False], close_on_exec:bool):
        '''{path} writer creating with mode {mode} if {path} does not exist, with close-on-exec {close_on_exec}'''
        pass
    def __init__(self, path: pathlib.Path, mode=None, must_not_exist=False, close_on_exec=True):
        self.path = path
        self.mode = mode
        self.must_not_exist = must_not_exist
        self.close_on_exec = close_on_exec
        pass

    def __str__(self):
        return l1(FileWriter.__doc__).format(**vars())

    def __enter__(self):
        '''open {self}
           - returns self'''
        try:
            if self.mode is None:
                flags=os.O_WRONLY
            elif self.must_not_exist:
                flags=os.O_WRONLY|os.O_CREAT|os.O_EXCL
            else:
                flags=os.O_WRONLY|os.O_CREAT
            if self.close_on_exec:
                if getattr(os, 'O_CLOEXEC'):
                    flags=flags|os.O_CLOEXEC
                    pass
                pass
            self.__fd = os.open(self.path, flags, int(self.mode or 0))
            self.output = io.FileIO(self.__fd, mode='w', closefd=False)
            return self
        except Exception:
            raise in_function_context(FileWriter.__enter__,vars()) from None
        pass
    
    def __exit__(self, t, e, b):
        '''close {self}'''
        try:
            del self.output
            os.close(self.__fd)
            del self.__fd
        except Exception:
            raise in_function_context(FileWriter.__exit__,vars()) from None
        pass
    
    def seek_to(self, position:FilePosition):
        '''position so next write occurs {position} bytes from start of file
           - returns self'''
        try:
            self.output.seek(int(position), io.SEEK_SET)
            return self
        except:
            raise in_function_context(FileWriter.seek_to,vars()) from None
        pass
    
    def seek_by(self, offset:FilePositionDelta):
        '''position so next write occurs {offset} bytes from current position
           - returns self'''
        try:
            self.output.seek(int(offset), io.SEEK_SET)
            return self
        except:
            raise in_function_context(FileWriter.seek_by,vars()) from None
        pass
    
    def size(self) -> ByteCount:
        '''return size of file'''
        try:
            return ByteCount(os.fstat(self.__fd).st_size)
        except:
            raise in_function_context(FileWriter.size,vars()) from None
        pass

    def position(self) -> ByteCount:
        '''get current position'''
        try:
            return ByteCount(self.output.seek(0, io.SEEK_CUR))
        except Exception:
            raise in_function_context(FileWriter.position,vars()) from None
        pass

    def fd(self) -> int:
        return self.__fd
    pass

@dataclass
class FileLock(contextlib.AbstractContextManager):
    '''non-blocking "flock" lock {self.target}'''
    target:Union[FileReader,FileWriter]
    def __init__(self, target:Union[FileReader,FileWriter]):
        self.target = target
        pass

    def __str__(self):
        return l1(FileLock.__doc__).format(**vars())

    def __enter__(self):
        '''acquire {self}'''
        try:
            fcntl.flock(self.target.fd(), fcntl.LOCK_EX|fcntl.LOCK_NB)
            return self
        except Exception:
            raise in_function_context(FileLock.__enter__,vars()) from None
        pass

    def __exit__(self, t, e, b):
        '''release {self}'''
        try:
            fcntl.flock(self.target.fd(), fcntl.LOCK_UN)
        except Exception:
            raise in_function_context(FileLock.__exit__,vars()) from None
