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

from asyncio import wait, FIRST_COMPLETED, TaskGroup
from tempfile import TemporaryDirectory

from xju.cmc import AsyncTask, AsyncMutex, AsyncCondition, AsyncDict
from xju.cmc.sockets import UnixSocketAddress, StreamListener, ConnectedStreamSocket
from xju.newtype import Int
from xju.time import monotonic_now, Duration

class RunnerId(Int["RunnerIdTag"]): pass

async def server(listeners: Sequence[StreamListener[UnixSocketAddress]]):
    next_runner_id = RunnerId(1)
    m = AsyncMutex()
    done = list[RunnerId]
    c = AsyncCondition(m)
    def runner(runner_id: RunnerId, connection: ConnectedStreamSocket[UnixSocketAddress]):
        try:
            await run_socket_till_done(connection)
        except Exception as e:
            print(f"run failed because {e}")
            pass
        async with Lock(m) as l:
            done.append(runner_id)
            c.notify_all(l)
            pass
        pass

    @async_cm
    @dataclass
    class ConnectionAndRunner:
        socket: ConnectedStreamSocket[UnixSocketAddress]
        runner: AsyncTask
        pass
    
    async with AsyncDict[RunnerId, SocketAndRunner]() as connections:
        async with AsyncLock(m) as l:
            # so we can be forced into a dead loop by DOS client aborting queued
            # connection, we won't accept more often than:
            secs_between_accepts = 0.05
            last_accept = monotonic_now() - Duration(0.05)
            while True:
                while len(done):
                    runner_id = done.pop()
                    await connections.pop(runner_id)
                    pass
                # delay new connections more with number of current connections
                dont_accept_before = last_accept + Duration(0.05) + Duration(1) * len(runners) * len(runners)
                now = monotonic_now()
                if now < dont_accept_before:
                    # can't accept yet
                    await c.wait_for(l, dont_accept_before - now)
                else:
                    # been long enough, can accept more...
                    socket = ConnectedStreamSocket[UnixSocketAddress](listeners)
                    connection = SocketAndRunner(socket,
                                                 AsynTask(partial(runner, next_runner_id, socket)))
                    with (
                        # ... but while we're waiting, clean up done runners
                        AsyncTask(partial(c.wait_for, l, Duration(60*60))) as t1,
                        AsyncTask(partial(connections.set,
                                          next_runner_id,
                                          connection)) as t2
                    ):
                        await wait([t1, t2], return_when=FIRST_COMPLETED)
                        if t2.done:
                            last_accept = monotonic_now()
                            if e = t2.exception():
                                print(f"failed to accept pending connection because {e}")
                            else:
                                next_runner_id += 1
                            pass
                        if t1.done:
                            assert t1.exception() is None, t1.exception()
                        pass
                    pass
                pass
            pass
        pass
    pass

async def client(client_id: int, addresses: list[UnixSocketAddress]):
    connect_to = addresses[client_id % len(addresses)]
    try:
        async with Timeout(20):
            async with ConnectedStreamSocket(connect_to) as connection:
                ... do the stuff
                pass
            pass
        pass
    except Exception as e:
        print(f"client {client_id} failed because {e}")
        pass
    pass

async def main():
    with (
            TemporaryDirectory() as d,
            StreamListener(UnixSocketAddress(d / 'socket_a'), 2) as listener_a,
            StreamListener(UnixSocketAddress(d / 'socket_b'), 2) as listener_b,
    ):
        async with AsyncTask(partial(server, [listener_a, listener_b])):
            async with TaskGroup() as g:
                for i in range(1, int(sys.argv[1])+1):
                    g.create_task(partial(client, i, [
                        listener_a.local_address,
                        listener_b.local_address]))
                    pass
                pass
            pass
        pass
    pass
pass

class RunnerIdTag: pass
