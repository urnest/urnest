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

#include <ctime>

namespace xju
{
std::chrono::system_clock::time_point unix_epoch() throw()
{
  // we just assume that time_t uses the unix epoch - test-unix_epoch.cc
  // verifies this
  return std::chrono::system_clock::from_time_t(0);
}

}
