// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/decodeStringValue.hh>

#include <utility>
#include <xju/snmp/DecodeIterator.hh>
#include <xju/format.hh>
#include <sstream>
#include <xju/snmp/decodeLength.hh>

namespace xju
{
namespace snmp
{
std::pair<std::vector<uint8_t>,DecodeIterator> decodeStringValue(
  DecodeIterator const at,
  uint8_t const type_) /*throw(xju::Exception)*/
{
  try {
    if ((*at) != type_) {
      std::ostringstream s;
      s << "type is " << xju::format::hex(*at) << " not "
        << xju::format::hex(type_);
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    auto const length(decodeLength(at+1));
    if (!length.first.valid()) {
      std::ostringstream s;
      s << "strings with indefinite length are illegal";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    std::vector<uint8_t> s;
    for(DecodeIterator i=length.second;
        i!=length.second+length.first.value();
        ++i) {
      s.push_back(*i);
    }
    return std::make_pair(s,length.second+length.first.value());
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

