// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "decodeIPv4AddressValue.hh"
#include <utility>
#include "xju/snmp/DecodeIterator.hh"
#include <xju/format.hh>
#include <sstream>
#include "xju/snmp/decodeLength.hh"

namespace xju
{
namespace snmp
{
std::pair<xju::ip::v4::Address,DecodeIterator> decodeIPv4AddressValue(
  DecodeIterator const at) throw(xju::Exception)
{
  try {
    if ((*at) != 0x40) {
      std::ostringstream s;
      s << "type is " << xju::format::hex(*at) << " not 0x40";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    auto const length(decodeLength(at+1));
    if (!length.first.valid()) {
      std::ostringstream s;
      s << "IPv4Address value with indefinite length are illegal";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    if (length.first.value()!=4){
      std::ostringstream s;
      s << "IPv4Address length " << length.first.value() << " is not 4";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    std::vector<uint8_t> s;
    for(DecodeIterator i=length.second;
        i!=length.second+length.first.value();
        ++i) {
      s.push_back(*i);
    }
    return std::make_pair(
      xju::ip::v4::Address(
        (((uint32_t)s[0])<<24)+
        (((uint32_t)s[1])<<16)+
        (((uint32_t)s[2])<<8)+
        (((uint32_t)s[3])<<0)),
      length.second+length.first.value());
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "decode string at " << at;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}


}
}

