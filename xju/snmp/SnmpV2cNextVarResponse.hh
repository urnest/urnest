// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_SNMPV2CNEXTVARRESPONSE_H
#define XJU_SNMP_SNMPV2CNEXTVARRESPONSE_H

#include <memory>
#include "xju/snmp/Oid.hh"
#include "xju/Exception.hh"
#include "xju/snmp/Value.hh"
#include <iostream>

namespace xju
{
namespace snmp
{
class SnmpV2cNextVarResponse
{
public:
  class EndOfMibView : public xju::Exception
  {
  public:
    EndOfMibView(Oid const& oid, const xju::Traced& trace) throw();
  };

  SnmpV2cNextVarResponse(Oid oid, SnmpV2cNextVarResponse::EndOfMibView e) 
    throw():
      oid_(oid),
      e_(new SnmpV2cNextVarResponse::EndOfMibView(e))
  {
  }
  //pre: *v is valid
  SnmpV2cNextVarResponse(Oid oid,std::shared_ptr<Value const> v) throw():
      oid_(oid),
      v_(v)
  {
  }

  Oid const& oid() const throw()
  {
    return oid_;
  }
  xju::snmp::Value const& operator*() const /*throw(
    EndOfMibView)*/;
  
private:
  Oid oid_;
  std::shared_ptr<xju::Exception const> e_;
  std::shared_ptr<Value const> v_;
  
  friend std::ostream& operator<<(std::ostream& s, 
                                  SnmpV2cNextVarResponse const& x)
    throw();
};


}
}

#endif

