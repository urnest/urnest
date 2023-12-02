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
    message: Union[
        "SnmpV1GetRequest", "SnmpV1GetNextRequest", "SnmpV1SetRequest", "SnmpV1Response",
        "SnmpV2cGetRequest", "SnmpV2cGetNextRequest", "SnmpV2cGetBulkRequest",
        "SnmpV2cSetRequest", "SnmpV2cResponse"]
    
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
    value_type:Literal["Integer"]
    value: int
    pass

@dataclass
class OctetStringValue:
    """snmp octet string"""
    value_type:Literal["OctetString"]
    value: bytes
    pass

@dataclass
class TimeTicksValue:
    """snmp time ticks"""
    value_type:Literal["TimeTicks"]
    value: int
    pass

@dataclass
class Gauge32Value:
    """snmp guage"""
    value_type:Literal["Gauge32"]
    value: int
    pass

@dataclass
class IpAddressValue:
    """snmp ipv4 address in dotted form"""
    value_type:Literal["IpAddress"]
    value: str
    pass

@dataclass
class OpaqueValue:
    """snmp opaque"""
    value_type:Literal["Opaque"]
    value: bytes
    pass

@dataclass
class NullValue:
    """snmp null"""
    value_type:Literal["Null"]
    pass

@dataclass
class Counter64Value:
    """snmp unsigned 64 bit integer"""
    value_type:Literal["Counter64"]
    value: int
    pass

@dataclass
class Counter32Value:
    """snmp unsigned 32 bit counter"""
    value_type:Literal["Counter32"]
    value: int
    pass

SnmpV1Value = IntValue | OctetStringValue | TimeTicksValue | Gauge32Value | Counter32Value | IpAddressValue | OpaqueValue | NullValue

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
class SnmpV1SetRequest:
    message_type: Literal["SnmpV1SetRequest"]
    community: Community
    request_id: RequestId
    vars: list[tuple[Oid, SnmpV1Value]]
    pass

@dataclass
class SnmpV2cGetRequest:
    message_type: Literal["SnmpV2cGetRequest"]
    community: Community
    request_id: RequestId
    oids: list[Oid]
    pass

@dataclass
class SnmpV2cGetNextRequest:
    message_type: Literal["SnmpV2cGetNextRequest"]
    community: Community
    request_id: RequestId
    oids: list[Oid]
    pass

@dataclass
class SnmpV2cGetBulkRequest:
    message_type: Literal["SnmpV2cGetBulkRequest"]
    community: Community
    request_id: RequestId
    get_next: list[Oid]
    get_next_n: list[Oid]
    n: int
    pass

@dataclass
class SnmpV2cSetRequest:
    message_type: Literal["SnmpV2cSetRequest"]
    community: Community
    request_id: RequestId
    vars: list[tuple[Oid, "SnmpV2cValue"]]
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
    community: Community
    request_id: RequestId
    error: None | TooBig | NoSuchName | BadValue | ReadOnly | GenErr
    values: list[tuple[Oid, SnmpV1Value]]


SnmpV2cValue = SnmpV1Value | Counter32Value | Counter64Value

SnmpV2cVarResult = (
    Literal["NoSuchObject"] | Literal["NoSuchInstance"] | Literal["EndOfMibView"] | SnmpV2cValue )

@dataclass
class NoAccess:
    error_type: Literal["NoAccess"]
    oid: Oid

@dataclass
class WrongType:
    error_type: Literal["WrongType"]
    oid: Oid

@dataclass
class WrongLength:
    error_type: Literal["WrongLength"]
    oid: Oid

@dataclass
class WrongEncoding:
    error_type: Literal["WrongEncoding"]
    oid: Oid

@dataclass
class WrongValue:
    error_type: Literal["WrongValue"]
    oid: Oid

@dataclass
class NoCreation:
    error_type: Literal["NoCreation"]
    oid: Oid

@dataclass
class InconsistentValue:
    error_type: Literal["InconsistentValue"]
    oid: Oid

@dataclass
class ResourceUnavailable:
    error_type: Literal["ResourceUnavailable"]
    oid: Oid

@dataclass
class CommitFailed:
    error_type: Literal["CommitFailed"]
    oid: Oid

@dataclass
class UndoFailed:
    error_type: Literal["UndoFailed"]
    oid: Oid

@dataclass
class NotWritable:
    error_type: Literal["NotWritable"]
    oid: Oid

@dataclass
class InconsistentName:
    error_type: Literal["InconsistentName"]
    oid: Oid


@dataclass
class SnmpV2cResponse:
    message_type: Literal["SnmpV2cResponse"]
    community: Community
    request_id: RequestId
    error: (None | TooBig | NoSuchName | BadValue | ReadOnly | GenErr |
            NoAccess | NotWritable | WrongType |
            WrongLength | WrongEncoding | WrongValue | NoCreation |
            InconsistentName | InconsistentValue | ResourceUnavailable | CommitFailed | UndoFailed )
    values: list[tuple[Oid, SnmpV2cVarResult]]


class MessageId(Int["MessageIdTag"]):
    """RFC 3412 msgID (an unsigned 31-bit integer)"""
    pass


@dataclass
class SnmpV3GetRequest:
    message_type: Literal["SnmpV3GetRequest"]
    message_id: MessageId
    max_size: int
    engine_id: bytes
    context_name: bytes
    request_id: RequestId
    oids: list[Oid]
    pass

@dataclass
class SnmpV3GetNextRequest:
    message_type: Literal["SnmpV3GetNextRequest"]
    message_id: MessageId
    max_size: int
    engine_id: bytes
    context_name: bytes
    request_id: RequestId
    oids: list[Oid]
    pass

@dataclass
class SnmpV3GetBulkRequest:
    message_type: Literal["SnmpV3GetBulkRequest"]
    message_id: MessageId
    max_size: int
    engine_id: bytes
    context_name: bytes
    request_id: RequestId
    get_next: list[Oid]
    get_next_n: list[Oid]
    n: int
    pass

@dataclass
class SnmpV3SetRequest:
    message_type: Literal["SnmpV3SetRequest"]
    message_id: MessageId
    max_size: int
    engine_id: bytes
    context_name: bytes
    request_id: RequestId
    vars: list[tuple[Oid, "SnmpV2cValue"]]
    pass


@dataclass
class SnmpV3Response:
    message_type: Literal["SnmpV3Response"]
    message_id: MessageId
    max_size: int
    engine_id: bytes
    context_name: bytes
    request_id: RequestId
    error: (None | TooBig | NoSuchName | BadValue | ReadOnly | GenErr |
            NoAccess | NotWritable | WrongType |
            WrongLength | WrongEncoding | WrongValue | NoCreation |
            InconsistentName | InconsistentValue | ResourceUnavailable | CommitFailed | UndoFailed )
    values: list[tuple[Oid, SnmpV2cVarResult]]


# tags for newtypes defined above
class CommunityTag:pass
class RequestIdTag:pass
class OidTag:pass
class ErrorIndexTag: pass
class MessageIdTag:pass
