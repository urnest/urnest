// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "decodeIntValue.hh"
#include <sstream>
#include <xju/format.hh>
#include "xju/snmp/decodeLength.hh"

namespace xju
{
namespace snmp
{

std::pair<uint64_t,DecodeIterator> decodeIntValue(DecodeIterator const at)
  throw(xju::Exception)
{
  try {
    if ((*at) != 0x02) {
      std::ostringstream s;
      s << "type is " << xju::format::hex(*at) << " not 0x02";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    auto const length(decodeLength(at+1));
    if (!length.first.valid()) {
      std::ostringstream s;
      s << "integers with indefinite length are illegal";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    const unsigned int byteCount(length.first.value());
    if (byteCount>8) {
      std::ostringstream s;
      s << "can only handle 8-byte integers, not " << byteCount;
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    uint64_t result=0;
    if ((*length.second)&0x80) {
      // negative number
      result=~result;
    }
    unsigned int shift=8*(byteCount-1);
    auto i=length.second;
    for(; i!=length.second+byteCount; ++i) {
      result&=~(((uint64_t)0xff)<<shift);
      result|=((uint64_t)*i)<<shift;
      shift-=8;
    }
    return std::make_pair(result,i);
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "decode integer at " << at;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}


}
}

