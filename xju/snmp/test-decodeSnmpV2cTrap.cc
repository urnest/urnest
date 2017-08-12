// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/snmp/decodeSnmpV2cTrap.hh"

#include <iostream>
#include <xju/assert.hh>
#include "xju/snmp/NullValue.hh"
#include "xju/snmp/StringValue.hh"
#include "xju/snmp/OidValue.hh"

namespace xju
{
namespace snmp
{

void test1() {
  // decodeSnmpV2cTrap
  std::vector<uint8_t> x({
      0x30, 0x4e, 0x02, 0x01, 0x01, 0x04, 0x04, 0x61, 0x61, 0x61, 0x61, 0xa7, 0x43, 0x02, 0x04, 0x6b, 0x2d, 0x7f, 0xbc, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x35, 0x30, 0x0d, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x02, 0x01, 0x01, 0x03, 0x00, 0x43, 0x01, 0x25, 0x30, 0x15, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x06, 0x03, 0x01, 0x01, 0x04, 0x01, 0x00, 0x06, 0x07, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x07, 0x08, 0x30, 0x0d, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x07, 0x08, 0x01, 0x02, 0x01, 0x04
        });
  
  SnmpV2cTrap y(decodeSnmpV2cTrap(x));
  xju::assert_equal(y.community_,Community("aaaa"));
  xju::assert_equal(y.id_,RequestId(67));
  xju::assert_equal(y.trapType_,Oid(".1.3.6.1.4.1.7.8"));
  xju::assert_equal(y.timestamp_,std::chrono::milliseconds(370));
  xju::assert_equal(y.vars_.size(),1);
  xju::assert_equal(y.vars_[0].first,Oid(".1.3.6.1.4.1.7.8.1"));
  xju::assert_not_equal(y.vars_[0].second->intValue(),4);
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

