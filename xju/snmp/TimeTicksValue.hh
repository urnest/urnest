// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_TIMETICKSVALUE_H
#define XJU_SNMP_TIMETICKSVALUE_H

#include "xju/snmp/Value.hh"
#include <chrono>

namespace xju
{
namespace snmp
{

class TimeTicksValue : public Value
{
public:
  explicit TimeTicksValue(std::chrono::milliseconds val) throw();
  std::chrono::milliseconds val_;

  // Value::
  virtual std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw();

  // Value::
  virtual std::string str() const throw();

  virtual std::chrono::milliseconds timeTicksValue() const throw(
    xju::Exception) {
    return val_;
  }

  friend bool operator<(TimeTicksValue const& x, TimeTicksValue const& y) throw()
  {
    return x.val_<y.val_;
  }
  friend bool operator>(TimeTicksValue const& x, TimeTicksValue const& y) throw()
  {
    return x.val_>y.val_;
  }
  friend bool operator==(TimeTicksValue const& x, TimeTicksValue const& y) throw()
  {
    return x.val_==y.val_;
  }
  friend bool operator!=(TimeTicksValue const& x, TimeTicksValue const& y) throw()
  {
    return x.val_!=y.val_;
  }
  friend bool operator<=(TimeTicksValue const& x, TimeTicksValue const& y) throw()
  {
    return x.val_<=y.val_;
  }
  friend bool operator>=(TimeTicksValue const& x, TimeTicksValue const& y) throw()
  {
    return x.val_>=y.val_;
  }
};



}
}

#endif

