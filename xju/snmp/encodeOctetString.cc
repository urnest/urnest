// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "encodeOctetString.hh"
#include <vector>
#include <stdint.h>
#include "encodeLength.hh"
#include <algorithm>
#include "xju/snmp/encodeLength.hh"

namespace xju
{
namespace snmp
{

std::vector<uint8_t>::iterator encodeOctetString(
  std::vector<uint8_t>::iterator const to,
  uint8_t const asn1Type,
  std::vector<uint8_t> const& val) throw()
{
  // X.690 octetstring using primitive encoding
  auto at(to);
  *at++=asn1Type;
  at=encodeLength(at,val.size());
  std::copy(val.begin(),val.end(),at);
  return at+val.size();
}

  
}
}

