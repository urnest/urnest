// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "SnmpV2cSetRequest.hh"
#include <sstream>
#include <xju/format.hh>

namespace xju
{
namespace snmp
{

std::ostream& operator<<(std::ostream& s, 
                         SnmpV2cSetRequest const& x) throw() {
  return s << "community " << x.community_._ << ", id " << x.id_.value()
           << ", values " 
           << xju::format::join(
             x.values_.begin(),x.values_.end(),
             [](std::pair<Oid const, std::shared_ptr<Value const> > x) {
               std::ostringstream s;
               s << x.first << ": " << (*x.second);
               return s.str();
             },
             ", ");
}


}
}

