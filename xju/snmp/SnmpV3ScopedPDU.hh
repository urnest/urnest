// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <xju/snmp/ContextEngineID.hh>
#include <xju/snmp/ContextName.hh>
#include <vector>
#include <cinttypes>

namespace xju
{
namespace snmp
{

class SnmpV3ScopedPDU
{
public:
  SnmpV3ScopedPDU(
    ContextEngineID contextEngineID,
    ContextName contextName,
    std::vector<uint8_t> encodedPDU) noexcept:
      contextEngineID_(std::move(contextEngineID)),
      contextName_(std::move(contextName)),
      encodedPDU_(std::move(encodedPDU))
  {
  }
  ContextEngineID contextEngineID_;
  ContextName contextName_;
  std::vector<uint8_t> encodedPDU_;
};


}
}
