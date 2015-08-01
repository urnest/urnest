// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/countSignificantBits.hh"

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

void test1() {
  xju::assert_equal(countSignificantBits(0),0);
  xju::assert_equal(countSignificantBits(1),1);
  xju::assert_equal(countSignificantBits(0x10),5);
  xju::assert_equal(countSignificantBits(0x8000000000000000),64);
  xju::assert_equal(countSignificantBits(0x413b),15);
}

}

using namespace xju;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

