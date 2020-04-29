// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_OIDVALUE_H
#define XJU_SNMP_OIDVALUE_H

#include "xju/snmp/Value.hh"
#include "xju/snmp/Oid.hh"

namespace xju
{
namespace snmp
{

class OidValue : public Value
{
public:
  ~OidValue() throw(){}
  
  explicit OidValue(Oid const& val) throw();
  Oid const val_;

  // Value::
  Oid oidValue() const /*throw(xju::Exception)*/ override
  {
    return val_;
  }

  // Value::
  std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw() override;

  // Value::
  virtual std::string str() const throw() override
  {
    return val_.toString();
  }

  // Value::
  // pre: y is a OidValue
  virtual bool less(Value const& y) const throw() override;

  friend bool operator<(OidValue const& x, OidValue const& y) throw()
  {
    return x.val_<y.val_;
  }
  friend bool operator>(OidValue const& x, OidValue const& y) throw()
  {
    return x.val_>y.val_;
  }
  friend bool operator==(OidValue const& x, OidValue const& y) throw()
  {
    return x.val_==y.val_;
  }
  friend bool operator!=(OidValue const& x, OidValue const& y) throw()
  {
    return x.val_!=y.val_;
  }
  friend bool operator<=(OidValue const& x, OidValue const& y) throw()
  {
    return x.val_<=y.val_;
  }
  friend bool operator>=(OidValue const& x, OidValue const& y) throw()
  {
    return x.val_>=y.val_;
  }

};


}
}

#endif

