// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/startsWith.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

void test1() {
  xju::assert_equal(xju::startsWith(std::string("fred"),std::string("fr")),true);
  xju::assert_equal(xju::startsWith(std::string("fred"),std::string("fredx")),false);
  xju::assert_equal(xju::startsWith(std::string("fred"),std::string("xfr")),false);

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

