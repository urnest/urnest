// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_OPAQUEVALUE_H
#define XJU_SNMP_OPAQUEVALUE_H

#include "Value.hh"
#include <string>

namespace xju
{
namespace snmp
{

class OpaqueValue : public Value
{
public:
  ~OpaqueValue() throw(){}

  explicit OpaqueValue(std::vector<uint8_t> const& val) throw();
  explicit OpaqueValue(std::vector<uint8_t> && val) throw();

  std::vector<uint8_t> const val_;

  // Value::
  std::vector<uint8_t>const& opaqueValue() const /*throw(xju::Exception)*/ override
  {
    return val_;
  }

  // Value::
  std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw() override;

  // Value::
  std::string str() const throw() override;

  // Value::
  // pre: y is a OpaqueValue
  virtual bool less(Value const& y) const throw() override;

  friend bool operator<(OpaqueValue const& x, OpaqueValue const& y) throw()
  {
    return x.val_<y.val_;
  }
  friend bool operator>(OpaqueValue const& x, OpaqueValue const& y) throw()
  {
    return x.val_>y.val_;
  }
  friend bool operator==(OpaqueValue const& x, OpaqueValue const& y) throw()
  {
    return x.val_==y.val_;
  }
  friend bool operator!=(OpaqueValue const& x, OpaqueValue const& y) throw()
  {
    return x.val_!=y.val_;
  }
  friend bool operator<=(OpaqueValue const& x, OpaqueValue const& y) throw()
  {
    return x.val_<=y.val_;
  }
  friend bool operator>=(OpaqueValue const& x, OpaqueValue const& y) throw()
  {
    return x.val_>=y.val_;
  }
  
};


}
}

#endif

