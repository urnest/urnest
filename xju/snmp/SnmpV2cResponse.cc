// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "SnmpV2cResponse.hh"
#include <xju/format.hh>

namespace xju
{
namespace snmp
{

std::ostream& operator<<(std::ostream& s, SnmpV2cResponse const& x) throw()
{
  return s << "type " << xju::format::hex(x.responseType_)
           << ", community " << x.community_._
           << ", id " << x.id_.value()
           << ", error status " << (int)x.error_
           << ", error index " << x.errorIndex_.value()
           << ", values "
           << xju::format::join(
             x.varResults_.begin(), x.varResults_.end(),", ");
}

std::ostream& operator<<(std::ostream& s, 
                         SnmpV2cResponse::VarResult const& x) throw() {
  if (x.e_.valid()) {
    s << x.oid_ << ": " << x.e_.value();
  }
  else
  {
    s << x.oid_ << ": " << (*x.v_);
  }
  return s;
}
               
std::ostream& operator<<(std::ostream& s,
                         SnmpV2cResponse::VarResult::E const x) throw() {
  switch(x) {
  case SnmpV2cResponse::VarResult::NO_SUCH_OBJECT:
    return s << "NO_SUCH_OBJECT";
  case SnmpV2cResponse::VarResult::NO_SUCH_INSTANCE:
    return s << "NO_SUCH_INSTANCE";
  case SnmpV2cResponse::VarResult::END_OF_MIB_VIEW:
    return s << "END_OF_MIB_VIEW";
  }
  return s << "(exception " << (int)x << ")";
}

}
}

