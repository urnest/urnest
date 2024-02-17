// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/crypt/hashers/SHA256.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace crypt
{
namespace hashers
{

void test1() {
  SHA256 h;
  xju::assert_equal(h.hash({1,2,3,4,5}), std::vector<uint8_t>({
        116, 248, 31, 225, 103, 217, 155, 76,
        180, 29, 109, 12, 205, 168, 34, 120,
        202, 238, 159, 62, 47, 37, 213, 229,
        163, 147, 111, 243, 220, 236, 96, 208
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

