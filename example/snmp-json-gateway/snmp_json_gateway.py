"""
- message types captured using xju.json_codec - compatible classes, e.g. SnmpV1GetRequest
"""
from dataclasses import dataclass
from typing import Literal
from xju.newtype import Int, Str
from typing import Union

@dataclass
class GatewayMessage:
    # dotted ip v4 address, udp port
    source: tuple[str, int]
    # dotted ip v4 address, udp port
    dest: tuple[str, int]
    message: Union["SnmpV1GetRequest", "SnmpV1GetNextRequest", "SnmpV1Response"]
    
class Community(Str["CommunityTag"]):
    """snmp community"""
    pass

class RequestId(Int["RequestIdTag"]):
    """RFC 1157 request-id (an unsigned 64-bit integer)"""
    pass

class Oid(Str["OidTag"]):
    """oid in dotted notation"""
    pass

@dataclass
class IntValue:
    """snmp integer"""
    value_type:Literal["IntValue"]
    value: int
    pass

@dataclass
class StrValue:
    """snmp octet string"""
    value_type:Literal["StrValue"]
    value: str
    pass

@dataclass
class TimeTicksValue:
    """snmp time ticks"""
    value_type:Literal["TimeTicksValue"]
    value: int
    pass

@dataclass
class GaugeValue:
    """snmp guage"""
    value_type:Literal["GaugeValue"]
    value: int
    pass

@dataclass
class CounterValue:
    """snmp counter unsigned 32 bit integer"""
    value_type:Literal["CounterValue"]
    value: int
    pass

@dataclass
class Ip4AddressValue:
    """snmp ipv4 address in dotted form"""
    value_type:Literal["Ip4AddressValue"]
    value: str
    pass

@dataclass
class OpaqueValue:
    """snmp opaque"""
    value_type:Literal["OpaqueValue"]
    value: list[int]  # list of bytes
    pass

@dataclass
class NullValue:
    """snmp null"""
    value_type:Literal["NullValue"]
    pass

@dataclass
class U64Value:
    """snmp unsigned 64 bit integer"""
    value_type:Literal["U64Value"]
    value: int
    pass


SnmpV1Value = IntValue | StrValue | TimeTicksValue | GaugeValue | CounterValue | Ip4AddressValue | OpaqueValue | NullValue

@dataclass
class SnmpV1GetRequest:
    message_type: Literal["SnmpV1GetRequest"]
    community: Community
    request_id: RequestId
    oids: list[Oid]
    pass

@dataclass
class SnmpV1GetNextRequest:
    message_type: Literal["SnmpV1GetNextRequest"]
    community: Community
    request_id: RequestId
    oids: list[Oid]
    pass

@dataclass
class TooBig:
    error_type: Literal["TooBig"]
    pass

@dataclass
class NoSuchName:
    error_type: Literal["NoSuchName"]
    oid: Oid

@dataclass
class BadValue:
    error_type: Literal["BadValue"]
    oid: Oid

@dataclass
class ReadOnly:
    error_type: Literal["ReadOnly"]
    oid: Oid

@dataclass
class GenErr:
    error_type: Literal["GenErr"]
    oid: Oid

@dataclass
class SnmpV1Response:
    message_type: Literal["SnmpV1Response"]
    response_type: int 
    community: Community
    request_id: RequestId
    error: None | TooBig | NoSuchName | BadValue | ReadOnly | GenErr
    values: list[tuple[Oid, SnmpV1Value]]


SnmpV2Value = SnmpV1Value | U64Value


# tags for newtypes defined above
class CommunityTag:pass
class RequestIdTag:pass
class OidTag:pass
class ErrorIndexTag: pass
