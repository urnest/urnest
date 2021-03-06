// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "StringValue.hh"

#include "encodeOctetString.hh"
#include "xju/snmp/encodedLengthOfValue.hh"
#include <xju/format.hh>

namespace xju
{
namespace snmp
{

StringValue::StringValue(std::vector<uint8_t> const& val) throw():
    Value(encodedLengthOfValue(val)),
    val_(val)
{
}

StringValue::StringValue(std::vector<uint8_t> && val) throw():
    Value(encodedLengthOfValue(val)),
    val_(val)
{
}

StringValue::StringValue(std::string const& val) throw():
    StringValue(std::vector<uint8_t>(val.begin(),val.end()))
{
}

std::vector<uint8_t>::iterator StringValue::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  return encodeOctetString(begin,0x04,
                           std::vector<uint8_t>(val_.begin(),val_.end()));
}
std::string StringValue::str() const throw()
{
  return xju::format::quote(xju::format::cEscapeString(
                              std::string(val_.begin(),val_.end())));
}

bool StringValue::less(Value const& y) const throw()
{
  return val_ < dynamic_cast<StringValue const&>(y).val_;
}

}
}

