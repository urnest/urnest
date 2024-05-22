# Copyright (c) 2024 Trevor Taylor
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
# socket wrappers with proper context management
#
from asyncio import FIRST_COMPLETED, get_event_loop, TaskGroup, wait
from dataclasses import dataclass, field
from functools import partial
from pathlib import Path
from typing import (
    Protocol,
    Generic,
    ClassVar,
    Self,
    Sequence,
    Any,
    TypeVar,
    overload,
    cast,
    runtime_checkable
)
import socket

from xju.cmc import Opt, cmclass, async_cmclass, AsyncDict, AsyncTask
from xju.newtype import Int
from xju.xn import in_function_context

class ListenerBacklog(Int["ListenerBacklogTag"]):
    """
    accept(2) backlog

    - note backlog 0 allows 1 pending connection but rejects further pending
      connections with Resource temporarily unavailable (errno 11)
    """
    pass

@runtime_checkable
class AddressTypeProto(Protocol):
    family: ClassVar[int]  # e.g. socket.AF_INET

    @staticmethod
    def abort(s: socket.socket) -> None: ...

    @classmethod
    def get_local_address(cls, s: socket.socket) -> Self: ...

    @classmethod
    def get_remote_address(cls, s: socket.socket) -> Self: ...

    @property
    def sockaddr(self) -> Any: ...

    @classmethod
    def from_sockaddr(cls, sockaddr: Any) -> Self: ...

    pass

AddressType = TypeVar('AddressType', bound=AddressTypeProto)

# some socket families, not exhaustive - see socket(7)
@dataclass
class UnixAddressType(AddressTypeProto):
    family:ClassVar[int] = field(init=False, default=socket.AF_UNIX)

    path: Path

    def __str__(self) -> str:
        return f"unix socket {self.path}"

    @property
    def sockaddr(self) -> Any:
        return str(self.path)
    
    @classmethod
    def from_sockaddr(cls, sockaddr: Any) -> "UnixAddressType":
        assert isinstance(sockaddr, str)
        return UnixAddressType(Path(sockaddr))

    @staticmethod
    def abort(s: socket.socket) -> None:
        s.close()

    @classmethod
    def get_local_address(cls, s: socket.socket) -> "UnixAddressType":
        return UnixAddressType.from_sockaddr(s.getsockname())

    @classmethod
    def get_remote_address(cls, s: socket.socket) -> "UnixAddressType":
        return UnixAddressType.from_sockaddr(s.getpeername())
    
    pass

@dataclass
class StreamSocket(Generic[AddressType]):
    socket_type: type[AddressType]
    close_on_exec: bool = True
    _socket: None | socket.socket = field(init=False, default=None)  # until entered

    def __enter__(self) -> Self:
        "create a {self.socket_type.__name__} socket (family {self.socket_type.family}) with close-on-exec {self.close_on_exec}"
        try:
            flags=int(socket.SOCK_NONBLOCK)
            if self.close_on_exec:
                flags=flags|socket.SOCK_CLOEXEC
            self._socket = socket.socket(self.socket_type.family,type=socket.SOCK_STREAM|flags)
            return self
        except Exception as e:
            raise in_function_context(StreamSocket.__enter__, vars()) from None
        pass
    
    def __exit__(self, *_) -> None:
        self.socket_type.abort(self.socket)
        pass

    @property
    def socket(self) -> socket.socket:
        result = self._socket
        assert result is not None
        return result

    async def until_readable(self) -> Self:
        "wait until {self} is readable"
        loop = get_event_loop()
        fd = self.socket.fileno()
        f = loop.create_future()
        loop.add_reader(fd,partial(f.set_result, None))
        try:
            await f
            return self
        finally:
            loop.remove_reader(fd)
            f.cancel()

    async def until_writable(self) -> Self:
        "wait until {self} is writable"
        loop = get_event_loop()
        fd = self.socket.fileno()
        f = loop.create_future()
        loop.add_writer(fd,partial(f.set_result, None))
        try:
            await f
            return self
        finally:
            loop.remove_writer(fd)
            f.cancel()
    pass

@cmclass
class StreamSocketListener(Generic[AddressType]):
    address: AddressType
    backlog: ListenerBacklog
    socket: StreamSocket[AddressType]

    def __str__(self):
        return f"stream socket listener on {self.address} with backlog {self.backlog}"

    def __init__(self, address: AddressType, backlog: ListenerBacklog, close_on_exec=True):
        self.address = address
        self.backlog = backlog
        self.socket = StreamSocket[AddressType](address.__class__, close_on_exec)

    def __enter__(self) -> Self:
        "{self} start listening"
        try:
            self.socket.socket.bind(self.address.sockaddr)
            self.socket.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.socket.socket.listen(self.backlog.value())
            return self
        except Exception:
            raise in_function_context(StreamSocketListener.__enter__, vars()) from None
        pass

    async def until_incoming_connection(self) -> Self:
        "wait until {self} has an incoming connection available"
        await self.socket.until_readable()
        return self
    pass

@async_cmclass
class StreamSocketConnection(Generic[AddressType]):
    """
    connected socket context manager

    - constructor variants provide server side (accept)
      and client side (connect)
    - use self.socket with e.g. asyncio stream reader/writer,
      start_tls, protocols like websockets.WebSocketClientProtocol
      and websockets.ServerProtocol
    - no resources are acquired until context entry
    """

    _local_address: AddressType | None = None
    _remote_address: AddressType | None = None
    _s: AddressType | StreamSocketListener[AddressType]
    _socket: Opt[StreamSocket[AddressType]]
    close_on_exec: bool

    @overload
    def __init__(self, listener: StreamSocketListener[AddressType]):
        "at context entry, await and accept a connection from {listener}"

    @overload
    def __init__(self, listeners: Sequence[StreamSocketListener[AddressType]]):
        """
        at context entry, await and accept first connection from {listeners}
        
         - context entry will wait forever if listeners is empty
        """

    @overload
    def __init__(self, remote_address: AddressType, close_on_exec:bool = True):
        "at context entry, await and connect to {remote_address}"


    def __init__(self, s, close_on_exec=True):
        self._s = s
        self._socket = Opt()
        self.close_on_exec=close_on_exec

    async def __aenter__(self) -> Self:
        match self._s:
            case AddressTypeProto():
                st = cast(AddressType, self._s)
                self._socket.set(StreamSocket[AddressType](type(st), self.close_on_exec))
                self.socket.socket.connect(self._s.sockaddr)
                await self.socket.until_writable()
                self._local_address = st.get_local_address(self.socket.socket)
                self._remote_address = st.get_remote_address(self.socket.socket)
            case StreamSocketListener():
                await self._s.until_incoming_connection()
                self._socket.set(_AcceptedStreamSocketConnection[AddressType](
                    self._s.address.__class__, self._s.socket.socket.accept()[0], self.close_on_exec))
                self._local_address = self._s.address.get_local_address(self.socket.socket)
                self._remote_address = self._s.address.get_remote_address(self.socket.socket)
            case _:
                async def forever():
                    await get_event_loop().create_future()
                    pass
                tasks = [
                    AsyncTask(listener.until_incoming_connection)
                    for listener in self._s
                ] or [
                    AsyncTask(forever)
                ]
                async with AsyncDict[int, AsyncTask]({i: task for i,task in enumerate(tasks)}) as group:
                    done, pending = await wait([t.task for t in tasks], return_when=FIRST_COMPLETED)
                    assert len(done) > 0, (done, pending)
                    done_listener = done.pop().result()
                    assert isinstance(done_listener, StreamSocketListener)
                    self._socket.set(_AcceptedStreamSocketConnection[AddressType](
                        done_listener.address.__class__,
                        done_listener.socket.socket.accept()[0],
                        self.close_on_exec))
                    self._local_address = done_listener.address.get_local_address(self.socket.socket)
                    self._remote_address = done_listener.address.get_remote_address(self.socket.socket)
                    pass
            
        return self

    # properties valid in context
    @property
    def socket(self) -> StreamSocket[AddressType]:
        result = self._socket.get()
        assert result is not None
        return result

    @property
    def local_address(self) -> AddressType:
        assert self._local_address is not None
        return self._local_address

    @property
    def remote_address(self) -> AddressType:
        assert self._remote_address is not None
        return self._remote_address

    pass

class _AcceptedStreamSocketConnection(StreamSocket[AddressType], Generic[AddressType]):
    def __init__(self, socket_type: type[AddressType], s: socket.socket, close_on_exec:bool):
        super().__init__(socket_type, close_on_exec)
        self._socket = s
        pass

    def __enter__(self) -> Self:
        return self
    pass



# @cmclass
# class DatagramSocket(Generic[AddressType]):
#     _local_address: None | AddressType
#     _address_type: type[AddressType]
#     _socket: Opt[socket.socket]

#     @overload
#     def __init__(self, local_address: AddressType) -> None: ...
#     @overload
#     def __init__(self, address_type: type[AddressType]) -> None: ...

#     def __init__(self, address_or_address_type) -> None:
#         if isinstance(address_or_address_type, AddressTypeProto):
#             self.local_address = address_or_address_type
#             self.address_type = type(address_or_address_type)
#         else:
#             self.local_address = None
#             self.address_type = address_type

#     def __enter__(self):
#         self._socket = socket.socket(self.address_type.family, SOCK_DRAM)
#         if self._local_address is not None:
#             self._local_address = AddressType.

class ListenerBacklogTag:
    pass
