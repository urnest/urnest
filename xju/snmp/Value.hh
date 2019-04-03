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
#include "xju/snmp/Oid.hh"
#include "xju/check_types_related.hh"
#include <typeinfo>

#include <chrono>
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
  virtual std::vector<uint8_t>const& stringValue() const throw(xju::Exception);
  virtual int intValue() const throw(xju::Exception);
  virtual unsigned int uintValue() const throw(xju::Exception);
  virtual long longValue() const throw(xju::Exception);
  virtual unsigned long ulongValue() const throw(xju::Exception);
  virtual Oid oidValue() const throw(xju::Exception);
  virtual std::chrono::milliseconds timeTicksValue() const throw(
    xju::Exception);
  
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

  // pre: typeid(y)==typeid(*this)
  virtual bool less(Value const& y) const throw()=0;
  
  friend std::ostream& operator<<(std::ostream& s, Value const& x) throw()
  {
    return s << x.str();
  }

  friend bool operator<(Value const& x, Value const& y) noexcept
  {
    if (std::string(typeid(x).name())<std::string(typeid(y).name())){
      return true;
    }
    if (std::string(typeid(y).name())<std::string(typeid(x).name())){
      return false;
    }
    return x.less(y);
  }
};
  
}
}

#endif


