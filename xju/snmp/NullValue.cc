// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "NullValue.hh"
#include "xju/snmp/encodeLength.hh"

namespace xju
{
namespace snmp
{

std::vector<uint8_t>::iterator NullValue::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  auto at(begin);
  *at++=0x05;
  at=encodeLength(at,0);
  return at;
}

bool NullValue::less(Value const& y) const throw()
{
  dynamic_cast<NullValue const&>(y);
  return false;
}

}
}

