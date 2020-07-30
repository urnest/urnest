// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/Optional.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

void test1() {
  xju::Optional<int> x;
  xju::assert_equal(x.valid(),false);
  x=3;
  xju::assert_equal(x.valid(),true);
  xju::assert_equal(x.value(),3);
  xju::assert_equal(*x.get(),3);
  x.clear();
  xju::assert_equal(x.valid(),false);

  xju::Optional<int> y(5);
  xju::assert_equal(y.valid(),true);
  xju::assert_equal(y.value(),5);

  xju::assert_not_equal(x,y);
  x=y;
  xju::assert_equal(x,y);
  xju::assert_equal(x.valid(),true);
  xju::assert_equal(x.value(),5);

  xju::assert_less_equal(x,y);
  xju::assert_greater_equal(x,y);

  y=6;
  xju::assert_less_equal(x,y);
  xju::assert_less(x,y);
  xju::assert_not_equal(x,y);

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

