// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/crypt/randl.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace crypt
{

void test1() {
  xju::assert_less(randl((uint32_t)0x775526),0x775526);
  xju::assert_less(randl((uint64_t)0x793cfa75526),0x793cfa75526);
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

