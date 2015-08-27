// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/snmp/SnmpV2cNextVarResponse.hh"
#include "xju/format.hh"

namespace xju
{
namespace snmp
{
SnmpV2cNextVarResponse::EndOfMibView::EndOfMibView(
  Oid const& oid, const xju::Traced& trace) throw():
    xju::Exception("end of mib view at " + xju::format::str(oid), trace)
{
}

xju::snmp::Value const& SnmpV2cNextVarResponse::operator*() const throw(
  EndOfMibView)
{
  if (e_.get()) {
    throw dynamic_cast<EndOfMibView const&>(*e_);
  }
  return *v_;
}

std::ostream& operator<<(std::ostream& s, 
                         SnmpV2cNextVarResponse const& x) throw() {
  if (x.e_.get()) {
    s << x.oid_ << ": " << readableRepr(*x.e_);
  }
  else
  {
    s << x.oid_ << ": " << (*x.v_);
  }
  return s;
}
               

}
}

