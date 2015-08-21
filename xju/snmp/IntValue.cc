// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/snmp/IntValue.hh"

#include "xju/format.hh"
#include "xju/snmp/encodeInt.hh"
#include "xju/snmp/encodedLengthOfValue.hh"

namespace xju
{
namespace snmp
{

IntValue::IntValue(int64_t const& val) throw():
    Value(encodedLengthOfValue(val)),
    val_(val)
{
}

std::vector<uint8_t>::iterator IntValue::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  return encodeInt(begin,0x02,val_);
}
std::string IntValue::str() const throw()
{
  return xju::format::int_(val_);
}


}
}

