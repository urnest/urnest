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

class WebSocketClientProtocol:
    _ws_client: websockets.client.Protocol

    async def send_client_handshake(self, handshake: ClientHandshake) -> (
            WebSocketConnectionRejected |
            WebSocketConnectionAccepted
    ):
        ...

class WebSocketConnectionRejected:
    server_handshake: ServerHandshake
    pass

class WebSocketConnectionAccepted:
    server_handshake: ServerHandshake
    connection: WebSocketConnection
    pass


class WebSocketConnection(asyncio.Protocol):
    _ws: websockets.protocol.Protocol

    pinger: None | Future[None]

    _guard: Mutex
    _changed: Condition
    _xon: bool = True
    
    async def send_message(x: bytes, frame_size: None | ByteCount):
        async with AsyncLock(self._guard) as l:
            for frame in _ws.frames(x):
                data=frame.encode()
                while len(data):
                    while not self._xon:
                        self.changed.wait()
                        pass
                    data=data[_ws.send_some(data):]
                    pass
                pass
            pass
        pass
    async def ping(data: bytes):
        """send ping, await pong or close"""
        ...
    async def send_pong(data: bytes):
        ...
    async def send_close(code: int, why: str):
        ...
    async def recv(max_size: None | ByteCount) -> (
            bytes |
            Ping |
            Close
    ):
        ...
    async def recv(max_size: None | ByteCount, ping_handler: Callable[[bytes], Awaitable[None]]) -> (
            bytes |
            Close
    ):
        ...
    
