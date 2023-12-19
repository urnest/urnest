// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/crypt/macs/hmacsha512.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/Exception.hh>

namespace xju
{
namespace crypt
{
namespace macs
{

void test1() {

  std::string message("fred the fox was big and brown");
  MacKey const key(
    {0x43,0x8a,0x93,0x10,0x0a,0x7c,0x7a,0x21,0x42,0xf0,0x0a,0x7c,0x7a,0x21,0x42,0xf0,
     0x18,0x98,0xe1,0x09,0xf2,0xdd,0x8c,0x88,0x1b,0x0d,0x0a,0x7c,0x7a,0x21,0x42,0xf0,
     0x43,0x8a,0x93,0x10,0x0a,0x7c,0x7a,0x21,0x42,0xf0,0x0a,0x7c,0x7a,0x21,0x42,0xf0,
     0x18,0x98,0xe1,0x09,0xf2,0xdd,0x8c,0x88,0x1b,0x0d,0x0a,0x7c,0x7a,0x21,0x42,0xf0});
  hmacsha512::Calculator c(key);
  Mac const m(c.calculateMac(std::vector<uint8_t>(message.begin(),
                                                  message.end())));
  hmacsha512::Verifier v(key);
  v.verifyMac(m,std::vector<uint8_t>(message.begin(),message.end()));

  std::string badMessage("fred the fox was bold as brass");
  try{
    v.verifyMac(m,std::vector<uint8_t>(badMessage.begin(),badMessage.end()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to verify hmac-sha512 MAC of 30-byte message using MAC key 438a93100a7c7a2142f00a7c7a2142f01898e109f2dd8c881b0d0a7c7a2142f0438a93100a7c7a2142f00a7c7a2142f01898e109f2dd8c881b0d0a7c7a2142f0 is 6494c1b8811d0e668cb00a19f06c7e6ee395ade0e74cfd9a9d0d326736fad187dbf141a4f2e5154454ae73965e610cfc3cdd6ac45d03ee474fd4ce86d39c75ae because\nMAC is d267f8a8067d3782ff436a14306acd5659e5705aad5b6083f84cdeb32890b453c3dd0992ede3e8abd3c0d4216bedba28d294024165e8a7f204c689dcf4eea3e6.");
  }
}

}
}
}

using namespace xju::crypt::macs;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}
