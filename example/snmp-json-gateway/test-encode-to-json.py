#!/usr/bin/env python3
import sys
import json
from xju.assert_ import Assert
from xju.json_codec import Codec
from snmp_json_gateway import SnmpV1Response, RequestId, Community, Oid
from snmp_json_gateway import NullValue, IntValue, StrValue, TimeTicksValue, Counter64Value
from snmp_json_gateway import GaugeValue, CounterValue, Ip4AddressValue, OpaqueValue
from snmp_json_gateway import TooBig, NoSuchName, BadValue, ReadOnly, GenErr
from snmp_json_gateway import SnmpV2cResponse, NoAccess, WrongType, WrongLength, WrongEncoding
from snmp_json_gateway import WrongValue, NoCreation, InconsistentValue, ResourceUnavailable
from snmp_json_gateway import CommitFailed, UndoFailed, NotWritable, InconsistentName

codec=Codec(list[SnmpV1Response|SnmpV2cResponse])

print(json.dumps(codec.encode([
    SnmpV1Response(
        "SnmpV1Response",
        Community("fred"),
        RequestId(1),
        None,
        [
            (Oid(".1.4.6.1.27.3"), IntValue("IntValue", 7)),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.5"), StrValue("StrValue", "ann")),
            (Oid(".1.4.6.1.27.6"), TimeTicksValue("TimeTicksValue", 720)),
            (Oid(".1.4.6.1.27.7"), GaugeValue("GaugeValue", 9987)),
            (Oid(".1.4.6.1.27.8"), CounterValue("CounterValue", 19987)),
            (Oid(".1.4.6.1.27.9"), Ip4AddressValue("Ip4AddressValue","188.18.22.11")),
            (Oid(".1.4.6.1.27.10"), OpaqueValue("OpaqueValue",[18,22,253])),
        ]),
    SnmpV1Response(
        "SnmpV1Response",
        Community("fred"),
        RequestId(2),
        TooBig("TooBig"),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV1Response(
        "SnmpV1Response",
        Community("fred"),
        RequestId(3),
        NoSuchName("NoSuchName", Oid(".1.4.6.1.27.3")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV1Response(
        "SnmpV1Response",
        Community("fred"),
        RequestId(4),
        BadValue("BadValue", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV1Response(
        "SnmpV1Response",
        Community("fred"),
        RequestId(5),
        ReadOnly("ReadOnly", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV1Response(
        "SnmpV1Response",
        Community("fred"),
        RequestId(6),
        GenErr("GenErr", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(1),
        None,
        [
            (Oid(".1.4.6.1.27.3"), IntValue("IntValue", 7)),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.5"), StrValue("StrValue", "ann")),
            (Oid(".1.4.6.1.27.6"), TimeTicksValue("TimeTicksValue", 720)),
            (Oid(".1.4.6.1.27.7"), GaugeValue("GaugeValue", 9987)),
            (Oid(".1.4.6.1.27.8"), CounterValue("CounterValue", 19987)),
            (Oid(".1.4.6.1.27.9"), Ip4AddressValue("Ip4AddressValue","188.18.22.11")),
            (Oid(".1.4.6.1.27.10"), OpaqueValue("OpaqueValue",[18,22,253])),
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
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(3),
        NoSuchName("NoSuchName", Oid(".1.4.6.1.27.3")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(4),
        BadValue("BadValue", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(5),
        ReadOnly("ReadOnly", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        GenErr("GenErr", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        NoAccess("NoAccess", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        WrongType("WrongType", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        WrongLength("WrongLength", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        WrongEncoding("WrongEncoding", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        WrongValue("WrongValue", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        NoCreation("NoCreation", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        InconsistentValue("InconsistentValue", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        ResourceUnavailable("ResourceUnavailable", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        CommitFailed("CommitFailed", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        UndoFailed("UndoFailed", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        NotWritable("NotWritable", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV2cResponse(
        "SnmpV2cResponse",
        Community("fred"),
        RequestId(6),
        InconsistentName("InconsistentName", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ])])))
