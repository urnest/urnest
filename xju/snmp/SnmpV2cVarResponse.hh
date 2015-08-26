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
  class EndOfMibView : public xju::Exception
  {
  public:
    EndOfMibView(Oid const& oid, const xju::Traced& trace) throw();
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
  SnmpV2cVarResponse(Oid oid, SnmpV2cVarResponse::EndOfMibView e) throw():
      oid_(oid),
      e_(new SnmpV2cVarResponse::EndOfMibView(e))
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
    NoSuchInstance,
    EndOfMibView);
  
private:
  Oid oid_;
  std::shared_ptr<xju::Exception const> e_;
  std::shared_ptr<Value const> v_;
  
  friend std::ostream& operator<<(std::ostream& s, SnmpV2cVarResponse const& x)
    throw();
};


}
}

#endif

