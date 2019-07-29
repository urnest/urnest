// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/SnmpV2cGetBulkRequest.hh>
#include <xju/format.hh>

namespace xju
{
namespace snmp
{

std::ostream& operator<<(std::ostream& s, 
                         SnmpV2cGetBulkRequest const& x) throw()
{
  return s << "community " << x.community_._ << ", id " << x.id_.value()
           << ", oids " 
           << xju::format::join(x.get_.begin(),x.get_.end(),", ")
           << " and up to " << x.n_ << " values of each of oids "
           << xju::format::join(x.getNextN_.begin(),
                                x.getNextN_.end(),", ");
}


}
}

