// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/snmp/SnmpV2cVarResponse.hh"
#include "xju/format.hh"

namespace xju
{
namespace snmp
{
SnmpV2cVarResponse::NoSuchObject::NoSuchObject(
  Oid const& oid, const xju::Traced& trace) throw():
xju::Exception("no such object " + xju::format::str(oid), trace)
{
}
SnmpV2cVarResponse::NoSuchInstance::NoSuchInstance(
  Oid const& oid, const xju::Traced& trace) throw():
xju::Exception("no such instance " + xju::format::str(oid), trace)
{
}
xju::snmp::Value const& SnmpV2cVarResponse::operator*() const throw(
  NoSuchObject,
  NoSuchInstance)
{
  if (e_.get()) {
    if (dynamic_cast<NoSuchObject const*>(e_.get())) {
      throw dynamic_cast<NoSuchObject const&>(*e_);
    }
    throw dynamic_cast<NoSuchInstance const&>(*e_);
  }
  return *v_;
}

xju::snmp::Value const* SnmpV2cVarResponse::operator->() const throw(
  NoSuchObject,
  NoSuchInstance)
{
  if (e_.get()) {
    if (dynamic_cast<NoSuchObject const*>(e_.get())) {
      throw dynamic_cast<NoSuchObject const&>(*e_);
    }
    throw dynamic_cast<NoSuchInstance const&>(*e_);
  }
  return v_.operator->();
}

std::ostream& operator<<(std::ostream& s, 
                         SnmpV2cVarResponse const& x) throw() {
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

