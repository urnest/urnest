// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/StrOBuf.hh>

#include <iostream>
#include <xju/assert.hh>
#include <algorithm>

namespace xju
{

void test1() {
  {
    StrOBuf x(5,12);
    auto space=x.flush(0);
    xju::assert_equal(space.second-space.first,5U);
    std::string four("four");
    space.first=std::copy(four.begin(),four.end(),space.first);
    space=x.flush(space.first);
    xju::assert_equal(std::string(x.str().first,x.str().second),four);
    xju::assert_equal(space.second-space.first,1U);
    *space.first++=',';
    space=x.flush(space.first);
    xju::assert_equal(std::string(x.str().first,x.str().second),four+",");
    xju::assert_equal(space.second-space.first,5U);
    std::string const five(" five");
    space.first=std::copy(five.begin(),five.end(),space.first);
    xju::assert_equal(space.first,space.second);
    space=x.flush(space.first);
    xju::assert_equal(std::string(x.str().first,x.str().second),"four, five");
    xju::assert_equal(space.second-space.first,2U);
    std::string const seven("67");
    space.first=std::copy(seven.begin(),seven.end(),space.first);
    space=x.flush(space.first);
    xju::assert_equal(std::string(x.str().first,x.str().second),"four, five67");
    xju::assert_equal(space.second-space.first,0U);
  }
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

