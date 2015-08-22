// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "SnmpV2cGetRequest.hh"

#include <iostream>
#include <xju/assert.hh>
#include "xju/snmp/encode.hh"

namespace xju
{
namespace snmp
{

void test1() {
  SnmpV2cGetRequest r(
    Community("private"),
    RequestId(1),
    {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0")});
  std::vector<uint8_t> x(encode(r));
  xju::assert_equal(
    x,
    std::vector<uint8_t>({
        0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA0,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
          }));

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

