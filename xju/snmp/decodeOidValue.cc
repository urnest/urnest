// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "decodeOidValue.hh"
#include <sstream>
#include <xju/format.hh>
#include "xju/snmp/decodeLength.hh"
#include "xju/snmp/decodeOidComponent.hh"

namespace xju
{
namespace snmp
{
std::pair<Oid,DecodeIterator> decodeOidValue(
  DecodeIterator const at) throw(xju::Exception)
{
  try {
    if ((*at) != 0x06) {
      std::ostringstream s;
      s << "type is " << xju::format::hex(*at) << " not 0x06";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    auto const length(decodeLength(at+1));
    bool const lengthIndefinite(!length.first.valid());
    auto atEnd=[&](DecodeIterator const& i) throw() {
      if (lengthIndefinite) {
        return (*i)==0 && (*i+1)==0;
      }
      else
      {
        return i==length.second+length.first.value();
      }
    };
    DecodeIterator i(length.second);
    std::vector<uint32_t> components;
    if ((*i) != 0x2b) {
      std::ostringstream s;
      s << "oid starts with " << xju::format::hex(*i) << " not 0x2b";
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    components.push_back(1);
    components.push_back(3);
    ++i;
    while(!atEnd(i)) {
      try {
        auto c(decodeOidComponent(i));
        components.push_back(c.first);
        i=c.second;
      }
      catch(xju::Exception& e) {
        std::ostringstream s;
        s << "decode next component having decoded "
          << xju::format::join(components.begin(),
                               components.end(),
                               [](uint32_t x) { return xju::format::int_(x); },
                               ", ");
        e.addContext(s.str(), XJU_TRACED);
        throw;
      }
    }
    if (lengthIndefinite) {
      i=i+2; // skip 2x zero bytes
    }
    return std::make_pair(Oid(components),i);
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "decode oid at " << at;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}


}
}

