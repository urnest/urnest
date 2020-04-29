// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_OID_H
#define XJU_SNMP_OID_H

#include <string>
#include "xju/Exception.hh"
#include <vector>
#include <iostream>
#include <stdint.h>

namespace xju
{
namespace snmp
{

// RFC 1157 OID
class Oid
{
public:
  // construct from dotted, eg .1.3.6.1.4.364
  explicit Oid(std::string const& dotted) /*throw(
    xju::Exception)*/;
  explicit Oid(std::vector<uint32_t> const& components) throw():
      components_(components)
  {
  }
  std::string toString() const throw();
  std::vector<uint32_t> const& components() const throw()
  {
    return components_;
  }
  // *this is a prefix of y or is y
  bool contains(Oid const& y) const throw();
  
private:
  std::vector<uint32_t> components_;

  friend bool operator==(Oid const& x, Oid const& y) throw()
  {
    return x.components_ == y.components_;
  }
  friend bool operator!=(Oid const& x, Oid const& y) throw()
  {
    return x.components_ != y.components_;
  }
  friend bool operator<(Oid const& x, Oid const& y) throw()
  {
    return x.components_ < y.components_;
  }
  friend bool operator>(Oid const& x, Oid const& y) throw()
  {
    return x.components_ > y.components_;
  }
  friend bool operator<=(Oid const& x, Oid const& y) throw()
  {
    return x.components_ <= y.components_;
  }
  friend bool operator>=(Oid const& x, Oid const& y) throw()
  {
    return x.components_ >= y.components_;
  }
  friend Oid operator+(Oid const& a, Oid const& b) throw();

  friend std::ostream& operator<<(std::ostream& s, Oid const& x) throw()
  {
    return s << x.toString();
  }
};


}
}

#endif

