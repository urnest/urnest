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

from typing import Optional, Union, Callable
from collections.abc import Awaitable

from autobahn.asyncio.websocket import WebSocketClientProtocol, WebSocketClientFactory
from asyncio import AbstractEventLoop
from autobahn.websocket.types import TransportDetails as _TransportDetails

from xju import inContext

Deadline=Callable[ [], float]  # seconds until deadline, or 0 if passed
Message=Union[bytes,                              # binary message
              int, float, str, bool, dict, list]  # json-decoded message

class Timeout(Exception):
    pass

class AbstractTransportDetails(Object):
    pass

class TLSTransportDetails(AbstractTransportDetails):
    def __init__(self, x:_TransportDetails):
        self.x:_TransportDetails = x
        pass
    pass

class TLSWebSocket(Connection):
    'websocket over TLS to {self.host}:{self.port} with headers {self.headers}'
    
    def __init__(self,
                 loop:AbstractEventLoop,
                 connect_deadline:Deadline,
                 host,port,headers:Headers={}):
        self.host = host
        self.port = port
        self.headers:Headers = headers
        self.transportDetails:Optional[TLSTransportDetails] = None
        self.deadline:Deadline = deadline
        self.headers = headers
        self._protocol:Optional[_Protocol] = None
        Connection.__init__(self,loop)
        pass

    def __str__(self):
        return l1(TLSWebSocket.__doc__).format(**vars())

    async def __aenter__(self) -> Awaitable[Connection]:
        try:
            self._protocol = _ClientProtocol(loop, str(self))
            connected = loop.create_future()
            def on_transport_connected(transportDetails:_TransportDetails):
                protocol.clear_handlers()
                self.transportDetails = TLSTransportDetails(transportDetails)
                connected.set_result(self)
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
                    self.setHeaders(self.headers)
                    pass
                def buildProtocol(self):
                    protocol.factory = self
                    return protocol
                pass

            self.loop.connectSSL(self.host,self.port,F(self.deadline()))
            return connected
        except Exception as e:
            raise inContext(
                f'connect websocket over TLS to {self.host}:{self.port} with headers {self.headers}') from e
        pass

    def __aexit__(self,t,e,b) -> None:
        if not self._protocol.closed:
            self._protocol.abort()
            pass
        pass
    
    pass

class Connection():
    class Closed(Exception):
        def __init__(self,s):
            Exception.__init__(self,s)
            pass
        pass

    def __init__(self,loop:AbstractEventLoop):
        self.loop:AbstractEventLoop = loop
        pass

    def __exit__(self, t, e, b):
        if not self._protocol.closed:
            self._protocol.abort()
            pass
        pass

    async def receive_message(self, deadline:Deadline) -> Message:
        '''receive message within {deadline()}s'''
        '''Timeout - deadline reached before message received'''
        '''Connection.Closed - connection closed before message reached'''
        result = self.loop.create_future()
        def deadline_reached(self):
            self._protocol.clear_handlers()
            result.set_exception(
                Timeout(f'deadline reached before message received'))
            pass
        timer = self.loop.callLater(deadline(),deadline_reached)
        def on_message(message:Message):
            self._protocol.clear_handlers()
            timer.cancel()
            result.set_result(message)
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
        return future

    def send_message(self, message:Message) -> None:
        '''send Message {message} to {self}'s peer'''
        try:
            self._protocol.sendMsg(message)
        except Exception as e:
            raise inContext(
                l1(Connection.send_message.doc.format(**vars()))) from e
        pass

    async def close(self,
                    deadline:Deadline,
                    code:Optional[int],
                    reason:Optional[str]) -> Sequence[Message]:
        '''close websocket to peer {self.peer} sending close code {code}, reason {reason} by {deadline()}s, returning any intervening received messages'''
        try:
            result = self.loop.create_future()
            intervening_messages = []
            if not self._protocol.closed:
                self._protocol.setOptions(
                    closingHandshakeTimeout=deadline())
                def on_message(message:Message):
                    intervening_messages.append(message)                    
                    pass
                def on_close(was_clean:bool, code:Optional[int], reason:Optional[str]):
                    self._protocol.clear_handlers()
                    result.set_result(intervening_messages)
                    pass
                self._protocol.replace_handlers(
                    self._protocol._unexpected_transport_connected,
                    on_message,
                    on_close)
                self._protocol.sendClose()
                pass
            else:
                result.set_result(intervening_messages)
                pass
            return result
        except Exception as e:
            raise inContext(l1(Connection.close.__doc__.format(**vars()))) from e
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

    # autobahn.websocket.interfaces.IWebSocketChannel
    def onConnecting(self, transport_details:TransportDetails):
        self._on_transport_connected(transport_details)
        pass
    #REVISIT: onConnect useful?

    # autobahn.websocket.interfaces.IWebSocketChannel
    def onMessage(self, payload, isBinary)->None:
        if isBinary:
            assert isinstance(payload:bytes), payload.__class__
        self._on_message(payload)
        pass

    # autobahn.websocket.interfaces.IWebSocketChannel
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

class _ServerProtocol(WebSocketServerProtocol,_Protocol):
    def __init__(self, loop:AbstractEventLoop, peer:str):
        _Protocol.__init__(self, loop, peer)
        WebSocketServerProtocol.__init__(self)
        pass
    pass



class TLSWebSocketServer():
    'TLS websocket server on local address {self.localddress}:{self.port}'
    def __init__(self,loop:AbstractEventLoop,
                 localaddress,
                 localport):
        self.loop:AbstractEventLoop = loop
        self.host = localaddress
        self.port = port
        self.server:Optional[REVISIT] = None
        self.handshaking = list[_ServerProtcol]
        self.backlog:list[TLSConnection] = []
        self.next_connection:Awaitable[TLSConnection] = loop.create_future()
        pass

    def __str__(self):
        return l1(TLSWebSocketServer.__doc__).format(**vars())

    def __enter__(self) -> TLSWebSocketServer:
        try:
            class F(WebSocketServerFactory):
                def __init__(self,openingHandshakeTimeout:float,
                             server:TLSWebSocketServer):
                    super().__init__(self)
                    self.setOpeningHandshakeTimeout(openingHandshakeTimeout)
                    self.server = server
                    pass
                def buildProtocol(self):
                    protocol = _ServerProtcool(self.loop, REVISIT)
                    self.server.handshaking.append(protocol)
                    def on_connected():
                        self.server.handshaking.remove(protocol)
                        self.server.
                    REVISIT
                    protocol.factory = self
                    return protocol
                pass

            self.server = self.loop.create_server(factory, REVISIT)
            return self
        except Exception as e:
            raise inContext(
                f'connect websocket over TLS to {self.host}:{self.port} with headers {self.headers}') from e
        pass

    def __exit__(self, t, e, b):
        REVISIT: abort all handshaking
        self.server.close()
        pass

    async def accept_connection(self, deadline:Deadline) -> Awaitable[TLSConnection]:
        result = loop.create_future()
        REVISIT
        return result
        
    async def _connect_via(
            self,
            loop:AbstractEventLoop,
            deadline:Deadline) -> Tuple[_Protocol,AbstractTransportDetails]:
        '''connect {self} via {loop} by {deadline()}s'''
        try:
            protocol = _ClientProtocol(loop, str(self))
            connected = loop.create_future()
            def on_transport_connected(transportDetails:_TransportDetails):
                protocol.clear_handlers()
                connected.set_result(TLSTransportDetails(transportDetails))
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

            loop.connectSSL(self.host,self.port,F(deadline()))
            transportDetails = await connected
            return (protocol, transportDetails)
        except Exception as e:
            raise inContext(l1(TLSConnector.connectVia.__doc__).format(**vars())) from e
        pass
    pass

