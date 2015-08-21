// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_VALUE_H
#define XJU_SNMP_VALUE_H

#include <string>
#include "xju/Exception.hh"
#include <vector>
#include <stdint.h>
#include <iostream>

namespace xju
{
namespace snmp
{

class Value
{
public:
  virtual ~Value() throw(){}

  explicit Value(size_t encodedLength) throw():
      encodedLength_(encodedLength)
  {
  }
  
  // encodeTo(x)-x
  size_t const encodedLength_;
  
  // convenience functions that do type and range checking
  operator std::string() const throw(xju::Exception);
  operator int() const throw(xju::Exception);
  operator unsigned int() const throw(xju::Exception);
  operator long() const throw(xju::Exception);
  operator unsigned long() const throw(xju::Exception);

  // return length of encoded value
  // ie return encodeTo(x)-x
  size_t encodedLength() const throw()
  {
    return encodedLength_;
  }

  // encode at begin, returning end of encoding
  virtual std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw()=0;

  // human readable
  virtual std::string str() const throw()=0;
  
  friend std::ostream& operator<<(std::ostream& s, Value const& x) throw()
  {
    return s << x.str();
  }
};


}
}

#endif

