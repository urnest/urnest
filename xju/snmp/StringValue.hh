// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_STRINGVALUE_H
#define XJU_SNMP_STRINGVALUE_H

#include "Value.hh"
#include <string>

namespace xju
{
namespace snmp
{

class StringValue : public Value
{
public:
  ~StringValue() throw(){}

  explicit StringValue(std::vector<uint8_t> const& val) throw();
  explicit StringValue(std::vector<uint8_t> && val) throw();

  // copy string byte values verbatim
  // post: val_.size()==val.size()
  explicit StringValue(std::string const& val) throw();
  
  std::vector<uint8_t> const val_;

  // Value::
  std::vector<uint8_t>const& stringValue() const throw(xju::Exception) override
  {
    return val_;
  }

  // Value::
  std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw() override;

  // Value::
  virtual std::string str() const throw() override;

  // Value::
  // pre: y is a StringValue
  virtual bool less(Value const& y) const throw() override;

  friend bool operator<(StringValue const& x, StringValue const& y) throw()
  {
    return x.val_<y.val_;
  }
  friend bool operator>(StringValue const& x, StringValue const& y) throw()
  {
    return x.val_>y.val_;
  }
  friend bool operator==(StringValue const& x, StringValue const& y) throw()
  {
    return x.val_==y.val_;
  }
  friend bool operator!=(StringValue const& x, StringValue const& y) throw()
  {
    return x.val_!=y.val_;
  }
  friend bool operator<=(StringValue const& x, StringValue const& y) throw()
  {
    return x.val_<=y.val_;
  }
  friend bool operator>=(StringValue const& x, StringValue const& y) throw()
  {
    return x.val_>=y.val_;
  }
  
};


}
}

#endif

