#!/usr/bin/env python3
import sys
import json
from xju.assert_ import Assert
from xju.json_codec import Codec
from snmp_json_gateway import SnmpV1Response, RequestId, Community, Oid
from snmp_json_gateway import NullValue, IntValue, StrValue, TimeTicksValue
from snmp_json_gateway import GaugeValue, CounterValue, Ip4AddressValue, OpaqueValue
from snmp_json_gateway import TooBig, NoSuchName, BadValue, ReadOnly, GenErr

codec=Codec(list[SnmpV1Response])

print(json.dumps(codec.encode([
    SnmpV1Response(
        "SnmpV1Response",
        3,
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
        3,
        Community("fred"),
        RequestId(2),
        TooBig("TooBig"),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV1Response(
        "SnmpV1Response",
        3,
        Community("fred"),
        RequestId(3),
        NoSuchName("NoSuchName", Oid(".1.4.6.1.27.3")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV1Response(
        "SnmpV1Response",
        3,
        Community("fred"),
        RequestId(4),
        BadValue("BadValue", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV1Response(
        "SnmpV1Response",
        3,
        Community("fred"),
        RequestId(5),
        ReadOnly("ReadOnly", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ]),
    SnmpV1Response(
        "SnmpV1Response",
        3,
        Community("fred"),
        RequestId(6),
        GenErr("GenErr", Oid(".1.4.6.1.27.4")),
        [
            (Oid(".1.4.6.1.27.3"), NullValue("NullValue")),
            (Oid(".1.4.6.1.27.4"), NullValue("NullValue")),
        ])])))
