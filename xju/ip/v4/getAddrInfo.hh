#ifndef _GETADDRINFO_HH
#define _GETADDRINFO_HH
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
#include "xju/ip/Port.hh"
#include <string>
#include "xju/Exception.hh"

namespace xju
{
namespace ip
{
namespace v4
{
// see getaddrinfo manpage for ai_socktype, eg SOCK_STREAM, SOCK_DGRAM
// see getaddrinfo manpage for ai_protocol, for ip, 0 for above socktypes
// pre: ai_socktype!=0
//      hostname.size()
//      service.size()
//
std::vector<std::pair<xju::ip::v4::Address,xju::ip::Port> > getAddrInfo(
  std::string const& hostname,
  std::string const& service,
  int ai_socktype,
  int ai_protocol)
  throw(
    xju::Exception);


}
}
}
#endif
