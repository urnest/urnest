// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <string>
#include <xju/path.hh>
#include <set>
#include <xju/linux/disk-space/Percent.hh>
#include <chrono>
#include <xju/steadyNow.hh>
#include <iostream>
#include <xju/linux/disk-space/trackLevel.hh>
#include <xju/linux/disk-space/statFileSystem.hh>
#include <thread>
#include <xju/prev.hh>
#include <xju/format.hh>
#include <vector>
#include <memory>
#include <xju/Thread.hh>
#include <algorithm>
#include <iterator>
#include <xju/stringToUInt.hh>
#include <sstream>
#include <xju/endsWith.hh>
#include <xju/json/parse.hh>
#include <xju/file/read.hh>


namespace xju
{
namespace linux
{
namespace disk_space
{

//pre: triggers contains 0%
void monitor(std::string const& self,
             xju::path::AbsolutePath const& path,
             std::set<Percent> const& triggers,
             std::chrono::milliseconds const& period) noexcept
{
  xju::assert_not_equal(triggers.find(Percent(0)),triggers.end());
  
  //current is lower bound of disk space use, initially assumed 0%
  auto current{triggers.begin()};
  std::chrono::steady_clock::time_point nextPoll{xju::steadyNow()};
  while(true){
    nextPoll+=period;
    try{
      auto i{trackLevel(current,
                        triggers,
                        statFileSystemOfPath(path,nextPoll))};
      if (i!=triggers.end() &&
          ((current==triggers.end()) || (*i<*current))) {
        std::cout << xju::path::str(path) << " less than "
                  << xju::format::str((*i)) << "% full" << std::endl;
      }
      else if (current!=triggers.end() &&
               ((i==triggers.end() || (*i>*current)))) {
        std::cout << xju::path::str(path)
                  << xju::format::str((*xju::prev(i))) << "% full"
                  << std::endl;
      }
      current=i;
    }
    catch(xju::Exception& e){
      std::cerr << readableRepr(e) << std::endl;
    }
    std::this_thread::sleep_until(nextPoll);
  }
}

std::tuple<xju::path::AbsolutePath,
           std::set<Percent>,
           std::chrono::milliseconds> readConfig(
             xju::path::AbsFile const& configFile) throw(
               xju::Exception)
{
  try{
    auto const j{
      xju::json::parse(xju::Utf8String(xju::file::read(configFile)))};
    auto const path{
      xju::path::splitdir(
        j->getMember(xju::Utf8String("filesys")).asString())};
    std::set<Percent> levels{Percent(0)};
    for(auto l: j->getMember(xju::Utf8String("triggers")).asArray()){
      levels.insert(Percent(l->asInt()));
    }
    std::string const period{
      j->getMember(xju::Utf8String("period")).asString()};
    if (xju::endsWith(period,std::string("ms"))){
      return std::make_tuple(
        path,
        levels,
        std::chrono::milliseconds(
          xju::stringToUInt(
            std::string(period.begin(),period.end()-2))));
    }
    else if (xju::endsWith(period,std::string("s"))){
      return std::make_tuple(
        path,
        levels,
        std::chrono::milliseconds(
          1000*
          xju::stringToUInt(
            std::string(period.begin(),period.end()-1))));
    }
    else if (xju::endsWith(period,std::string("m"))){
      return std::make_tuple(
        path,
        levels,
        std::chrono::milliseconds(
          60UL*1000UL*
          xju::stringToUInt(
            std::string(period.begin(),period.end()-1))));
    }
    else if (xju::endsWith(period,std::string("h"))){
      return std::make_tuple(
        path,
        levels,
        std::chrono::milliseconds(
          60UL*60UL*1000UL*
          xju::stringToUInt(
            std::string(period.begin(),period.end()-1))));
    }
    std::ostringstream s;
    s << "period " << xju::format::quote(period)
      << " does not end with ms, s, m or h";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "read config file " << xju::path::str(configFile);
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

}
}
}
using namespace xju::linux::disk_space;

int main(int argc, char* argv[])
{
  if (argc==2 && std::string("-h")==argv[1])
  {
    std::cout << "usage: " << argv[0] <<
R"--( [ <config-file>... ]
  ... where config-file contains a json object containing
    filesys : string naming file system\n"
    triggers: list of levels (each a percentage) at which to
              print \"X% full\" messages to stdout
    period:   string specifying how often to check e.g. "15s"
              available units: 
                ms (milliseconds), s (seconds), m (minutes), h (hours)
  e.g.
  { 
    filesys: "/",
    triggers: [ 90,95,97,99 ],
    period: "60s"
  }
  ... will trigger "X% full" messages at 90% full, again at 95% full etc.
  Note that as disk usage drops back down, it must drop 0.5% below the
  trigger point for the next lower level to apply; when it does  
  a "below X% full" message is printed.

  Runs forever. On startup, acts as if the last known disk usage was 0%.
)--";
    return 0;
  }
  std::vector<std::unique_ptr<xju::Thread> > threads;
  std::transform(argv+1,argv+argc,
                 std::back_inserter(threads),
                 [&](std::string const& configFile) {
                   auto const x{readConfig(xju::path::split(configFile))};
                   return std::unique_ptr<xju::Thread>(
                     new xju::Thread(
                       [&]()
                       {
                         monitor(argv[0],
                                 std::get<0>(x),
                                 std::get<1>(x),
                                 std::get<2>(x));
                       }));
                     });
  return 0;
}
