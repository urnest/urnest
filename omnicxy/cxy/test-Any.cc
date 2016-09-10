// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <cxy/Any.hh>

#include <iostream>
#include <xju/assert.hh>
#include <cinttypes>

namespace cxy
{

void test1() {
  int16_t const x(-23);

  ::cxy::Any<> const a(x);
  xju::assert_equal(a.isA<int16_t>(),true);

  int16_t const y(a.get<int16_t>());
  xju::assert_equal(x, y);
}

}

using namespace cxy;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}
