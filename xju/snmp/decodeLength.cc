// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "decodeLength.hh"
#include <utility>

namespace xju
{
namespace snmp
{
// result.first is length, if not valid, length is X.690 indefinite form
// result.second is just after decoded data
std::pair<xju::Optional<size_t>,DecodeIterator> decodeLength(
  DecodeIterator const at) throw(xju::Exception)
{
  try {
    if ((*at)<0x80) {
      // X.690 definite short encoding
      return std::make_pair(xju::Optional<size_t>(*at),at+1);
    }
    else if ((*at)==0x80)
    {
      // X.690 indefinite form
      return std::make_pair(xju::Optional<size_t>(),at+1);
    }
    else
    {
      // X.690 definite long form
      uint64_t length=0;
      int const byteCount((*at)&~0x80);
      if (byteCount > 8) {
        std::ostringstream s;
        s << "can only handle 8-byte lengths, not " << byteCount;
        throw xju::Exception(s.str(), XJU_TRACED);
      }
      auto i=at+1;
      unsigned int shift=8*(byteCount-1);
      for(; i!=(at+1+byteCount); ++i) {
        length|=((uint64_t)*i)<<shift;
        shift-=8;
      }
      return std::make_pair(xju::Optional<size_t>(length),i);
    }
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "decode length at " << at;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}


}
}

