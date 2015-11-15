// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/encodeResponse.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/snmp/RequestId.hh>
#include <xju/snmp/SnmpV1GetRequest.hh>
#include <memory>
#include "xju/snmp/Value.hh"
#include "xju/snmp/OidValue.hh"
#include "xju/snmp/NullValue.hh"
#include "xju/snmp/validateResponse.hh"
#include "xju/snmp/decodeSnmpV1Response.hh"

namespace xju
{
namespace snmp
{

void test1() {
  std::vector<uint8_t> const x(
    encodeResponse(
      SnmpV1GetRequest(
        Community("private"),
        xju::snmp::RequestId(1),
        {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),Oid(".1.3")}),
      {Oid(".1.3"),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0")},
      {{Oid(".1.3"),std::shared_ptr<xju::snmp::Value const>(
            new xju::snmp::OidValue(Oid(".1.3.7")))},
        {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
            std::shared_ptr<Value const>(new NullValue)}}));
        
  xju::assert_equal(
    x,
    std::vector<uint8_t>({
        0x30,53,0x02,0x01,0x00,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA2,39,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,28,0x30,0x07,0x06,0x01,0x2B,0x06,0x02,0x2B,0x07,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
          }));
}

void test2() {
  SnmpV1GetRequest const request(
    Community("private"),
    xju::snmp::RequestId(1),
    {Oid(".1.3"),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0")});
  
  std::vector<uint8_t> const x(
    encodeResponse(
      request,
      std::vector<Oid>(request.oids_.begin(),request.oids_.end()),
      NoSuchName(Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),XJU_TRACED)));
  try {
    validateResponse(request,decodeSnmpV1Response(x));
    xju::assert_never_reached();
  }
  catch(NoSuchName const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
}

}
}

using namespace xju::snmp;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

