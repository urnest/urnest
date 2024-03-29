// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_DECODEPREENCODEDVALUE_H
#define XJU_SNMP_DECODEPREENCODEDVALUE_H

#include <utility>
#include "xju/snmp/DecodeIterator.hh"
#include "xju/Exception.hh"
#include <cinttypes>

namespace xju
{
namespace snmp
{
std::pair<std::vector<uint8_t>,DecodeIterator> decodePreEncodedValue(
  DecodeIterator const at,
  uint8_t const type_=0x04) /*throw(xju::Exception)*/;

}
}

#endif

