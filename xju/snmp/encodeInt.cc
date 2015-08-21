// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "encodeInt.hh"
#include "xju/snmp/encodeLength.hh"
#include "xju/snmp/intDataLength.hh"

namespace xju
{
namespace snmp
{

std::vector<uint8_t>::iterator encodeInt(
  std::vector<uint8_t>::iterator const to,
  uint8_t const asn1Type,
  int64_t const val) throw()
{
  auto at(to);
  *at++=asn1Type;
  auto const dataLength=intDataLength(val);
  at=encodeLength(at, dataLength);
  for(int i=dataLength; i!=0; --i) {
    uint8_t b=(val >> ((i-1)*8)) & 0xff;
    *at++=b;
  }
  return at;
}


}
}

