#!/usr/bin/env python3

import sys
import asyncio
import socket

class Protocol:
    def connection_made(self, transport):
        print(f'Socket opened, transport {transport}')
        self.transport = transport

    def datagram_received(self, data, addr):
        message = data.decode()
        print(f'Received %{message!r} from {addr}')

socket_file, = sys.argv[1:]

async def main(prog):
    print(prog)

    # Get a reference to the event loop as we plan to use
    # low-level APIs.
    loop = asyncio.get_running_loop()

    # One protocol instance will be created to serve all
    # client requests.
    transport, protocol = await loop.create_datagram_endpoint(
        lambda: Protocol(),
        local_addr=socket_file, family=socket.AF_UNIX)

    try:
        await asyncio.sleep(3600)  # Serve for 1 hour.
    finally:
        transport.close()


asyncio.run(main(sys.argv[0]))
