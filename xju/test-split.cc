// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/split.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

void test1() {
  xju::assert_equal(xju::split(std::string("fred"),':'),
                    std::vector<std::string>{"fred"});
  xju::assert_equal(xju::split(std::string("fred:jock"),':'),
                    std::vector<std::string>{"fred","jock"});
  xju::assert_equal(xju::split(std::string("fred::jock"),':'),
                    std::vector<std::string>{"fred","","jock"});
  xju::assert_equal(xju::split(std::string("fred:jock:sally"),':'),
                    std::vector<std::string>{"fred","jock","sally"});
  xju::assert_equal(xju::split(std::string(""),':'),
                    std::vector<std::string>{""});
  xju::assert_equal(xju::split(std::string(":fred"),':'),
                    std::vector<std::string>{"","fred"});
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

