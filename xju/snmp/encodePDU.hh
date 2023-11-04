// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
// low level encoding functions, for high level
// function see encode.hh and encodeResponse.hh
//
#ifndef XJU_SNMP_ENCODEPDU_HH
#define XJU_SNMP_ENCODEPDU_HH

#include <vector>
#include <cinttypes>
#include <xju/snmp/Community.hh>
#include <xju/snmp/RequestId.hh>
#include <utility>
#include <xju/snmp/Oid.hh>
#include <memory>
#include <xju/snmp/Value.hh>
#include <xju/snmp/SnmpVar.hh>

namespace xju
{
namespace snmp
{

std::vector<uint8_t> encodePDU(
  Community const& community,
  RequestId requestId,
  uint64_t error,
  uint64_t errorIndex,
  std::vector<SnmpVar> const& vars,
  uint8_t pduType // 0xA0 snmp v2 get etc
  ) throw();

}
}

#endif

