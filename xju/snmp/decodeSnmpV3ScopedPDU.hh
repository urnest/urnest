// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_DECODESNMPV3SCOPEDPDU_H
#define XJU_SNMP_DECODESNMPV3SCOPEDPDU_H

#include <xju/snmp/SnmpV3ScopedPDU.hh>
#include <xju/snmp/SnmpV3ScopedPduData.hh>

namespace xju
{
namespace snmp
{

SnmpV3ScopedPDU decodeSnmpV3ScopedPDU(
    SnmpV3ScopedPduData const& data) /*throw(
    // malformed
    xju::Exception)*/;

}
}

#endif
