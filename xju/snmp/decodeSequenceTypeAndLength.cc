// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "decodeSequenceTypeAndLength.hh"

#include "xju/snmp/decodeLength.hh"
#include <utility>
#include <sstream>

namespace xju
{
namespace snmp
{

// result.first.first is sequence type
// result.first.second is length, if not valid, length is X.690 indefinite form
// result.second is just after decoded data
std::pair<std::pair<uint8_t,xju::Optional<size_t> >, DecodeIterator>
decodeSequenceTypeAndLength(DecodeIterator const at) /*throw(
  xju::Exception)*/
{
  try {
    uint8_t const sequenceType=*at;
    auto length=decodeLength(at+1);
    return std::make_pair(std::make_pair(sequenceType,length.first),
                          length.second);
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "decode sequence type and length at " << at;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}


}
}

