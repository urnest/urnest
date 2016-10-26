// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/Thread.hh>

#include <iostream>
#include <xju/assert.hh>

#include <chrono>

namespace xju
{

void test1() {
  bool x(false);
  {
    xju::Thread t([&]() { x=true; });
  }
  xju::assert_equal(x,true);

  x=false;
  {
    xju::Thread t([&]() {
        while(!x) {
          std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
      },
      [&]() {
        x=true;
      });
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

