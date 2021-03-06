// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "SnmpV1GetRequest.hh"
#include <xju/format.hh>
#include <algorithm>
#include "xju/snmp/Value.hh"
#include <memory>

namespace xju
{
namespace snmp
{

std::ostream& operator<<(std::ostream& s, SnmpV1GetRequest const& x) throw()
{
  return s << "community " << x.community_._ << ", id " << x.id_.value()
           << ", oids " 
           << xju::format::join(x.oids_.begin(),x.oids_.end(),", ");
}

}
}

