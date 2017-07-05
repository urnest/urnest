// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "IPv4AddressValue.hh"
#include "xju/snmp/encodedLengthOfValue.hh"
#include "xju/snmp/encodeOctetString.hh"
#include "xju/format.hh"

namespace xju
{
namespace snmp
{

IPv4AddressValue::IPv4AddressValue(xju::ip::v4::Address val) throw():
    Value(encodedLengthOfValue(std::vector<uint8_t>({0,0,0,0}))),
  val_(val)
{
}

std::vector<uint8_t>::iterator IPv4AddressValue::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  return encodeOctetString(
    begin,0x40,{
      (uint8_t)((val_.value()>>24) & 0xff),
        (uint8_t)((val_.value()>>16) & 0xff),
        (uint8_t)((val_.value()>> 8) & 0xff),
        (uint8_t)((val_.value()>> 0) & 0xff)});
}

std::string IPv4AddressValue::str() const throw()
{
  return xju::format::str(val_);
}


}
}

