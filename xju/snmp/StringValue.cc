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

namespace xju
{
namespace snmp
{

StringValue::StringValue(std::string const& val) throw():
    Value(encodedLengthOfValue(val)),
    val_(val)
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
  return xju::format::quote(xju::format::cEscapeString(val_));
}


}
}

