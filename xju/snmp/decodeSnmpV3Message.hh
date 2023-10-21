// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_DECODESNMPV3MESSAGE_H
#define XJU_SNMP_DECODESNMPV3MESSAGE_H

#include <xju/snmp/SnmpV3Message.hh>
#include <vector>
#include <cinttypes>

namespace xju
{
namespace snmp
{

SnmpV3Message decodeSnmpV3Message(
    std::vector<uint8_t> const& data) /*throw(
    // malformed
    xju::Exception)*/;

}
}

#endif
