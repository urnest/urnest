'''async/await websocket'''

'''scenarios:
- transport connect fails
- transport connect times out
- handshake fails
- handshake timesout
- receive message:
  - timeout 
  - close received
  - message received
- send message
  - after other end closes
  - after this end close sent
  - good

want:
with xju.time.steadyNow()+10 as deadline,
     TLSWebSocket(loop,deadline,host,port,headers={}) as c:
  c.send_message('xxx'.encode('utf-8'))
  xju.Assert(c.receive_message(deadline)=='yyy'.encode('utf-8')
  pass
'''

from typing import Optional

from autobahn.asyncio.websocket import WebSocketClientProtocol, WebSocketClientFactory
from asyncio import AbstractEventLoop
from autobahn.websocket.types import TransportDetails

from xju.time import Timepoint as Deadline
from xju import inContext

Message=bytes

class TLSWebSocket(Connection):
    class TransportDetails(AbstractTransportDetails):
        def __init__(self, x:TransportDetails):
            self.x = x
            pass
        pass

    def __init__(self,loop,deadline,host,port,headers:Headers={}):
        self.host = host
        self.port = port
        self.headers = headers
        self.transportDetails:Optional[TransportDetails] = None
        Connection.__init__(self,loop,deadline,self._connect_via,headers)
        pass

    def __enter__(self) -> TLSWebSocket:
        try:
            Connection.__enter__(self)
            return self
        except Exception as e:
            raise inContext(
                f'connect websocket over TLS to {self.host}:{self.port} with headers {self.headers}') from e
        pass

    def __str__(self):
        return f'websocket over TLS to {self.host}:{self.port} with headers {self.headers}'

    async def _connect_via(
            self,
            loop:AbstractEventLoop,
            deadline:Deadline) -> Tuple[_Protocol,AbstractTransportDetails]:
        '''connect {self} via {loop} by {deadline}'''
        try:
            protocol = _ClientProtocol(loop)
            connected = loop.create_future()
            def on_transport_connected(transportDetails:TransportDetails):
                protocol.clear_handlers()
                connected.set_result(TLSWebSocket.TransportDetails(
                    transportDetails))
                pass
            def on_close(was_clean:bool, code:Optional[int], reason:Optional[str]):
                protocol.clear_handlers()
                if was_clean:
                    raise Exception(f'unexpected "clean" websocket closure apparently before connection established, noting code {code} and reason {reason}')
                else:
                    connected.set_exception(
                        Exception(f'closed before connected, WebSocketProtocol onClose code {code}, reason {reason!r}'))
                    pass
                pass
            protocol.replace_handlers(
                on_transport_connected,
                protocol._unexpected_message,
                on_close)

            class F(WebSocketClientFactory):
                def __init__(self,openingHandshakeTimeout:float):
                    super().__init__(self)
                    self.setOpeningHandshakeTimeout(openingHandshakeTimeout)
                    self.setHeaders(headers)
                    pass
                def buildProtocol(self):
                    protocol.factory = self
                    return protocol
                pass

            loop.connectSSL(self.host,self.port,F(deadline.remaining()))
            transportDetails = await connected
            return (protocol, transportDetails)
        except Exception as e:
            raise inContext(l1(TLSConnector.connectVia.__doc__).format(**vars())) from e
        pass
    pass
class Connector():
    '''see e.g. TLSConnector below'''
    pass

class AbstractTransportDetails(Object):
    pass

class Connection():
    class Closed(Exception):
        def __init__(self,s):
            Exception.__init__(self,s)
            pass
        pass

    def __init__(self,
                 loop:AbstractEventLoop,
                 deadline:Deadline,
                 connect_via:Callable[[AbstractEventLoop,Deadline],
                                      Tuple[_Protocol,
                                            AbstractTransportDetails]],
                 headers={}):
        self.deadline:Deadline = deadline
        self.loop:AbstractEventLoop = loop
        self.connect_via = connect_via
        self.headers = headers
        self._protocol:Optional[_Protocol] = None
        self._transportDetails:Optional[AbstractTransportDetails] = None
        pass
    
    async def __enter__(self) -> Connection:
        self._protocol, self._transportDetails = await self.connect_via(
            self.loop,self.deadline)
        return self
    
    async def __exit__(self,t,e,b) -> None:
        if not self._protocol.closed:
            self._protocol.abort()
            pass
        pass
    
    async def receive_message(self, deadline:Deadline) -> Message:
        result:Message = Message()
        future = self.loop.create_future()
        def deadline_reached(self):
            self._protocol.clear_handlers()
            result.set_exception(
                Timeout(f'deadline reached before message received'))
            pass
        timer = self.loop.callLater(deadline.remaining(),deadline_reached)
        def on_message(message:Message):
            self._protocol.clear_handlers()
            timer.cancel()
            result = message
            future.set_result(None)
            pass
        def on_close(was_clean:bool, code:Optional[int], reason:Optional[str]):
            self._protocol.clear_handlers()
            timer.cancel()
            cleanly = 'cleanly' if clean else 'uncleanly'
            result.set_exception(
                Connection.Closed(
                    f'connection closed {cleanly} before message received, WebSocketProtocol code {code} and reason {reason!r}'))
            pass
        self._protocol.replace_handlers(
            self._unexpected_transport_connected,
            on_message,
            on_close)
        await future
        return result

    def send_message(self, message:Message) -> None:
        '''send Message {message} to {self}'s peer'''
        try:
            self._protocol.sendMsg(message)
        except Exception as e:
            raise inContext(
                l1(Connection.send_message.doc.format(**vars()))) from e
        pass

    async def close(self, deadline:Deadline) -> Sequence[Message]:
        '''close connection to peer {self.peer} by {deadline}, returning any intervening received messages'''
        try:
            intervening_messages = []
            if not protocol.closed:
                protocol.setOptions(
                    closingHandshakeTimeout=deadline.remaining())
                result = self.loop.create_future()
                def on_message(message:Message):
                    intervening_messages.append(message)                    
                    pass
                def on_close(was_clean:bool, code:Optional[int], reason:Optional[str]):
                    self._protocol.clear_handlers()
                    result.set_result()
                    pass
                protocol.replace_handlers(
                    protocol._unexpected_transport_connected,
                    on_message,
                    on_close)
                protocol.sendClose()
                await result
                pass
            return intervening_messages
        except Exception as e:
            raise inContext(l1(Connection.close.__doc__.format(**vars())))) from e
            pass
        pass
    pass

class _Protocol():
    OnTransportConnected=Callable[ [TransportDetails],None ]
    OnMessage=Callable[ [Message],None ]
    OnClose=Callable[ [bool,Optional[int],Optional[str]],None ]
    def __init__(self, loop:AbstractEventLoop, peer:str):
                    
        self.loop = loop
        self.peer = peer
        self._on_transport_connected:OnTransportConnected = self._unexpected_transport_connected
        self._on_message:Protocol.OnMessage = self._unexpected_message
        self._on_close:Protocol.OnClose = self._unexpected_close
        self.closed = False
        pass
    def replace_handlers(self,
                         on_transport_connected:_Protocol.OnTransportConnected,
                         on_message:_Protocol.OnMessage,
                         on_close:_Protocol.OnClose)->None:
        self._on_message = on_message
        self._on_close = on_close
        pass
    def clear_handlers(self)->None:
        self._on_transport_connected:_Protocol.OnTransportConnected = self._unexpected_transport_connected
        self._on_message:_Protocol.OnMessage = self._unexpected_message
        self._on_close:_Protocol.OnClose = self._unexpected_close
        pass

    def _unexpected_transport_connected(self,transport_details:TransportDetails)->None:
        raise Exception(f'transport connected {transport_details} when no callback registered')
    
    def _unexpected_message(self,message:Message)->None:
                    
        raise Exception(f'received message {message} from {self.peer} when no callback registered')
        pass

    def _unexpected_close(self,
                          was_clean:bool,
                          code:Optional[int],
                          reason:Optional[str])->None:
        raise Exception(f'received close (clean={was_clean}, code={code}, reason={reason!r}) from {self.peer} when no callback registered')
        pass

    def onConnecting(self, transport_details:TransportDetails):
        self._on_transport_connected(transport_details)
        pass
    def onMessage(self, message:Message)->None:
        self._on_message(message)
        pass
    def onClose(was_clean:bool,code:Optional[int],reason:Optional[str])->None:
        self.closed = True
        self._on_close(was_clean,code,reason)
        pass
    pass

class _ClientProtocol(WebSocketClientProtocol,_Protocol):
    def __init__(self, loop:AbstractEventLoop, peer:str):
        _Protocol.__init__(self, loop, peer)
        WebSocketClientProtocol.__init__(self)
        pass
    pass

