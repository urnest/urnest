// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/decodeSnmpV3ScopedPDU.h>
#include <xju/snmp/SnmpV3ScopedPDU.hh>
#include <vector>
#include <cinttypes>
#include <string>
#include <xju/snmp/decodeSequenceTypeAndLength.hh>
#include <sstream>
#include <xju/Exception.hh>
#include <xju/snmp/decodeStringValue.hh>

namespace xju
{
namespace snmp
{
SnmpV3ScopedPDU decodeSnmpV3ScopedPDU(
    std::vector<uint8_t> const& data) /*throw(
    // malformed
    xju::Exception)*/
{
  std::vector<std::string> ok;
  try {
    auto const s1(decodeSequenceTypeAndLength(start));
    if (s1.first.first!=0x30) {
      std::ostringstream s;
      s << "expected sequence type byte 0x30, got " 
        << xju::format::hex(s1.first.first)
        << " at offset 0";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    try {
      auto conextEngineID(decodeStringValue(s1.second));
      try {
        auto contextName(decodeStringValue(contextEngineID.second)); try {
          auto const encodedPDU(extractValue(contextName.second));
          return SnmpV3ScopedPDU(std::move(contextEngineID.first),
                                 std::move(contextName.fist),
                                 std::move(encodedPDU.first));
        }
        catch(xju::Exception const& e) {
          std::ostringstream s;
          s << "context name "
            << xju::format::quote(std::string(contextName.first.begin(),
                                              contextName.first.end()))
            << " at " << contextEngineID.second;
          ok.push_back(s.str());
          throw;
        }
      }
      catch(xju::Exception const& e) {
        std::ostringstream s;
        s << "snmp engine id at " << s1.second;
        ok.push_back(s.str());
        throw;
      }
    }
    catch(xju::Exception const& e) {
      std::ostringstream s;
      s << "sequence type 0x30 and length " 
        << formatLength(s1.first.second);
      ok.push_back(s.str());
      throw;
    }
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "decode snmp v3 scoped pdu from " << data.size() << " bytes of data";
    if (ok.size()) {
      s << " having successfully decoded "
        << xju::format::join(ok.rbegin(),ok.rend(),", ");
    }
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

}
}

