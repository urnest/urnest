// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/encodeSnmpV3UsmMessage.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/snmp/decodeSnmpV3Message.hh>
#include <xju/snmp/NullValue.hh>

namespace xju
{
namespace snmp
{

void test1() {
  xju::assert_equal(
    decodeSnmpV3Message(
      encodeSnmpV3UsmMessage<NoAuthMacCalculator,0U>(
        SnmpV3UsmMessage(
          SnmpV3Message::ID(33),
          512,
          (SnmpV3Message::Flags)(SnmpV3Message::REPORTABLE|SnmpV3Message::AUTH),
          SnmpV3UsmSecurityParameters(
            ContextEngineID(std::vector<uint8_t>{0x01,0x02,0x03}),
            EngineBoots(3),
            EngineTime(445),
            UserName("fred")),
          SnmpV3ScopedPDU(
            ContextEngineID({0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00 }),
            // contextName = fred
            ContextName({0x6a, 0x6f, 0x63, 0x6b}),
            PDU(
              RequestId(88),
              0,
              0,
                   {SnmpVar(Oid(Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0")),std::make_shared<NullValue>())},
              0xa0))),
        SnmpV3UsmAuthKey(std::vector<uint8_t>()))),
    std::make_pair(
      SnmpV3Message(
        SnmpV3Message::ID(33),
        512,
        (SnmpV3Message::Flags)(SnmpV3Message::REPORTABLE|SnmpV3Message::AUTH),
        SnmpV3Message::SecurityModel(3),
        SnmpV3SecParams(std::vector<uint8_t>{48,22,4,3,1,2,3,2,1,3,2,2,1,189,4,4,102,114,101,100,4,0,4,0}),
        SnmpV3ScopedPduData(std::vector<uint8_t>{
            48,57,4,17,128,0,31,136,128,230,121,8,1,151,53,46,93,0,0,0,0,4,4,106,111,99,107,160,30,2,1,88,2,1,0,2,1,0,48,19,48,17,6,13,43,6,1,4,1,148,120,1,2,7,3,2,0,5,0})),
      22UL));
                   
}

}
}

using namespace xju::snmp;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

