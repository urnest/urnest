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

IPv4AddressValue::IPv4AddressValue(IPv4Address val) throw():
  Value(encodedLengthOfValue(std::string("    "))),
  val_(val)
{
}

std::vector<uint8_t>::iterator IPv4AddressValue::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  return encodeOctetString(
    begin,0x40,{val_[0],val_[1],val_[2],val_[3]});
}

std::string IPv4AddressValue::str() const throw()
{
  return xju::format::str(val_);
}


}
}

