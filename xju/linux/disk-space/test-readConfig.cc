// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/linux/disk-space/readConfig.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/file/write.hh>
#include <tuple>
#include <xju/path.hh>
#include <xju/strip.hh>

namespace xju
{
namespace linux
{
namespace disk_space
{

void test1() {
  std::string const c{R"--(
{
  "filesys":"/",
  "triggers":[ 50,75,90,95,96 ],
  "period": "1s"
}
)--"};
  xju::file::write(xju::path::split("xxx"),
                   c.c_str(),c.size(),
                   xju::file::Mode(0666));
  xju::assert_equal(readConfig(xju::path::split("xxx")),
                    std::make_tuple(
                      xju::path::splitdir("/"),
                      std::set<Percent>({
                          Percent(0),
                          Percent(50),
                          Percent(75),
                          Percent(90),
                          Percent(95),
                          Percent(96)}),
                      std::chrono::milliseconds(1000)));
}

void test2() {
  std::string const c{R"--(
{
  "filesys":"/",
  "triggers":[ 50,75,90,95,96 ],
  "period": "1d"
}
)--"};
  auto const f{xju::path::split("xxx")};
  xju::file::write(f,c.c_str(),c.size(),xju::file::Mode(0666));
  try{
    auto const x{readConfig(f)};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to read xju linux disk-space monitor config file "+xju::path::str(f)+" because\nfailed to validate period \"1d\" (at line 5 column 13) because\nperiod \"1d\" does not end with ms, s, m or h.");
  }
}

void test3() {
  std::string const c{R"--(
{
  "filesys":"/",
  "trigger":[ 50,75,90,95,96 ],
  "period": "1s"
}
)--"};
  auto const f{xju::path::split("xxx")};
  xju::file::write(f,c.c_str(),c.size(),xju::file::Mode(0666));
  try{
    auto const x{readConfig(f)};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to read xju linux disk-space monitor config file "+xju::path::str(f)+" because\nfailed to get value of object with 3 elements (at line 2 column 1)'s \"triggers\" member because\nobject with 3 elements (at line 2 column 1) has no \"triggers\" member.");
  }
}

void test4() {
  std::string const c{R"--(
{
  "filesys":"/",
  "triggers":[ 50,75,90,95,110 ],
  "period": "1s"
}
)--"};
  auto const f{xju::path::split("xxx")};
  xju::file::write(f,c.c_str(),c.size(),xju::file::Mode(0666));
  try{
    auto const x{readConfig(f)};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to read xju linux disk-space monitor config file "+xju::path::str(f)+" because\nfailed to validate trigger level 110 (at line 4 column 28) because\n110% is invalid, must be <= 100%.");
  }
}

}
}
}

using namespace xju::linux::disk_space;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  test3(), ++n;
  test4(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

