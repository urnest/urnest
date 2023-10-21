// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "decodeSnmpV3Message.hh"
#include <string>
#include "xju/Optional.hh"
#include "xju/format.hh"
#include "xju/snmp/DecodeIterator.hh"
#include "xju/snmp/decodeSequenceTypeAndLength.hh"
#include "xju/snmp/decodeIntValue.hh"
#include "xju/snmp/decodeStringValue.hh"
#include "xju/snmp/decodeValue.hh"
#include <algorithm>
#include <iterator>
#include <limits.h>
#include "xju/functional.hh"

namespace xju
{
namespace snmp
{

namespace
{
std::string formatLength(xju::Optional<size_t> const& length) throw()
{
  std::ostringstream s;
  s << (length.valid()?xju::format::int_(length.value())+" bytes":
        std::string("indefinite"));
  return s.str();
}

}
SnmpV3Message decodeSnmpV3Message(
  std::vector<uint8_t> const& data) /*throw(
    SnmpVersionMismatch,
    RequestTypeMismatch,
    xju::Exception)*/
{
  std::vector<std::string> ok;
  try {
    DecodeIterator const start(data);
    auto const s1(decodeSequenceTypeAndLength(start));
    if (s1.first.first!=0x30) {
      std::ostringstream s;
      s << "expected sequence type byte 0x30, got " 
        << xju::format::hex(s1.first.first)
        << " at offset 0";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    try {
      auto const snmpVersion(decodeIntValue(s1.second));
      if (snmpVersion.first!=3) {
        SnmpVersionMismatch e(snmpVersion.first,3,XJU_TRACED);
        std::ostringstream s;
        s << "decode at " << s1.second;
        e.addContext(s.str(),XJU_TRACED);
        throw e;
      }
      try {
        auto const headerDataSequence(decodeSequenceTypeAndLength(snmpVersion.second)); 
        if (headerDataSequence.first.first!=0x30) {
          std::ostringstream s;
          s << "expected sequence type byte 0x30, got " 
            << xju::format::hex(s1.first.first)
            << " at " << snmpVersion.second;
          throw xju::Exception(s.str(), XJU_TRACED);
        }
        try {
          auto const id(decodeIntValue(headerDataSequence.second));
          if (id.first > UINT32_MAX){
            std::ostringstream s;
            s << "expected message id < 2147483648, got " << id.first;
            throw xju::Exception(s.str(),XJU_TRACED);
          }
          try {
            auto const maxSize(decodeIntValue(id.second));
            if (maxSize.first < 484){
              std::ostringstream s;
              s << "expected max size >= 484, got " << maxSize.first;
              throw xju::Exception(s.str(),XJU_TRACED);
            }
            if (maxSize.first > UINT32_MAX){
              std::ostringstream s;
              s << "expected max size < 2147483648, got " << maxSize.first;
              throw xju::Exception(s.str(),XJU_TRACED);
            }
            try {
              auto const msgFlags(decodeStringValue(maxSize.second));
              if (msgFlags.size() != 1){
                std::ostringstream s;
                s << "expected 1 byte message flags, got " << msgFlags.size() << " bytes (hex) "
                  << xju::format::join(msgFlags.begin(),msgFlags.end,xju::format::Hex(""),",");
                throw xju::Exception(s.str(),XJU_TRACED);
              }
              try {
                auto const securityModel(decodeIntValue(msgFlags.second));
                if (securityModel.first > UINT32_MAX){
                  std::ostringstream s;
                  s << "expected max size < 2147483648, got " << securityModel.first;
                  throw xju::Exception(s.str(),XJU_TRACED);
                }
                try{
                  auto const securityParameters(decodeStringValue(securityModel.second));
                  try{
                    auto const scopedPduData(decodeSequenceTypeAndLength(securityParameters.second));
                    if (scopedPduData.first.first == 0x30) // sequence -> ScopedPDU (unencrypted)
                    {
                      SnmpV3Message::Flags flags(msgFlags.first);
                      if (!flags.priv()){
                        std::ostringstream s;
                        s << "unexpected msg privacy flag with unencrypted ScopedPDU";
                        throw xju::Exception(s.str(),XJU_TRACED);
                      }
                    }
                    try{
                      std::vector<uint8_t> scopedPDU;
                      scopedPDU.resize(scopedPduData.first.second);
                      DecodeIterator at(scopedPduData.second);
                      for(auto to(scopedPDU.begin()); to != scopedPDU.end(); ++to){
                        *to = *at++;
                      }
                      return std::make_pair(
                        SnmpV3Message(
                          ID(msgID.first),
                          maxSize.first,
                          Flags(msgFlags.first),
                          SecurityModel(securityModel.first),
                          std::move(securityParameters.first),
                          std::move(scopedPduData.first)),
                        at);
                    }
                    catch(xju::Exception const& e){
                      std::ostringstream s;
                      s << "scoped pdu type " << xju::format::hex(scopedPduData.first.first)
                        << " and length " << scopedPduData.first.second
                        << " at " << securityParameters.second;
                      ok.push_back(s.st());
                      throw;
                    }
                  }
                  catch(xju::Exception& e) {
                    std::ostringstream s;
                    s << "security parameters (hex) "
                      << xju::format::join(securityParameters.first.begin(),
                                           securityParameters.first.end(),
                                           xju::format::Hex(""),
                                           ",")
                      << " at " << securityModel.second;
                    ok.push_back(s.str());
                    throw;
                  }
                }
                catch(xju::Exception& e) {
                  std::ostringstream s;
                  s << "security model " << security.first
                    << " at " << msgFlags.second;
                  ok.push_back(s.str());
                  throw;
                }
              }
              catch(xju::Exception const& e) {
                std::ostringstream s;
                s << "message flags "
                  << xju::format::join(msgFlags.first.begin(),
                                       msgFlags.first.end(),
                                       xju::format::Hex(""),
                                       ",")
                  << " at " << maxSize.second;
                ok.push_back(s.str());
                throw;
              }
            }
            catch(xju::Exception const& e) {
              std::ostringstream s;
              s << "max size " << maxSize.first << " at " << id.second;
              ok.push_back(s.str());
              throw;
            }
          }
          catch(xju::Exception const& e) {
            std::ostringstream s;
            s << "request id " << id.first
              << " at " << headerDataSequence.second;
            ok.push_back(s.str());
            throw;
          }
        }
        catch(xju::Exception const& e) {
          std::ostringstream s;
          s << "header data sequence type " << xju::format::hex(headerDataSequence.first.first)
            << " and length " 
            << formatLength(headerDataSequence.first.second)
            << " at " << snmpVersion.second;
          ok.push_back(s.str());
          throw;
        }
      }
      catch(xju::Exception const& e) {
        std::ostringstream s;
        s << "snmp version 2c at " << s1.second;
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
    s << "decode snmp v3 message from " 
      << data.size() << " bytes of data";
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
