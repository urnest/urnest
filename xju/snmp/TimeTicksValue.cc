// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "TimeTicksValue.hh"

#include "xju/snmp/encodedLengthOfValue.hh"
#include "xju/snmp/encodeInt.hh"
#include "xju/format.hh"

namespace xju
{
namespace snmp
{

TimeTicksValue::TimeTicksValue(xju::MicroSeconds val) throw():
    Value(encodedLengthOfValue(val.value()/10000)),
    val_(val) {
  }

std::vector<uint8_t>::iterator TimeTicksValue::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  return encodeInt(begin,0x43,val_.value()/10000);
}
std::string TimeTicksValue::str() const throw()
{
  return xju::format::str(val_);
}


}
}

