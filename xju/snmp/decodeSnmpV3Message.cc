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
#include <xju/snmp/formatLength.hh>
#include <xju/snmp/extractRemainder.hh>
#include <xju/snmp/SnmpVersionMismatch.hh>

namespace xju
{
namespace snmp
{

std::pair<SnmpV3Message,
          size_t> // offset of securityParameters in data
decodeSnmpV3Message(
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
              if (msgFlags.first.size() != 1){
                std::ostringstream s;
                s << "expected 1 byte message flags, got " << msgFlags.first.size() << " bytes (hex) "
                  << xju::format::join(msgFlags.first.begin(),msgFlags.first.end(),
                                       xju::format::Hex(""),std::string(","));
                throw xju::Exception(s.str(),XJU_TRACED);
              }
              try {
                auto const securityModel(decodeIntValue(msgFlags.second));
                if (securityModel.first > UINT32_MAX){
                  std::ostringstream s;
                  s << "expected security model < 2147483648, got " << securityModel.first;
                  throw xju::Exception(s.str(),XJU_TRACED);
                }
                try{
                  auto const securityParameters(decodeStringValue(securityModel.second));
                  size_t offsetOfSecurityParametersInData(
                    start.remaining() - securityParameters.second.remaining() - securityParameters.first.size());
                  try{
                    auto const scopedPduData(decodeSequenceTypeAndLength(securityParameters.second));
                    SnmpV3Message::Flags const flags = (SnmpV3Message::Flags)msgFlags.first[0];

                    if ((flags & SnmpV3Message::AUTH_PRIV) != SnmpV3Message::AUTH_PRIV){
                      if (scopedPduData.first.first != 0x30) // sequence -> ScopedPDU (unencrypted)
                      {
                        std::ostringstream s;
                        s << "expected unencrypted ScopedPDU to have type 0x30, got "
                          << xju::format::hex(scopedPduData.first.first);
                        throw xju::Exception(s.str(),XJU_TRACED);
                      }
                    }
                    else
                    {
                      if (scopedPduData.first.first != 0x04) // octet string -> encrypted
                      {
                        std::ostringstream s;
                        s << "expected encrypted ScopedPDU to have type 0x04, got "
                          << xju::format::hex(scopedPduData.first.first);
                        throw xju::Exception(s.str(),XJU_TRACED);
                      }
                    }
                    try{
                      if (scopedPduData.first.second.valid() &&
                          scopedPduData.second.remaining() != scopedPduData.first.second.value()){
                        std::ostringstream s;
                        s << "expected " << scopedPduData.first.second.value()
                          << " bytes of scoped pdu data but have " << scopedPduData.second.remaining();
                        throw xju::Exception(s.str(),XJU_TRACED);
                      }
                      std::vector<uint8_t> scopedPDU(extractRemainder(securityParameters.second));
                      return std::make_pair(
                        SnmpV3Message(
                          SnmpV3Message::ID(id.first),
                          maxSize.first,
                          (SnmpV3Message::Flags)msgFlags.first[0],
                          SnmpV3Message::SecurityModel(securityModel.first),
                          SnmpV3SecParams(std::move(securityParameters.first)),
                          SnmpV3ScopedPduData(std::move(scopedPDU))),
                        offsetOfSecurityParametersInData);
                    }
                    catch(xju::Exception const& e){
                      std::ostringstream s;
                      s << "scoped pdu type " << xju::format::hex(scopedPduData.first.first)
                        << " and length " << formatLength(scopedPduData.first.second)
                        << " at " << securityParameters.second;
                      ok.push_back(s.str());
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
                  s << "security model " << securityModel.first
                    << " at " << msgFlags.second;
                  ok.push_back(s.str());
                  throw;
                }
              }
              catch(xju::Exception const& e) {
                std::ostringstream s;
                s << "message flags (hex) "
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
        s << "snmp version 3 at " << s1.second;
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
