// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_IPV4ADDRESS_H
#define XJU_SNMP_IPV4ADDRESS_H

#include <stdint.h>
#include <vector>
#include <iostream>

namespace xju
{
namespace snmp
{

struct IPv4Address
{
  // octets in host byte-order
  explicit IPv4Address(uint32_t octets) throw():
      _(octets)
  {
  }
  //octets with octets[0] msb
  //pre: octets.size()==4
  explicit IPv4Address(std::vector<uint8_t> const& octets) throw():
      _((octets[0]<<24)|(octets[1]<<16)|(octets[2]<<8)|(octets[3]))
  {
  }
  
  uint32_t _;
  
  // i'th octet, i=0 gets msb
  // pre: 0<=i<=3
  uint8_t operator[](int i) const throw()
  {
    return (_ >> (8*(3-i)))&0xff;
  }

  friend std::ostream& operator<<(std::ostream& s, IPv4Address const& x) 
    throw();
  
  friend bool operator<(IPv4Address const& x, IPv4Address const& y) throw()
  {
    return x._ < y._;
  }
  friend bool operator>(IPv4Address const& x, IPv4Address const& y) throw()
  {
    return y < x;
  }
  friend bool operator!=(IPv4Address const& x, IPv4Address const& y) throw()
  {
    return y < x || x < y;
  }
  friend bool operator==(IPv4Address const& x, IPv4Address const& y) throw()
  {
    return !(y!=x);
  }
  friend bool operator<=(IPv4Address const& x, IPv4Address const& y) throw()
  {
    return !(x>y);
  }
  friend bool operator>=(IPv4Address const& x, IPv4Address const& y) throw()
  {
    return !(x<y);
  }
};


}
}

#endif

