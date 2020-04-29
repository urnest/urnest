// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_DECODESEQUENCETYPEANDLENGTH_H
#define XJU_SNMP_DECODESEQUENCETYPEANDLENGTH_H

#include <utility>
#include <stdint.h>
#include "xju/Optional.hh"
#include "xju/snmp/DecodeIterator.hh"
#include "xju/Exception.hh"

namespace xju
{
namespace snmp
{
// result.first.first is sequence type
// result.first.second is length, if not valid, length is X.690 indefinite form
// result.second is just after decoded data
std::pair<std::pair<uint8_t,xju::Optional<size_t> >, DecodeIterator>
decodeSequenceTypeAndLength(DecodeIterator const at) /*throw(
  xju::Exception)*/;

}
}

#endif

