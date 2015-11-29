// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/unix_epoch.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

void test1() {
  time_t const u(std::chrono::system_clock::to_time_t(unix_epoch()));
  struct tm m;
  ::gmtime_r(&u, &m);
  xju::assert_equal(m.tm_sec,0);
  xju::assert_equal(m.tm_min,0);
  xju::assert_equal(m.tm_hour,0);
  xju::assert_equal(m.tm_mday,1);
  xju::assert_equal(m.tm_mon,0);
  xju::assert_equal(m.tm_year,70);
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

