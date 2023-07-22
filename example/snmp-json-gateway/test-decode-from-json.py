#!/usr/bin/env python3
import sys
import json
from xju.assert_ import Assert
from xju.json_codec import Codec
from snmp_json_gateway import SnmpV1GetRequest, SnmpV1GetNextRequest, RequestId, Community, Oid

codec=Codec(list[SnmpV1GetRequest|SnmpV1GetNextRequest])

assert codec.decode(json.loads(open(sys.argv[1]).read())) == [
    SnmpV1GetRequest(
        "SnmpV1GetRequest",
        Community("fred"),
        RequestId(27),
        [Oid(".1.4.6.1.27.3"), Oid(".1.4.6.1.19")]),
    SnmpV1GetNextRequest(
        "SnmpV1GetNextRequest",
        Community("ann"),
        RequestId(2),
        [Oid(".1.4.6.12.27.3"), Oid(".1.4.6.12.19")])]
