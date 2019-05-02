// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/linux/disk-space/monitorSpaceUsedOnFileSystem.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/path.hh>
#include <vector>
#include <xju/Exception.hh>
#include <xju/format.hh>
#include <xju/file/write.hh>
#include <sstream>
#include <xju/steadyNow.hh>
#include <chrono>
#include <xju/split.hh>
#include <xju/file/read.hh>
#include <xju/stringToUInt.hh>
#include <xju/startsWith.hh>
#include <xju/endsWith.hh>

namespace xju
{
namespace linux
{
namespace disk_space
{

auto const testDataFile{xju::path::split("sizes.txt")};

// write sizes to report from stat program stub (note sizes are free space)
void writeSizes(std::vector<unsigned int> const& sizes) throw(
                  xju::Exception)
{
  auto const x{xju::format::join(sizes.begin(),sizes.end(),std::string(","))};
  try{
    xju::file::write(testDataFile,
                     x.data(),
                     x.size(),
                     xju::file::Mode(0666));
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "write sizes " << x << " to file " << xju::path::str(testDataFile);
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

void test1(std::string const& self) {
  // { 
  //   filesys: "/",
  //   triggers: [ 90,95,97,99 ],
  //   period: "60s"
  // }
  writeSizes({1000-850,
              1000-900,
              1000-980,
              1000-950});
  std::ostringstream stdout;
  std::ostringstream stderr;
  auto const t1{xju::steadyNow()};
  monitorSpaceUsedOnFileSystem(
    xju::path::splitdir("/var"),
    {Percent(0),
     Percent(90),
     Percent(95),
     Percent(97),
     Percent(99)},
    std::chrono::milliseconds(10),
    xju::steadyNow()+std::chrono::milliseconds(100),
    self,
    stdout,
    stderr);
  auto const so{stdout.str()};
  auto const se{stderr.str()};
  xju::assert_equal(
    so,
    std::string(
      "/var has reached 90% full\n"
      "/var has reached 97% full\n"
      "/var has dropped below 96.5% full\n"));
  xju::assert_equal(true,xju::startsWith(se,std::string("Failed to check space used on file system of /var")));
  xju::assert_equal(true,xju::endsWith(se,std::string("ran out of test data.\\n\".\n")));
  xju::assert_less(xju::steadyNow(),t1+std::chrono::milliseconds(150));
  xju::assert_greater_equal(xju::steadyNow(),t1+std::chrono::milliseconds(100));
}

int statCommand(std::vector<std::string> const& params) noexcept
{
  try{
    auto const testDataFile{xju::path::split("sizes.txt")};
    auto sizes{xju::split(xju::file::read(testDataFile),',')};
    if (sizes.front()==""){
      throw xju::Exception("ran out of test data",XJU_TRACED);
    }
    auto const thisSize{xju::stringToUInt(sizes.front())};
    std::vector<unsigned int> rest;
    std::transform(sizes.begin()+1,sizes.end(),
                   std::back_inserter(rest),
                   [](std::string const& x){
                     return xju::stringToUInt(x);
                   });
    writeSizes(rest);
    std::cout << "1000 " << thisSize << " 1" << std::endl;
    return 0;
  }
  catch(xju::Exception& e){
    std::cerr << readableRepr(e) << std::endl;
    return 1;
  }
}


}
}
}

using namespace xju::linux::disk_space;

int main(int argc, char* argv[])
{
  if (argc > 1){
    return statCommand(std::vector<std::string>(argv+1,argv+argc));
  }
  unsigned int n(0);
  test1(argv[0]), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

