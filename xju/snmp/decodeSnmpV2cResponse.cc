// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "decodeSnmpV2cResponse.hh"
#include <string>
#include "xju/Optional.hh"
#include "xju/format.hh"
#include "xju/snmp/DecodeIterator.hh"
#include "xju/snmp/decodeSequenceTypeAndLength.hh"
#include "xju/snmp/decodeIntValue.hh"
#include "xju/snmp/decodeStringValue.hh"
#include "xju/snmp/decodeOidValue.hh"
#include "xju/snmp/decodeValue.hh"
#include <algorithm>
#include <iterator>
#include <limits.h>
#include "xju/snmp/decodeLength.hh"

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

SnmpV2cResponse decodeSnmpV2cResponse(std::vector<uint8_t> const& data) /*throw(
    // malformed
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
      if (snmpVersion.first!=1) {
        std::ostringstream s;
        s << "expected integer 1 (SNMP V2c), got integer " 
          << snmpVersion.first
          << ", at " << s1.second;
        throw xju::Exception(s.str(), XJU_TRACED);
      }
      try {
        auto const community(decodeStringValue(snmpVersion.second)); try {
          auto const s2(decodeSequenceTypeAndLength(community.second)); try {
            auto const id(decodeIntValue(s2.second)); try {
              auto const errorStatus(decodeIntValue(id.second)); try {
                auto const ei(decodeIntValue(errorStatus.second)); try {
                  auto const s3(decodeSequenceTypeAndLength(ei.second)); try {
                    DecodeIterator at(s3.second);
                    std::vector<
                      SnmpVar> varResults;
                    auto atEnd=[&]() {
                      if (s3.first.second.valid())
                      {
                       // X.690 definite length
                       return at == s3.second+s3.first.second.value();
                      }
                      else
                      {
                       // X.690 indefinite length end in two zero bytes
                        return (((*at)==0) && ((*(at+1))==0)); 
                      }
                    };
                    while(!atEnd()) {
                      try {
                        auto const s(decodeSequenceTypeAndLength(at));
                        auto const oid(decodeOidValue(s.second));
                        switch(*oid.second) {
                        case 0x80:
                        case 0x81:
                        case 0x82:
                        {
                          auto const n(decodeLength(at+1));
                          if (!n.first.valid()) {
                            std::ostringstream s;
                            s << "indefinite length is illegal for 'null' "
                              << "value associated with exception " << (*oid.second)
                              << " for oid " << oid.first;
                            throw xju::Exception(s.str(),XJU_TRACED);
                          }
                          auto const o(oid.first);
                          switch(*oid.second){
                          case 0x80:
                            varResults.push_back(SnmpVar(o,SnmpVar::NoSuchObject(o,XJU_TRACED)));
                          break;
                          case 0x81:
                            varResults.push_back(SnmpVar(o,SnmpVar::NoSuchInstance(o,XJU_TRACED)));
                            break;
                          case 0x82:
                            varResults.push_back(SnmpVar(o,SnmpVar::EndOfMibView(o,XJU_TRACED)));
                            break;
                          }
                          at=n.second+n.first.value();
                          break;
                        }
                        default:
                        {
                          auto const value(decodeValue(oid.second));
                          varResults.push_back(
                            SnmpVar(oid.first,value.first));
                          at=value.second;
                          if (!s.first.second.valid()) {
                            // X.690 indefinite length - skip 2x zero bytes
                            at=at+2;
                          }
                        }
                        }
                      }
                      catch(xju::Exception& e) {
                        std::transform(
                          varResults.rbegin(),varResults.rend(),
                          std::back_inserter(ok),
                          xju::format::str<SnmpVar>);
                        std::ostringstream s;
                        s << "decode param oid and value sequence at offset " 
                          << at;
                        e.addContext(s.str(), XJU_TRACED);
                        throw;
                      }
                    }
                    if (errorStatus.first > INT_MAX) {
                      std::ostringstream s;
                      s << "error status " << errorStatus.first 
                        << " exceeds maximimum supported ("
                        << INT_MAX << ")";
                      throw xju::Exception(s.str(), XJU_TRACED);
                    }
                    return SnmpV2cResponse(
                      s2.first.first,
                      Community(std::string(community.first.begin(),
                                            community.first.end())),
                      RequestId(id.first),
                      (SnmpV2cResponse::ErrorStatus)errorStatus.first,
                      SnmpV2cResponse::ErrorIndex(ei.first),
                      varResults);
                  }
                  catch(xju::Exception& e) {
                    std::ostringstream s;
                    s << "3rd sequence type " 
                      << xju::format::hex(s3.first.first)
                      << " and length " << formatLength(s3.first.second)
                      << " at " << ei.second;
                    ok.push_back(s.str());
                    throw;
                  }
                }
                catch(xju::Exception const& e) {
                  std::ostringstream s;
                  s << "error index " << ei.first
                    << " at " << errorStatus.second;
                  ok.push_back(s.str());
                  throw;
                }
              }
              catch(xju::Exception const& e) {
                std::ostringstream s;
                s << "error " << xju::format::hex(errorStatus.first)
                  << " at " << id.second;
                ok.push_back(s.str());
                throw;
              }
            }
            catch(xju::Exception const& e) {
              std::ostringstream s;
              s << "request id " << id.first << " at " << s2.second;
              ok.push_back(s.str());
              throw;
            }
          }
          catch(xju::Exception const& e) {
            std::ostringstream s;
            s << "2nd sequence type " << xju::format::hex(s2.first.first)
              << " and length " 
              << formatLength(s2.first.second)
              << " at " << community.second;
            ok.push_back(s.str());
            throw;
          }
        }
        catch(xju::Exception const& e) {
          std::ostringstream s;
          s << "community "
            << xju::format::quote(std::string(community.first.begin(),
                                              community.first.end()))
            << " at " << snmpVersion.second;
          ok.push_back(s.str());
          throw;
        }
      }
      catch(xju::Exception const& e) {
        std::ostringstream s;
        s << "snmp version 1 at " << s1.second;
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
    s << "decode snmp v2c response from " << data.size() << " bytes of data";
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

