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
#include <xju/steadyEternity.hh>
#include <xju/linux/ntpd/readConfig.hh>
#include <xju/linux/ntpd/monitorNtpSync.hh>

int main(int argc, char* argv[])
{
  if (argc!=2)
  {
    std::cout << "usage: " << argv[0] <<
R"--( <config-file>
  ... where config-file contains a json object containing
    delayFirstCheckFor: string specifying how long to delay first
                        ntp check (see period for format)
    period:   string specifying how often to check e.g. "15s"
              available units: 
                ms (milliseconds), s (seconds), m (minutes), h (hours)
  e.g.
  { 
    delayFirstCheckFor: "2m",
    period: "15s"
  }
  ... will wait 2m then forever check ntp sync every 15s, generating:
    "system clock not synchronised" - when clock becomes unsynchronised and
                                      then every 15s while clock remains
                                      unsynchronised
    "system clock synchronised" - when clock becomes synchronised
  ... on stdout, detailing ntpq failures on stderr.

  Runs forever. On startup assumes clock is already synchronised.
)--";
    return 1;
  }
  std::pair<
    std::chrono::milliseconds, // delayFirstCheckFor
    std::chrono::milliseconds  // period
    > config{xju::linux::ntpd::readConfig(xju::path::split(argv[1]))};

  std::this_thread::sleep_for(config.first);
  xju::linux::ntpd::monitorNtpSync(config.second,
                                   xju::steadyEternity(),
                                   "/usr/bin/ntpq",
                                   std::cout,
                                   std::cerr);
  return 0;
}
