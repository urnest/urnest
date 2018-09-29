// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ip/decode.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace ip
{
namespace decode
{

void test1() {
  std::vector<uint8_t> const x{1,0x9B,3,5,0xaf,0x37,0xb2,0x8e};
  auto i{makeIterator(x.begin(),x.end())};
  xju::assert_equal(i,makeIterator(x.begin(),x.end()));

  xju::assert_equal(i.get8Bits(""),1);
  xju::assert_equal(i.get4Bits(""),9);
  xju::assert_equal(i.get3Bits(""),5);
  xju::assert_equal(i.get1Bit(""),1);
  xju::assert_equal(i.get16Bits(""),0x0305);
  xju::assert_equal(i.get32Bits(""),0xaf37b28e);
  xju::assert_equal(i.atEnd(),true);
  try
  {
    i.get1Bit("x");
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to read 1 bit \"x\" having read 8 bytes and 0 bits because\nend of input.");
  }
}

}
}
}

using namespace xju::ip::decode;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

