// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "decodeOidComponent.hh"
#include <utility>
#include <sstream>

namespace xju
{
namespace snmp
{

std::pair<uint32_t,DecodeIterator> decodeOidComponent(
  DecodeIterator const at) throw(xju::Exception)
{
  try {
    uint32_t result(0);
    DecodeIterator i(at);
    while((*i) & 0x80) {
      result|=(*i++)&(~0x80);
      result<<=7;
    }
    result|=(*i++);
    return std::pair<uint32_t,DecodeIterator>(result,i);
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "decode oid copmonent at " << at;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}


}
}

