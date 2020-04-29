// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_DECODESNMPV1GETNEXTREQUEST_H
#define XJU_SNMP_DECODESNMPV1GETNEXTREQUEST_H

#include "xju/snmp/SnmpV1GetNextRequest.hh"
#include <vector>
#include <stdint.h>
#include "xju/Exception.hh"
#include "xju/snmp/RequestTypeMismatch.hh"
#include "xju/snmp/SnmpVersionMismatch.hh"

namespace xju
{
namespace snmp
{
// returns the request
SnmpV1GetNextRequest decodeSnmpV1GetNextRequest(
  std::vector<uint8_t> const& data) /*throw(
    // not snmp v1
    SnmpVersionMismatch,
    // not a get next request
    RequestTypeMismatch,
    // malformed
    xju::Exception)*/;

}
}

#endif

