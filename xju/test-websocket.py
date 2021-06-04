from xju.assert_ import Assert
from xju.websocket import TLSWebSocket, TLSWebSocketServer, Connection
from xju.thread import Thread
from asyncio import run
from xju.time import steadyNow, SteadyTime

deadline = (steadyNow()+20).seconds_until_passed

async def test_good():
    with TLSWebSocketServer(loop, 'localhost') as server:

        def run_server():
            with await server.accept_connection(deadline) as connection:
                x = await websocket.receive_message(deadline)
                xju.Assert(x) == b'hello'
                websocket.send_message(b'hello too')
                pass
            pass

        t1 = Thread(run_server)
        async with t1:
            async with TLSWebSocket(loop, deadline, 'localhost', server.port) as client:
                client.send_message(b'hello')
                x = await client.receive_message(deadline)
                xju.Assert(x) == b'hello too'
                extra_messages = await client.close(deadline)
                xju.Assert(extra_messages) == []
                pass
            pass
        xju.Assert(t1.result()) == None

xju.Assert(asyncio.run(test_good)) == None
