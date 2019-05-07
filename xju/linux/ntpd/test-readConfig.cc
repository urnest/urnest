// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/linux/ntpd/readConfig.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/file/write.hh>
#include <xju/file/Mode.hh>
#include <xju/path.hh>

namespace xju
{
namespace linux
{
namespace ntpd
{

void test1() {
  std::string const c{R"--(
{
  "delayFirstCheckFor":"30s",
  "period":"10s"
}
)--"};
  xju::file::write(xju::path::split("xxx"),
                   c.c_str(),c.size(),
                   xju::file::Mode(0666));
  xju::assert_equal(readConfig(xju::path::split("xxx")),
                    std::make_tuple(
                      std::chrono::milliseconds(30000),
                      std::chrono::milliseconds(10000),
                      xju::path::split("/usr/bin/ntpq")));
}
void test2() {
  std::string const c{R"--(
{
  "delayFirstCheckFor":"30s",
  "period":"10s",
  "ntpq": "/usr/sbin/ntpq"
}
)--"};
  xju::file::write(xju::path::split("xxx"),
                   c.c_str(),c.size(),
                   xju::file::Mode(0666));
  xju::assert_equal(readConfig(xju::path::split("xxx")),
                    std::make_tuple(
                      std::chrono::milliseconds(30000),
                      std::chrono::milliseconds(10000),
                      xju::path::split("/usr/sbin/ntpq")));
}

}
}
}

using namespace xju::linux::ntpd;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

