// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_NULLVALUE_H
#define XJU_SNMP_NULLVALUE_H

#include "xju/snmp/Value.hh"

namespace xju
{
namespace snmp
{

class NullValue : public Value
{
public:
  ~NullValue() throw(){}
  
  explicit NullValue() throw():
      Value(2)
  {
  }

  // Value::
  std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw() override;

  // Value::
  virtual std::string str() const throw() override
  {
    return "null";
  }
};


}
}

#endif

