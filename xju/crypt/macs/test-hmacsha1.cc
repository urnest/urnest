// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/crypt/macs/hmacsha1.hh>

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
    {0x43,0x8a,0x93,0x10,0x0a,0x7c,0x7a,0x21,0x42,0xf0,
     0x18,0x98,0xe1,0x09,0xf2,0xdd,0x8c,0x88,0x1b,0x0d});
  hmacsha1::Calculator c(key);
  Mac const m(c.calculateMac(std::vector<uint8_t>(message.begin(),
                                                  message.end())));
  hmacsha1::Verifier v(key);
  v.verifyMac(m,std::vector<uint8_t>(message.begin(),message.end()));

  std::string badMessage("fred the fox was bold as brass");
  try{
    v.verifyMac(m,std::vector<uint8_t>(badMessage.begin(),badMessage.end()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to verify hmac-sha1 MAC of 30-byte message using MAC key 438a93100a7c7a2142f01898e109f2dd8c881b0d is cfede5c5cc920fae34d9f4ecf7fdb47d8b5b2b0f because\nMAC is 73571fdf7b89e12b88573a86e496072b1ee48ef0.");
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

