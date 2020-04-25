// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/oidDataLength.hh>

#include <unistd.h>
#include <algorithm>
#include "xju/snmp/encodedLengthOfOidComponent.hh"

namespace xju
{
namespace snmp
{
size_t oidDataLength(Oid const& oid) throw()
{
  uint64_t result(1U);
  for(auto i=oid.components().begin()+2; i!=oid.components().end();++i){
    result=result+encodedLengthOfOidComponent(*i);
  }
  return result;
}

}
}

