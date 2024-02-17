#!/usr/bin/env python3

import sys
from xju.json_codec import codec, JsonType
from xju.xn import in_context
from xju.cmc import Process
from json import loads, dumps
from snmp_json_gateway import (
    Community,
    ContextName,
    GatewayMessage,
    IntValue,
    ListeningOn,
    Oid,
    SnmpV2cGetRequest,
    SnmpV2cResponse,
    UserName,
    UsmSecurityParameters,
    SnmpV3GetRequest,
    SnmpV3Response,
)
from xju.assert_ import Assert

snmp_json_gateway_exe=sys.argv[1]
net_snmp_get_exe=sys.argv[2]

message_codec=codec(GatewayMessage)

def to_json(x: JsonType) -> bytes:
    return dumps(x).encode('utf-8')

engineId='AA23F2'
with Process([snmp_json_gateway_exe, '--v3usm', engineId, 'auto'], stdin="PIPE", stdout="PIPE") as g:
    assert g.stdout is not None
    assert g.stdin is not None
    port=codec(ListeningOn).decode(loads(g.stdout.readline().decode("utf-8"))).listening_on
    c=[
        net_snmp_get_exe, '-r', '0', '-v2c', '-c', 'fred', f"localhost:{port}", '.1.3.6.1.4.1.2680.1.2.7.3.2'
    ]
    print(c)
    with Process(c, stdout="PIPE") as p:
        assert p.stdout is not None
        m=message_codec.decode(loads(g.stdout.readline()))
        assert isinstance(m.message, SnmpV2cGetRequest), m
        Assert(m.message.community)==Community("fred")
        Assert(m.message.oids)==[Oid('.1.3.6.1.4.1.2680.1.2.7.3.2')]
        g.stdin.write(to_json(message_codec.encode(
            GatewayMessage(
                m.remote_endpoint,
                SnmpV2cResponse(
                    "SnmpV2cResponse",
                    m.message.community,
                    m.message.request_id,
                    None,
                    [(m.message.oids[0], IntValue("Integer",33))]))))+b'\n')
        g.stdin.flush()
        stdout, _ = p.communicate()
        Assert(stdout.decode('utf-8'))=='iso.3.6.1.4.1.2680.1.2.7.3.2 = INTEGER: 33\n'
        pass
    c=[
        net_snmp_get_exe, '-r', '0', '-v3', '-l','noAuthNoPriv','-u', 'fred', '-n', 'conny', f"localhost:{port}", '.1.3.6.1.4.1.2680.1.2.7.3.2'
    ]
    print(c)
    with Process(c, stdout="PIPE") as p:
        assert p.stdout is not None
        m=message_codec.decode(loads(g.stdout.readline()))
        assert isinstance(m.message, SnmpV3GetRequest), m
        Assert(m.message)==SnmpV3GetRequest(
            'SnmpV3GetRequest',
            m.message.message_id,
            m.message.max_size,
            bytes([0xAA,0x23,0xF2]),
            ContextName("conny"),
            UsmSecurityParameters(UserName('fred')),
            m.message.request_id,
            [Oid('.1.3.6.1.4.1.2680.1.2.7.3.2')])
        g.stdin.write(to_json(message_codec.encode(
            GatewayMessage(
                m.remote_endpoint,
                SnmpV3Response(
                    "SnmpV3Response",
                    m.message.message_id,
                    32768,
                    m.message.engine_id,
                    m.message.context_name,
                    m.message.security_parameters,
                    m.message.request_id,
                    None,
                    [(m.message.oids[0], IntValue("Integer",34))]))))+b'\n')
        g.stdin.flush()
        stdout, _ = p.communicate()
        Assert(stdout.decode('utf-8'))=='iso.3.6.1.4.1.2680.1.2.7.3.2 = INTEGER: 34\n'
        pass
    pass


for authAlg, netsnmpAuthAlg in [
        ('sha96', 'SHA'),
        ('sha256', 'SHA-256'),
        ('sha384', 'SHA-384'),
        ('sha512', 'SHA-512'),
]:
    try:
        engineId='AA23F2'
        with Process([snmp_json_gateway_exe, '--v3usm', engineId, '--auth', f'{authAlg}:pollywally', 'auto'], stdin="PIPE", stdout="PIPE") as g:
            assert g.stdout is not None
            assert g.stdin is not None
            try:
                l=g.stdout.readline().decode("utf-8")
                try:
                    port=codec(ListeningOn).decode(loads(l)).listening_on
                except Exception as e:
                    raise in_context(f'parse line {l!r}')
            except Exception as e:
                raise in_context('read and parse "listening_on" line')
            c=[
                net_snmp_get_exe, '-r', '0', '-v3', '-l','authNoPriv','-a',netsnmpAuthAlg,'-A','pollywally','-u', 'fred', '-n', 'conny', f"localhost:{port}", '.1.3.6.1.4.1.2680.1.2.7.3.3'
            ]
            print(c)
            with Process(c, stdout="PIPE") as p:
                assert p.stdout is not None
                m=message_codec.decode(loads(g.stdout.readline()))
                assert isinstance(m.message, SnmpV3GetRequest), m
                Assert(m.message)==SnmpV3GetRequest(
                    'SnmpV3GetRequest',
                    m.message.message_id,
                    m.message.max_size,
                    bytes([0xAA,0x23,0xF2]),
                    ContextName("conny"),
                    UsmSecurityParameters(UserName('fred')),
                    m.message.request_id,
                    [Oid('.1.3.6.1.4.1.2680.1.2.7.3.3')])
                g.stdin.write(to_json(message_codec.encode(
                    GatewayMessage(
                        m.remote_endpoint,
                        SnmpV3Response(
                            "SnmpV3Response",
                            m.message.message_id,
                            m.message.max_size,
                            m.message.engine_id,
                            m.message.context_name,
                            m.message.security_parameters,
                            m.message.request_id,
                            None,
                            [(m.message.oids[0], IntValue("Integer",84))]))))+b'\n')
                g.stdin.flush()
                stdout, _ = p.communicate()
                Assert(stdout.decode('utf-8'))=='iso.3.6.1.4.1.2680.1.2.7.3.3 = INTEGER: 84\n'
                pass
            pass
    except Exception as e:
        raise in_context(f"test auth alg {authAlg:} v netsnmp -a {netsnmpAuthAlg:}")


for privAlg, netsnmpPrivAlg in [
        ('aes128cfb', 'AES'),
]:
    try:
        engineId='AA23F2'
        args=[snmp_json_gateway_exe, '--v3usm', engineId, '--auth', f'sha256:pollywally', '--priv', f'{privAlg}:wallypolly', 'auto']
        print(' '.join(args))
        with Process(args, stdin="PIPE", stdout="PIPE") as g:
            assert g.stdout is not None
            assert g.stdin is not None
            try:
                l=g.stdout.readline().decode("utf-8")
                try:
                    port=codec(ListeningOn).decode(loads(l)).listening_on
                except Exception as e:
                    raise in_context(f'parse line {l!r}')
            except Exception as e:
                raise in_context('read and parse "listening_on" line')
            c=[
                net_snmp_get_exe, '-r', '0', '-v3', '-l','authPriv','-a','SHA-256','-A','pollywally','-x',netsnmpPrivAlg,'-X','wallypolly','-u', 'fred', '-n', 'conny', f"localhost:{port}", '.1.3.6.1.4.1.2680.1.2.7.3.3'
            ]
            print(' '.join(c))
            with Process(c, stdout="PIPE") as p:
                assert p.stdout is not None
                m=message_codec.decode(loads(g.stdout.readline()))
                assert isinstance(m.message, SnmpV3GetRequest), m
                Assert(m.message)==SnmpV3GetRequest(
                    'SnmpV3GetRequest',
                    m.message.message_id,
                    m.message.max_size,
                    bytes([0xAA,0x23,0xF2]),
                    ContextName("conny"),
                    UsmSecurityParameters(UserName('fred')),
                    m.message.request_id,
                    [Oid('.1.3.6.1.4.1.2680.1.2.7.3.3')])
                g.stdin.write(to_json(message_codec.encode(
                    GatewayMessage(
                        m.remote_endpoint,
                        SnmpV3Response(
                            "SnmpV3Response",
                            m.message.message_id,
                            m.message.max_size,
                            m.message.engine_id,
                            m.message.context_name,
                            m.message.security_parameters,
                            m.message.request_id,
                            None,
                            [(m.message.oids[0], IntValue("Integer",84))]))))+b'\n')
                g.stdin.flush()
                stdout, _ = p.communicate()
                Assert(stdout.decode('utf-8'))=='iso.3.6.1.4.1.2680.1.2.7.3.3 = INTEGER: 84\n'
                pass
            pass
    except Exception as e:
        raise in_context(f"test auth alg {authAlg:} v netsnmp -a {netsnmpAuthAlg:}")
