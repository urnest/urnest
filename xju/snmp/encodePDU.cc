// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/encodePDU.hh>
#include <memory>
#include <xju/snmp/makePDUSequence.hh>
#include <xju/assert.hh>
#include <xju/snmp/Sequence.hh>
#include <xju/snmp/IntValue.hh>
#include <xju/snmp/StringValue.hh>
#include <xju/snmp/PDU.hh>

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
  uint8_t pduType // 0xA2 snmp v1 get etc
  ) throw()
{
  typedef std::shared_ptr<Value const> vp;

  Sequence s({
      vp(new IntValue(1)), //SNMP V2c
      vp(new StringValue(std::vector<uint8_t>(community._.begin(),
                                              community._.end()))),
      makePDUSequence(PDU(requestId,error,errorIndex,vars,pduType))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

}
}

