// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/crypt/findNthOne.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace crypt
{

void test1() {
  std::vector<uint8_t> x{0x00,0xf1, 0x83};
  auto i(findNthOne(xju::BitCount(1), bit_begin(x)));
  i.set(false);
  xju::assert_equal(x, std::vector<uint8_t>({0x00,0xf1, 0x82}));
  i=findNthOne(xju::BitCount(4), i);
  i.set(false);
  xju::assert_equal(x, std::vector<uint8_t>({0x00,0xe1, 0x82}));
}

}
}

using namespace xju::crypt;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

