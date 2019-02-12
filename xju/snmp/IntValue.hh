// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_INTVALUE_H
#define XJU_SNMP_INTVALUE_H

#include <xju/snmp/Value.hh>
#include <vector>
#include <stdint.h>
#include <string>

namespace xju
{
namespace snmp
{

class IntValue : public Value
{
public:
  ~IntValue() throw(){}
  
  explicit IntValue(int64_t const& val) throw();
  
  int64_t const val_;

  // Value::
  int intValue() const throw(xju::Exception) override;
  unsigned int uintValue() const throw(xju::Exception) override;
  long longValue() const throw(xju::Exception) override;
  unsigned long ulongValue() const throw(xju::Exception) override;

  // Value::
  std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw() override;

  // Value::
  virtual std::string str() const throw() override;

  friend bool operator<(IntValue const& x, IntValue const& y) throw()
  {
    return x.val_<y.val_;
  }
  friend bool operator>(IntValue const& x, IntValue const& y) throw()
  {
    return x.val_>y.val_;
  }
  friend bool operator==(IntValue const& x, IntValue const& y) throw()
  {
    return x.val_==y.val_;
  }
  friend bool operator!=(IntValue const& x, IntValue const& y) throw()
  {
    return x.val_!=y.val_;
  }
  friend bool operator<=(IntValue const& x, IntValue const& y) throw()
  {
    return x.val_<=y.val_;
  }
  friend bool operator>=(IntValue const& x, IntValue const& y) throw()
  {
    return x.val_>=y.val_;
  }
};


}
}

#endif

