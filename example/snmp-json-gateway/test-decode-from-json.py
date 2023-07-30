#!/usr/bin/env python3
import sys
import json
from xju.assert_ import Assert
from xju.json_codec import Codec
from snmp_json_gateway import SnmpV1GetRequest, SnmpV1GetNextRequest, RequestId, Community, Oid
from snmp_json_gateway import SnmpV1SetRequest, IntValue, StrValue

from snmp_json_gateway import SnmpV2cGetRequest, SnmpV2cGetNextRequest, SnmpV2cGetBulkRequest
from snmp_json_gateway import SnmpV2cSetRequest, IntValue, StrValue, RequestId, Community, Oid

codec=Codec(list[SnmpV1GetRequest|SnmpV1GetNextRequest|SnmpV1SetRequest|
                 SnmpV2cGetRequest|SnmpV2cGetNextRequest|SnmpV2cGetBulkRequest|SnmpV2cSetRequest])

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
          (Oid(".1.4.6.12.19"), StrValue("StrValue","fred"))])
]
