// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_SNMPV2CVARRESPONSE_H
#define XJU_SNMP_SNMPV2CVARRESPONSE_H

#include <memory>
#include "xju/snmp/Oid.hh"
#include "xju/Exception.hh"
#include "xju/snmp/Value.hh"
#include <iostream>

namespace xju
{
namespace snmp
{
class SnmpV2cVarResponse
{
public:
  class NoSuchObject : public xju::Exception
  {
  public:
    NoSuchObject(Oid const& oid, const xju::Traced& trace) throw();
  };
  class NoSuchInstance : public xju::Exception
  {
  public:
    NoSuchInstance(Oid const& oid, const xju::Traced& trace) throw();
  };

  SnmpV2cVarResponse(Oid oid, SnmpV2cVarResponse::NoSuchObject e) throw():
      oid_(oid),
      e_(new SnmpV2cVarResponse::NoSuchObject(e))
  {
  }
  SnmpV2cVarResponse(Oid oid, SnmpV2cVarResponse::NoSuchInstance e) throw():
      oid_(oid),
      e_(new SnmpV2cVarResponse::NoSuchInstance(e))
  {
  }
  //pre: *v is valid
  SnmpV2cVarResponse(Oid oid,std::shared_ptr<Value const> v) throw():
      oid_(oid),
      v_(v)
  {
  }

  Oid const& oid() const throw()
  {
    return oid_;
  }
  xju::snmp::Value const& operator*() const throw(
    NoSuchObject,
    NoSuchInstance);
  xju::snmp::Value const* operator->() const throw(
    NoSuchObject,
    NoSuchInstance);

  std::shared_ptr<xju::snmp::Value const> value() const throw(
    NoSuchObject,
    NoSuchInstance);
  
  // return length of encoded value
  // ie return encodeTo(x)-x
  size_t encodedLength() const throw();
  
  // encode at begin, returning end of encoding
  virtual std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw();

  
private:
  Oid oid_;
  std::shared_ptr<xju::Exception const> e_;
  std::shared_ptr<Value const> v_;
  
  friend std::ostream& operator<<(std::ostream& s, SnmpV2cVarResponse const& x)
    throw();

  friend bool operator<(SnmpV2cVarResponse const& x,
                        SnmpV2cVarResponse const& y) noexcept
  {
    if (x.oid_<y.oid_) return true;
    if (y.oid_<x.oid_) return false;
    if (!x.e_.get()&&y.e_.get()) return true;
    if (!y.e_.get()&&x.e_.get()) return false;
    if (x.e_.get() && y.e_.get()){
      auto const sx{readableRepr(*x.e_)};
      auto const sy{readableRepr(*y.e_)};
      if (sx<sy) return true;
      if (sy<sx) return false;
    }
    if (!x.v_.get()&&y.v_.get()) return true;
    if (!y.v_.get()&&x.v_.get()) return false;
    if (x.v_.get() && y.v_.get()) return (*x.v_)<(*y.v_);
    return false;
  }
  friend bool operator>(SnmpV2cVarResponse const& x,
                        SnmpV2cVarResponse const& y) noexcept
  {
    return y<x;
  }
  friend bool operator==(SnmpV2cVarResponse const& x,
                         SnmpV2cVarResponse const& y) noexcept
  {
    return !(x<y) && !(y<x);
  }
  friend bool operator<=(SnmpV2cVarResponse const& x,
                         SnmpV2cVarResponse const& y) noexcept
  {
    return x<y || x==y;
  }
  friend bool operator>=(SnmpV2cVarResponse const& x,
                         SnmpV2cVarResponse const& y) noexcept
  {
    return x>y || x==y;
  }
  friend bool operator!=(SnmpV2cVarResponse const& x,
                         SnmpV2cVarResponse const& y) noexcept
  {
    return !(x==y);
  }
  
};


}
}

#endif

