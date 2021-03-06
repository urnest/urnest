// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/strip.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

void test1() {
  assert_equal(strip(""),"");
  assert_equal(strip("fred"),"fred");
  assert_equal(strip("   "),"");
  assert_equal(strip(" fred"),"fred");
  assert_equal(strip("fred "),"fred");
  assert_equal(strip("  fred  "),"fred");
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

