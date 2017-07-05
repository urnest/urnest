#ifndef _GETHOSTADDRESSES_HH
#define _GETHOSTADDRESSES_HH
// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <vector>
#include <xju/ip/v4/Address.hh>
#include "xju/HostName.hh"
#include <string>
#include "xju/Exception.hh"

namespace xju
{
namespace ip
{
namespace v4
{
// get IP v4 addresses of specified host
// pre: hostname != HostName("")
// post: result.size()
// post: std::set(result).size()==result.size()
//
std::vector<xju::ip::v4::Address> getHostAddresses(
  xju::HostName const& hostname)
  throw(
    // failure, including unknown host
    xju::Exception);


}
}
}
#endif
