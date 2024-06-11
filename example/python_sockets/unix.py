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

"""
 - listen on two paths, backlog 2
 - variable number of accepter (runner) tasks
 - show how to delay accept according to number already
 - show how to do tls+websockets
 - pass number of clients on command line
"""

from asyncio import wait, FIRST_COMPLETED, TaskGroup, get_event_loop, Timeout
from dataclasses import dataclass
from functools import partial
from pathlib import Path
import sys
from tempfile import TemporaryDirectory
from typing import Sequence
import ssl
from xju.cmc.sockets import WebSocketServerProtocol, WebSocketClientProtocol
from xju.assert_ import Assert
from xju.cmc import AsyncTask, AsyncMutex, AsyncCondition, AsyncDict, async_cmclass, AsyncLock
from xju.cmc.sockets import UnixAddressType, StreamSocketListener, StreamSocketConnection, ListenerBacklog
from xju.newtype import Int
from xju.time import now, Duration

class RunnerId(Int["RunnerIdTag"]): pass

async def server(listeners: Sequence[StreamSocketListener[UnixAddressType]]):
    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    ssl_context.load_cert_chain(Path(__file__).with_name("cert.pem"),
                                Path(__file__).with_name("key.pem"))
    next_runner_id = RunnerId(1)
    m = AsyncMutex()
    done = list[RunnerId]()
    c = AsyncCondition(m)
    async def runner(runner_id: RunnerId, connection: StreamSocketConnection[UnixAddressType]):
        try:
            def create_websocket_server_protocol():
                return WebSocketServerProtocol()

            websocket_protocol: WebSocketServerProtocol
            _transport, websocket_connection = await get_event_loop().connect_accepted_socket(
                create_websocket_server_protocol,
                sock=connection.socket.socket,
                ssl=ssl_context,
                ssl_handshake_timeout=5,
                ssl_shutdown_timeout=5)

            client = await websocket_connection.receive_client_handshake()
            connection = await client.send_server_accept()
            assert (await connection.recv()) == 'fred'
            await connection.send('jones')
            assert (await connection.recv()) == 'ack'
            await connection.close()

        except Exception as e:
            print(f"run failed because {e}")
            pass
        async with AsyncLock(m) as l:
            done.append(runner_id)
            c.notify_all(l)
            pass
        pass

    @async_cmclass
    @dataclass
    class ConnectionAndRunner:
        socket: StreamSocketConnection[UnixAddressType]
        runner: AsyncTask
        pass
    
    async with AsyncDict[RunnerId, ConnectionAndRunner]() as connections:
        async with AsyncLock(m) as l:
            # so we can be forced into a dead loop by DOS client aborting queued
            # connection, we won't accept more often than:
            secs_between_accepts = 0.05
            last_accept = now() - Duration(0.05)
            while True:
                while len(done):
                    runner_id = done.pop()
                    await connections.pop(runner_id)
                    pass
                # delay new connections more with number of current connections
                dont_accept_before = (
                    last_accept + Duration(0.05) + Duration(1) * len(connections) * len(connections)
                )
                t = now()
                if t < dont_accept_before:
                    # can't accept yet
                    await c.wait_for(l, dont_accept_before - t)
                else:
                    # been long enough, can accept more...
                    socket = StreamSocketConnection[UnixAddressType](listeners)
                    connection = ConnectionAndRunner(socket,
                                                     AsyncTask(partial(runner, next_runner_id, socket)))
                    async with (
                        # ... but while we're waiting, clean up done runners
                        AsyncTask(partial(c.wait_for, l, Duration(60*60))) as t1,
                        AsyncTask(partial(connections.set,
                                          next_runner_id,
                                          connection)) as t2
                    ):
                        await wait([t1, t2], return_when=FIRST_COMPLETED)
                        if t2.done():
                            last_accept = now()
                            if e := t2.exception():
                                print(f"failed to accept pending connection because {e}")
                            else:
                                next_runner_id += RunnerId(1)
                            pass
                        if t1.done():
                            assert t1.exception() is None, t1.exception()
                        pass
                    pass
                pass
            pass
        pass
    pass

async def client(client_id: int, addresses: list[UnixAddressType]):
    connect_to = addresses[client_id % len(addresses)]
    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    ssl_context.load_verify_locations(Path(__file__).with_name("cert.pem"))
    try:
        async with Timeout(20):
            async with StreamSocketConnection(connect_to) as connection:
                websocket_protocol: WebSocketClientProtocol
                _transport, websocket_protocol = await get_event_loop().create_connection(
                    WebSocketClientProtocol,
                    sock=connection.socket.socket,
                    ssl=ssl_context,
                    server_hostname='localhost',
                    ssl_handshake_timeout=5,
                    ssl_shutdown_timeout=5)
                match await websocket_protocol.send_client_handshake():
                    WebSocketConnectionRejected() as r:
                        print(f"websocket connection {client_id} rejected by server because {r}")
                        return
                    WebSocketConnectionAccepted() as acceptance:
                        print(f"websocket connection accepted with server handshake {acceptance.handshake}")
                        connection=acceptance.connection
                        await connection.send('fred')
                        Assert(await connection.recv()) == 'jones'
                        websocket_connection.send('ack')
                        websocket_connection.wait_closed()
                pass
            pass
        pass
    except Exception as e:
        print(f"client {client_id} connecting to {connect_to} failed because {e}")
        pass
    pass

async def main():
    with (
            TemporaryDirectory() as d,
            StreamSocketListener(UnixAddressType(Path(d) / 'socket_a'), ListenerBacklog(2)) as listener_a,
            StreamSocketListener(UnixAddressType(Path(d) / 'socket_b'), ListenerBacklog(2)) as listener_b,
    ):
        async with AsyncTask(partial(server, [listener_a, listener_b])):
            async with TaskGroup() as g:
                for i in range(1, int(sys.argv[1])+1):
                    g.create_task(client(i, [
                        listener_a.address,
                        listener_b.address]))
                    pass
                pass
            pass
        pass
    pass
pass

class RunnerIdTag: pass

# openssl req -x509 -nodes -new -sha256 -days 20000 -newkey rsa:2048 -keyout key.pem -out cert.pem -subj "/C=US/CN=localhost"
# cat key.pem cert.pem > key_cert.pem
