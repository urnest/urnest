// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "OidValue.hh"
#include "xju/snmp/encodedLengthOfValue.hh"
#include "xju/snmp/encodeLength.hh"
#include "xju/countSignificantBits.hh"

namespace xju
{
namespace snmp
{

namespace
{
uint64_t encodedLengthOfOidComponent(uint32_t const c) throw()
{
  int bits=xju::countSignificantBits(c);
  // encoded in groups of 7 bits, but c==0 uses one byte
  // bits     length
  // 0        1
  // <8       1
  // <15      2
  if(c==0) {
    return 1;
  }
  return (bits-1)/7+1;
}

size_t oidDataLength(Oid const& oid) throw()
{
  return std::accumulate(
    oid.components().begin()+2,
    oid.components().end(),
    uint64_t{1U},/*for 1.3 encoded as 0x2B*/
    [](uint64_t t, uint32_t c)
    {
      return t+encodedLengthOfOidComponent(c);
    });
}
std::vector<uint8_t>::iterator encodeOidComponent(
  std::vector<uint8_t>::iterator const begin,
  uint32_t c) throw()
{
  auto at(begin);
  int byteCount=encodedLengthOfOidComponent(c);
  at+=byteCount;
  std::vector<uint8_t>::reverse_iterator r(at);
  for(int i=0; i != byteCount; ++i) {
    *r++=0x80|(c&0x7f);
    c=c>>7;
  }
  *(at-1) &= ~0x80; // clear top bit of last byte
  return at;
}

}

OidValue::OidValue(Oid const& val) throw():
    Value(encodedLengthOfValue(val)),
    val_(val)
{
}

std::vector<uint8_t>::iterator OidValue::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  auto at(begin);
  *at++=0x06;
  at=encodeLength(at,oidDataLength(val_));
  *at++=0x2b; //1.3
  for(auto i=val_.components().begin()+2;
      i!=val_.components().end();
      ++i) {
    at=encodeOidComponent(at,*i);
  }
  return at;
}


}
}

