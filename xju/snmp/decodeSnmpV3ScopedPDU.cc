// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/decodeSnmpV3ScopedPDU.hh>
#include <xju/snmp/SnmpV3ScopedPDU.hh>
#include <vector>
#include <cinttypes>
#include <string>
#include <xju/snmp/decodeSequenceTypeAndLength.hh>
#include <sstream>
#include <xju/Exception.hh>
#include <xju/snmp/decodeStringValue.hh>
#include <xju/format.hh>
#include <xju/snmp/formatLength.hh>
#include <xju/snmp/extractRemainder.hh>
#include <xju/snmp/ContextEngineID.hh>
#include <xju/snmp/ContextName.hh>
#include <xju/snmp/showFirstBytes.hh>
#include <xju/snmp/decodePDU.hh>

namespace xju
{
namespace snmp
{
SnmpV3ScopedPDU decodeSnmpV3ScopedPDU(
    SnmpV3ScopedPduData const& data) /*throw(
    // malformed
    xju::Exception)*/
{
  std::vector<std::string> ok;
  try {
    DecodeIterator const start(data._);
    auto const s1(decodeSequenceTypeAndLength(start));
    if (s1.first.first!=0x30) {
      std::ostringstream s;
      s << "expected sequence type byte 0x30, got " 
        << xju::format::hex(s1.first.first)
        << " at offset 0";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    try {
      if (s1.first.second.valid() && s1.second.remaining() != s1.first.second.value()) {
        std::ostringstream s;
        s << "sequence length " << s1.first.second.value()
          << " does not match contained data length (" << s1.second.remaining() << ")";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      auto contextEngineID(decodeStringValue(s1.second)); try {
        auto contextName(decodeStringValue(contextEngineID.second)); try{
          auto pdu(decodePDU(contextName.second));
          if (!pdu.second.atEnd()){
            std::ostringstream s;
            s << "left over data data " << showFirstBytes(128, extractRemainder(pdu.second))
              << " having decoded pdu " << pdu.first;
            throw xju::Exception(s.str(),XJU_TRACED);
          }
          return SnmpV3ScopedPDU(ContextEngineID(contextEngineID.first),
                                 ContextName(
                                   std::string(contextName.first.begin(),contextName.first.end())),
                                 std::move(pdu.first));
        }
        catch(xju::Exception const& e) {
          std::ostringstream s;
          s << "context name at " << contextEngineID.second;
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
    s << "decode snmp v3 scoped pdu from " << data._.size() << " bytes of data";
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

