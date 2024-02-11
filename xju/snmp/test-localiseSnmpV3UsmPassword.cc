// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/localiseSnmpV3UsmPassword.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/crypt/hashers/SHA512.hh>

namespace xju
{
namespace snmp
{

void test1() {
  xju::assert_equal(localiseSnmpV3UsmPassword<xju::crypt::hashers::SHA512>(
                      ContextEngineID({0xaa}),
                      xju::Password("fred")),
                    std::vector<uint8_t>({
                        0x58, 0xab, 0xcc, 0xdb, 0x86, 0xf5, 0xdb, 0x05, 0x2a, 0x29, 0xfa, 0xa8, 0x95, 0x75, 0x5f, 0x54, 0x26, 0xf3, 0x3c, 0xae, 0x18, 0x71, 0x2a, 0xfb, 0x9a, 0x86, 0xb2, 0x70, 0xe5, 0xf4, 0x65, 0x2e, 0x58, 0xf6, 0xeb, 0xcc, 0x98, 0x7e, 0x38, 0x4a, 0x91, 0xf0, 0xb2, 0x6d, 0x3e, 0xb5, 0x20, 0x22, 0x07, 0xcc, 0xfb, 0x30, 0xb0, 0xca, 0xdf, 0x4a, 0x24, 0xd4, 0x4b, 0x70, 0xf5, 0xa4, 0xba, 0x10}));
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

