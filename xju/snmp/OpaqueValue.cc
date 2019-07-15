// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "OpaqueValue.hh"

#include "encodeOctetString.hh"
#include "xju/snmp/encodedLengthOfValue.hh"
#include <xju/format.hh>

namespace xju
{
namespace snmp
{

OpaqueValue::OpaqueValue(std::vector<uint8_t> const& val) throw():
    Value(encodedLengthOfValue(val)),
    val_(val)
{
}

OpaqueValue::OpaqueValue(std::vector<uint8_t> && val) throw():
    Value(encodedLengthOfValue(val)),
    val_(val)
{
}

std::vector<uint8_t>::iterator OpaqueValue::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  return encodeOctetString(begin,0x44,
                           std::vector<uint8_t>(val_.begin(),val_.end()));
}

std::string OpaqueValue::str() const throw()
{
  return xju::format::join(
    val_.begin(),val_.end(),
    [](uint8_t x){
      return xju::format::hex(x);
    },
    std::string(" "));
}


bool OpaqueValue::less(Value const& y) const throw()
{
  return val_ < dynamic_cast<OpaqueValue const&>(y).val_;
}

}
}

