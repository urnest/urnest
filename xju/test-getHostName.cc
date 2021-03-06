// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/getHostName.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

void test1(std::string actualHostName) {
  xju::assert_equal(xju::getHostName(),xju::HostName(actualHostName));
}

}

using namespace xju;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(argv[1]), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

