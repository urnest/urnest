// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/linux/ntpd/monitorNtpSync.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/path.hh>
#include <xju/format.hh>
#include <xju/file/write.hh>
#include <xju/file/Mode.hh>
#include <sstream>
#include <xju/startsWith.hh>
#include <xju/endsWith.hh>
#include <xju/split.hh>
#include <xju/stringToUInt.hh>
#include <algorithm>
#include <xju/file/read.hh>

namespace xju
{
namespace linux
{
namespace ntpd
{

auto const testDataFile{xju::path::split("statuses.txt")};

// write PeerStatuses to report from ntpq program stub
void writePeerStatuses(std::vector<PeerStatus> const& sizes) throw(
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
    s << "write PeerStatuses " << x << " to file "
      << xju::path::str(testDataFile);
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

void test1(xju::path::AbsFile const& self) {
  writePeerStatuses({PeerStatus(0x8811),
                     PeerStatus(0x151a),
                     PeerStatus(0x161a), //good
                     PeerStatus(0x161a), //good
                     PeerStatus(0x143a)});
  std::ostringstream stdout;
  std::ostringstream stderr;
  auto const t1{xju::steadyNow()};
  monitorNtpSync(
    std::chrono::milliseconds(10),
    xju::steadyNow()+std::chrono::milliseconds(150),
    self,
    stdout,
    stderr);
  auto const so{stdout.str()};
  auto const se{stderr.str()};
  xju::assert_equal(
    so,
    std::string(
      "system clock not synchronised\n"
      "system clock not synchronised\n"
      "system clock synchronised\n"
      "system clock not synchronised\n"));
  xju::assert_equal(true,xju::startsWith(se,std::string("Failed to check time sync status using ntpq command ")));
  xju::assert_equal(true,xju::endsWith(se,std::string("ran out of test data.\\n\".\n")));
  xju::assert_less(xju::steadyNow(),t1+std::chrono::milliseconds(200));
  xju::assert_greater_equal(xju::steadyNow(),t1+std::chrono::milliseconds(150));
}

int ntpqCommand(std::vector<std::string> const& params) noexcept
{
  try{
    auto statuses{xju::split(xju::file::read(testDataFile),',')};
    if (statuses.front()==""){
      throw xju::Exception("ran out of test data",XJU_TRACED);
    }
    auto const thisSize{xju::stringToUInt(statuses.front())};
    std::vector<PeerStatus> rest;
    std::transform(statuses.begin()+1,statuses.end(),
                   std::back_inserter(rest),
                   [](std::string const& x){
                     return PeerStatus(xju::stringToUInt(x));
                   });
    writePeerStatuses(rest);
    std::cout << R"--(
ind assid status  conf reach auth condition  last_event cnt
===========================================================
  1 42977  )--" << xju::format::int_(thisSize,16) << R"--(  yes  none  none    reject    mobilize  1)--" << std::endl;
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

using namespace xju::linux::ntpd;

int main(int argc, char* argv[])
{
  if (argc > 1){
    return ntpqCommand(std::vector<std::string>(argv+1,argv+argc));
  }
  unsigned int n(0);
  test1(xju::path::split(argv[0])), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

