// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/MemIBuf.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

void test1() {
  std::string const v("onetwothree");
  MemIBuf x(v.begin(),v.end(),3);
  auto y(x.underflow());
  xju::assert_equal(y.second-y.first,3U);
  xju::assert_equal(std::string(y.first,y.second),"one");
  y=x.underflow();
  xju::assert_equal(y.second-y.first,3U);
  xju::assert_equal(std::string(y.first,y.second),"two");
  y=x.underflow();
  xju::assert_equal(y.second-y.first,3U);
  xju::assert_equal(std::string(y.first,y.second),"thr");
  y=x.underflow();
  xju::assert_equal(y.second-y.first,2U);
  xju::assert_equal(std::string(y.first,y.second),"ee");
  y=x.underflow();
  xju::assert_equal(y.second-y.first,0U);
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

