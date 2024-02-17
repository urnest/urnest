// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/crypt/hashers/SHA1.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace crypt
{
namespace hashers
{

void test1() {
  SHA1 h;
  xju::assert_equal(h.hash({1,2,3,4,5}), std::vector<uint8_t>({
        17, 150, 106, 185, 192, 153, 248, 250,
        190, 250, 197, 76, 8, 213, 190, 43,
        216, 201, 3, 175
      }));
}

}
}
}

using namespace xju::crypt::hashers;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

