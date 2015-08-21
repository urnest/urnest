// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_DECODESNMPV1RESPONSE_H
#define XJU_SNMP_DECODESNMPV1RESPONSE_H

#include "xju/snmp/SnmpV1Response.hh"
#include <vector>
#include <stdint.h>
#include "xju/Exception.hh"

namespace xju
{
namespace snmp
{

SnmpV1Response decodeSnmpV1Response(std::vector<uint8_t> const& data) throw(
    // malformed
    xju::Exception);


}
}

#endif

