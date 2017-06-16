// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_COUNTER32VALUE_H
#define XJU_SNMP_COUNTER32VALUE_H

#include "xju/snmp/Value.hh"
#include <stdint.h>

namespace xju
{
namespace snmp
{

class Counter32Value : public Value
{
public:
  explicit Counter32Value(uint32_t val) throw();
  uint32_t val_;

  // Value::
  int intValue() const throw(xju::Exception) override;
  unsigned int uintValue() const throw() override;
  long longValue() const throw(xju::Exception) override;
  unsigned long ulongValue() const throw() override;

  // Value::
  virtual std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw();

  // Value::
  virtual std::string str() const throw();

  friend bool operator<(Counter32Value const& x, Counter32Value const& y) throw()
  {
    return x.val_<y.val_;
  }
  friend bool operator>(Counter32Value const& x, Counter32Value const& y) throw()
  {
    return x.val_>y.val_;
  }
  friend bool operator==(Counter32Value const& x, Counter32Value const& y) throw()
  {
    return x.val_==y.val_;
  }
  friend bool operator!=(Counter32Value const& x, Counter32Value const& y) throw()
  {
    return x.val_!=y.val_;
  }
  friend bool operator<=(Counter32Value const& x, Counter32Value const& y) throw()
  {
    return x.val_<=y.val_;
  }
  friend bool operator>=(Counter32Value const& x, Counter32Value const& y) throw()
  {
    return x.val_>=y.val_;
  }
};



}
}

#endif

