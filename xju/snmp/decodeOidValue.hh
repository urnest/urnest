// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_DECODEOIDVALUE_H
#define XJU_SNMP_DECODEOIDVALUE_H

#include <utility>
#include "xju/snmp/DecodeIterator.hh"
#include "xju/Exception.hh"
#include "xju/snmp/SnmpV1Table.hh"

namespace xju
{
namespace snmp
{
std::pair<Oid,DecodeIterator> decodeOidValue(
  DecodeIterator const at) /*throw(xju::Exception)*/;

}
}

#endif

