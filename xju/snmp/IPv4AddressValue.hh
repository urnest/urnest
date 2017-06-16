// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_IPV4ADDRESSVALUE_H
#define XJU_SNMP_IPV4ADDRESSVALUE_H
#include "xju/snmp/Value.hh"
#include "xju/snmp/IPv4Address.hh"

namespace xju
{
namespace snmp
{

class IPv4AddressValue : public Value
{
public:
  explicit IPv4AddressValue(IPv4Address val) throw();

  IPv4Address const val_;

  // Value::
  virtual std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw();
  
  // Value::
  virtual std::string str() const throw();

  friend bool operator<(IPv4AddressValue const& x, IPv4AddressValue const& y) throw()
  {
    return x.val_<y.val_;
  }
  friend bool operator>(IPv4AddressValue const& x, IPv4AddressValue const& y) throw()
  {
    return x.val_>y.val_;
  }
  friend bool operator==(IPv4AddressValue const& x, IPv4AddressValue const& y) throw()
  {
    return x.val_==y.val_;
  }
  friend bool operator!=(IPv4AddressValue const& x, IPv4AddressValue const& y) throw()
  {
    return x.val_!=y.val_;
  }
  friend bool operator<=(IPv4AddressValue const& x, IPv4AddressValue const& y) throw()
  {
    return x.val_<=y.val_;
  }
  friend bool operator>=(IPv4AddressValue const& x, IPv4AddressValue const& y) throw()
  {
    return x.val_>=y.val_;
  }
};
  

}
}

#endif

