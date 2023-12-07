#!/usr/bin/env python3
import sys
import json
from xju.assert_ import Assert
from xju.json_codec import Codec
from snmp_json_gateway import SnmpV1Response, RequestId, Community, Oid
from snmp_json_gateway import NullValue, IntValue, TimeTicksValue, Counter64Value
from snmp_json_gateway import Gauge32Value, Counter32Value, IpAddressValue, OctetStringValue, OpaqueValue
from snmp_json_gateway import TooBig, NoSuchName, BadValue, ReadOnly, GenErr
from snmp_json_gateway import SnmpV2cResponse, NoAccess, WrongType, WrongLength, WrongEncoding
from snmp_json_gateway import WrongValue, NoCreation, InconsistentValue, ResourceUnavailable
from snmp_json_gateway import CommitFailed, UndoFailed, NotWritable, InconsistentName
from snmp_json_gateway import SnmpV3Response, MessageId, UsmSecurityParameters, UserName, ContextName

codec=Codec(list[SnmpV1Response|SnmpV2cResponse|SnmpV3Response])

print(json.dumps(codec.encode([
    SnmpV1Response(
        "SnmpV1Response",
        Community("fred"),
        RequestId(1),
        None,
        [
            (Oid(".1.4.6.1.27.3"), IntValue("Integer", 7)),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
            (Oid(".1.4.6.1.27.5"), OctetStringValue("OctetString", b"ann")),
            (Oid(".1.4.6.1.27.6"), TimeTicksValue("TimeTicks", 720)),
            (Oid(".1.4.6.1.27.7"), Gauge32Value("Gauge32", 9987)),
            (Oid(".1.4.6.1.27.8"), Counter32Value("Counter32", 19987)),
            (Oid(".1.4.6.1.27.9"), IpAddressValue("IpAddress","188.18.22.11")),
            (Oid(".1.4.6.1.27.10"), OpaqueValue("Opaque",bytes([18,22,253]))),
        ]),
    SnmpV1Response(
        "SnmpV1Response",
        Community("fred"),
        RequestId(2),
        TooBig("TooBig"),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV1Response(
        "SnmpV1Response",
        Community("fred"),
        RequestId(3),
        NoSuchName("NoSuchName", Oid(".1.4.6.1.27.3")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV1Response(
        "SnmpV1Response",
        Community("fred"),
        RequestId(4),
        BadValue("BadValue", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV1Response(
        "SnmpV1Response",
        Community("fred"),
        RequestId(5),
        ReadOnly("ReadOnly", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV1Response(
        "SnmpV1Response",
        Community("fred"),
        RequestId(6),
        GenErr("GenErr", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(1),
        None,
        [
            (Oid(".1.4.6.1.27.3"), IntValue("Integer", 7)),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
            (Oid(".1.4.6.1.27.5"), OctetStringValue("OctetString", b"ann")),
            (Oid(".1.4.6.1.27.6"), TimeTicksValue("TimeTicks", 720)),
            (Oid(".1.4.6.1.27.7"), Gauge32Value("Gauge32", 9987)),
            (Oid(".1.4.6.1.27.8"), Counter32Value("Counter32", 19987)),
            (Oid(".1.4.6.1.27.9"), IpAddressValue("IpAddress","188.18.22.11")),
            (Oid(".1.4.6.1.27.10"), OpaqueValue("Opaque",bytes([18,22,253]))),
            (Oid(".1.4.6.1.27.11"), Counter64Value("Counter64", 100)),
            (Oid(".1.4.6.1.27.12"), "NoSuchObject"),
            (Oid(".1.4.6.1.27.13"), "NoSuchInstance"),
            (Oid(".1.4.6.1.27.14"), "EndOfMibView"),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(2),
        TooBig("TooBig"),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(3),
        NoSuchName("NoSuchName", Oid(".1.4.6.1.27.3")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(4),
        BadValue("BadValue", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(5),
        ReadOnly("ReadOnly", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        GenErr("GenErr", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        NoAccess("NoAccess", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        WrongType("WrongType", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        WrongLength("WrongLength", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        WrongEncoding("WrongEncoding", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        WrongValue("WrongValue", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        NoCreation("NoCreation", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        InconsistentValue("InconsistentValue", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        ResourceUnavailable("ResourceUnavailable", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        CommitFailed("CommitFailed", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        UndoFailed("UndoFailed", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        NotWritable("NotWritable", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        InconsistentName("InconsistentName", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),

    SnmpV3Response(
        "SnmpV3Response",
        MessageId(100),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(1),
        None,
        [
            (Oid(".1.4.6.1.27.3"), IntValue("Integer", 7)),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
            (Oid(".1.4.6.1.27.5"), OctetStringValue("OctetString", b"ann")),
            (Oid(".1.4.6.1.27.6"), TimeTicksValue("TimeTicks", 720)),
            (Oid(".1.4.6.1.27.7"), Gauge32Value("Gauge32", 9987)),
            (Oid(".1.4.6.1.27.8"), Counter32Value("Counter32", 19987)),
            (Oid(".1.4.6.1.27.9"), IpAddressValue("IpAddress","188.18.22.11")),
            (Oid(".1.4.6.1.27.10"), OpaqueValue("Opaque",bytes([18,22,253]))),
            (Oid(".1.4.6.1.27.11"), Counter64Value("Counter64", 100)),
            (Oid(".1.4.6.1.27.12"), "NoSuchObject"),
            (Oid(".1.4.6.1.27.13"), "NoSuchInstance"),
            (Oid(".1.4.6.1.27.14"), "EndOfMibView"),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(200),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(2),
        TooBig("TooBig"),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(300),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(3),
        NoSuchName("NoSuchName", Oid(".1.4.6.1.27.3")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(400),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(4),
        BadValue("BadValue", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(500),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(5),
        ReadOnly("ReadOnly", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(600),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(6),
        GenErr("GenErr", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(600),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(6),
        NoAccess("NoAccess", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(600),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(6),
        WrongType("WrongType", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(600),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(6),
        WrongLength("WrongLength", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(600),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(6),
        WrongEncoding("WrongEncoding", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(600),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(6),
        WrongValue("WrongValue", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(600),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(6),
        NoCreation("NoCreation", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(600),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(6),
        InconsistentValue("InconsistentValue", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(600),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(6),
        ResourceUnavailable("ResourceUnavailable", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(600),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(6),
        CommitFailed("CommitFailed", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(600),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(6),
        UndoFailed("UndoFailed", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(600),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(6),
        NotWritable("NotWritable", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ]),
    SnmpV3Response(
        "SnmpV3Response",
        MessageId(600),
        6200,
        b'freng',
        ContextName('conny'),
        UsmSecurityParameters(UserName('fred')),
        RequestId(6),
        InconsistentName("InconsistentName", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("Null")),
            (Oid(".1.4.6.1.27.4"), NullValue("Null")),
        ])])))
