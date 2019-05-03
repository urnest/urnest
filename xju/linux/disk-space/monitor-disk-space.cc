// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//



#include <vector>
#include <memory>
#include <xju/Thread.hh>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <xju/linux/disk-space/readConfig.hh>
#include <xju/linux/disk-space/monitorSpaceUsedOnFileSystem.hh>
#include <xju/steadyEternity.hh>
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
              print \"/ has reached X% full\" messages to stdout
    period:   string specifying how often to check e.g. "15s"
              available units: 
                ms (milliseconds), s (seconds), m (minutes), h (hours)
  e.g.
  { 
    filesys: "/",
    triggers: [ 90,95,97,99 ],
    period: "60s"
  }
  ... will trigger "/ has reached X% full" messages at 90% full, again at 
  95% full etc.

  Note that as disk usage drops back down, it must drop 0.5% below the
  trigger point for the next lower level to apply; when it does  
  a "/ has dropped below X% full" message is printed.
  
  Runs forever. On startup, acts as if the last known disk usage was 0%.

  Note that 100% full means 0-blocks available to user root, i.e. uses
  stat "%f" not "%a" (see stat(1) manpage).
)--";
    return 0;
  }
  std::vector<
    std::tuple<xju::path::AbsolutePath,
               std::set<Percent>,
               std::chrono::milliseconds> > configs;
  std::transform(
    argv+1,argv+argc,
    std::back_inserter(configs),
    [&](std::string const& configFile) {
      return xju::linux::disk_space::readConfig(xju::path::split(configFile));
    });
    
  std::vector<std::unique_ptr<xju::Thread> > threads;
  std::transform(
    configs.begin(),configs.end(),
    std::back_inserter(threads),
    [&](std::tuple<xju::path::AbsolutePath,
        std::set<Percent>,
        std::chrono::milliseconds> const& config) {
      return std::unique_ptr<xju::Thread>(
        new xju::Thread(
          [&]()
          {
            xju::linux::disk_space::monitorSpaceUsedOnFileSystem(
              std::get<0>(config),
              std::get<1>(config),
              std::get<2>(config),
              xju::steadyEternity(),
              "/usr/bin/stat",
              std::cout,
              std::cerr);
          }));
    });
  return 0;
}
