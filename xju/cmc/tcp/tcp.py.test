

@dataclass
class Scope:
    s:str
    def __enter__(self):
        print(f'+  {self.s}')
        return self
    def __exit__(self, t, e, b):
        print(f'-{"E" if e else " "} {self.s}')
        return self
    
async def client(tcp_port:int, deadline:time.Monotonic):
    with Scope(f'client on port {tcp_port}'):
        with xju.tcp.Socket('localhost', tcp_port) as s:
            with Scope(f'wait until connected to service'):
                await s.writable(deadline)
                pass
            with Scope('send "fred" to service'):
                await s.send(deadline, b'fred')
                pass
            with Scope('await "jock" from service'):
                xju.Assert(await s.read_n(deadline, 4))==b'jock'
                pass
            pass
        pass
    pass

async def main():
    deadline = time.monotonic()+5.0
    with xju.tcp.Service('localhost') as service:
        with xju.Task(loop, client(service.port, deadline)) as task:
            with Scope(f'await client connection on port {tcp_port}'):
                await service.readable(deadline)
                pass
            with xju.tcp.Socket(service) as s:
                with Scope('await "fred" from client'):
                    xju.Assert(await s.read_n(deadline, 4))==b'fred'
                    pass
                with Scope('send "jock" to client'):
                    await s.send(deadline, b'jock')
                    pass
                with Scope('await client finish'):
                    await task.finish(deadline)
                    pass
                pass
            pass
        pass
    pass

asyncio.run(main)
