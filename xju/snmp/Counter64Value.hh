// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_COUNTER64VALUE_H
#define XJU_SNMP_COUNTER64VALUE_H

#include "xju/snmp/Value.hh"
#include <stdint.h>

namespace xju
{
namespace snmp
{

class Counter64Value : public Value
{
public:
  explicit Counter64Value(uint64_t val) throw();
  uint64_t val_;

  // Value::
  int intValue() const throw(xju::Exception) override;
  unsigned int uintValue() const throw(xju::Exception) override;
  long longValue() const throw(xju::Exception) override;
  unsigned long ulongValue() const throw(xju::Exception) override;

  // Value::
  virtual std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw();

  // Value::
  virtual std::string str() const throw();

  // Value::
  // pre: y is a Counter64Value
  virtual bool less(Value const& y) const throw() override;

  friend bool operator<(Counter64Value const& x, Counter64Value const& y) throw()
  {
    return x.val_<y.val_;
  }
  friend bool operator>(Counter64Value const& x, Counter64Value const& y) throw()
  {
    return x.val_>y.val_;
  }
  friend bool operator==(Counter64Value const& x, Counter64Value const& y) throw()
  {
    return x.val_==y.val_;
  }
  friend bool operator!=(Counter64Value const& x, Counter64Value const& y) throw()
  {
    return x.val_!=y.val_;
  }
  friend bool operator<=(Counter64Value const& x, Counter64Value const& y) throw()
  {
    return x.val_<=y.val_;
  }
  friend bool operator>=(Counter64Value const& x, Counter64Value const& y) throw()
  {
    return x.val_>=y.val_;
  }
};



}
}

#endif

