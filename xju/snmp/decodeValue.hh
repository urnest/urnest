// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_DECODEVALUE_H
#define XJU_SNMP_DECODEVALUE_H

#include <utility>
#include "xju/snmp/Value.hh"
#include "xju/snmp/DecodeIterator.hh"
#include <memory>
#include "xju/Exception.hh"

namespace xju
{
namespace snmp
{
std::pair<std::shared_ptr<Value const>, DecodeIterator> decodeValue(
  DecodeIterator const i) throw(xju::Exception);


}
}

#endif

