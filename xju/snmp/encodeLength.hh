// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_ENCODELENGTH_H
#define XJU_SNMP_ENCODELENGTH_H

#include <vector>
#include <stdint.h>
#include <unistd.h>

namespace xju
{
namespace snmp
{

// encode x as a BER encoded length, as bytes placed onwards from *begin
// pre: begin... has enough room for encoded form of length
//
std::vector<uint8_t>::iterator encodeLength(
  std::vector<uint8_t>::iterator const begin,
  size_t const x) throw();

}
}

#endif
