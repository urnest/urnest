#!/usr/bin/env python3
import sys
import json
from xju.assert_ import Assert
from xju.json_codec import Codec
from snmp_json_gateway import SnmpV1GetRequest, SnmpV1GetNextRequest, RequestId, Community, Oid
from snmp_json_gateway import SnmpV1SetRequest, IntValue, StrValue, Counter32Value, Counter64Value

from snmp_json_gateway import SnmpV2cGetRequest, SnmpV2cGetNextRequest, SnmpV2cGetBulkRequest
from snmp_json_gateway import SnmpV2cSetRequest, IntValue, StrValue, RequestId, Community, Oid
from snmp_json_gateway import MessageId, SnmpV3GetRequest, SnmpV3GetNextRequest
from snmp_json_gateway import SnmpV3GetBulkRequest, SnmpV3SetRequest

codec=Codec(list[SnmpV1GetRequest|SnmpV1GetNextRequest|SnmpV1SetRequest|
                 SnmpV2cGetRequest|SnmpV2cGetNextRequest|SnmpV2cGetBulkRequest|SnmpV2cSetRequest|
                 SnmpV3GetRequest|SnmpV3GetNextRequest|SnmpV3GetBulkRequest|SnmpV3SetRequest])

Assert(codec.decode(json.loads(open(sys.argv[1]).read()))) == [
    SnmpV1GetRequest(
        "SnmpV1GetRequest",
        Community("fred"),
        RequestId(27),
        [Oid(".1.4.6.1.27.3"), Oid(".1.4.6.1.19")]),
    SnmpV1GetNextRequest(
        "SnmpV1GetNextRequest",
        Community("ann"),
        RequestId(2),
        [Oid(".1.4.6.12.27.3"), Oid(".1.4.6.12.19")]),
    SnmpV1SetRequest(
        "SnmpV1SetRequest",
        Community("ann"),
        RequestId(2),
        [ (Oid(".1.4.6.12.27.3"), IntValue("IntValue",3)),
          (Oid(".1.4.6.12.19"), StrValue("StrValue","fred"))]),

    SnmpV2cGetRequest(
        "SnmpV2cGetRequest",
        Community("fred"),
        RequestId(27),
        [Oid(".1.4.6.1.27.3"), Oid(".1.4.6.1.19")]),
    SnmpV2cGetNextRequest(
        "SnmpV2cGetNextRequest",
        Community("ann"),
        RequestId(2),
        [Oid(".1.4.6.12.27.3"), Oid(".1.4.6.12.19")]),
    SnmpV2cGetBulkRequest(
        "SnmpV2cGetBulkRequest",
        Community("ann"),
        RequestId(2),
        [Oid(".1.4.6.12.27.3"), Oid(".1.4.6.12.19")],
        [Oid(".1.4.6.12.27.5")],
        10),
    SnmpV2cSetRequest(
        "SnmpV2cSetRequest",
        Community("ann"),
        RequestId(2),
        [ (Oid(".1.4.6.12.27.3"), IntValue("IntValue",3)),
          (Oid(".1.4.6.12.19"), StrValue("StrValue","fred"))]),
    
    SnmpV3GetRequest(
        "SnmpV3GetRequest",
        MessageId(23),
        66786,
        b"cathy",
        b"colin",
        RequestId(27),
        [Oid(".1.4.6.1.27.3"), Oid(".1.4.6.1.19")]),
    SnmpV3GetNextRequest(
        "SnmpV3GetNextRequest",
        MessageId(24),
        66786,
        b"cathy",
        b"colin",
        RequestId(28),
        [Oid(".1.4.6.12.27.3"), Oid(".1.4.6.12.19")]),
    SnmpV3GetBulkRequest(
        "SnmpV3GetBulkRequest",
        MessageId(25),
        66786,
        b"cathy",
        b"colin",
        RequestId(29),
        [Oid(".1.4.6.12.27.3"), Oid(".1.4.6.12.19")],
        [Oid(".1.4.6.12.27.5")],
        10),
    SnmpV3SetRequest(
        "SnmpV3SetRequest",
        MessageId(26),
        66786,
        b"cathy",
        b"colin",
        RequestId(30),
        [ (Oid(".1.4.6.12.27.3"), Counter32Value("Counter32",32)),
          (Oid(".1.4.6.12.19"), Counter64Value("Counter64",64))]),
]
