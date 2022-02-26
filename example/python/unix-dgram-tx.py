#!/usr/bin/env python3

# can't really use asyncio because send does not block and there is no way to tell
# when it is done... maybe there is using await?
import socket
import sys

socket_file, = sys.argv[1:]

s = socket.socket(family=socket.AF_UNIX, type=socket.SOCK_DGRAM)
# connect opens existing file
s.connect(socket_file)
s.send('hello'.encode())
