// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "IPv4Address.hh"
#include "xju/format.hh"

namespace xju
{
namespace snmp
{

std::ostream& operator<<(std::ostream& s, IPv4Address const& x) 
throw()
{
  return s << xju::format::int_(x[0]) << "." 
           << xju::format::int_(x[1]) << "." 
           << xju::format::int_(x[2]) << "." 
           << xju::format::int_(x[3]);
}


}
}

