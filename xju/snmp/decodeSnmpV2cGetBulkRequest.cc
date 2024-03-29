// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "decodeSnmpV2cGetBulkRequest.hh"
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
SnmpV2cGetBulkRequest decodeSnmpV2cGetBulkRequest(
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
      if (snmpVersion.first!=1) {
        SnmpVersionMismatch e(snmpVersion.first,1,XJU_TRACED);
        std::ostringstream s;
        s << "decode at " << s1.second;
        e.addContext(s.str(),XJU_TRACED);
        throw e;
      }
      try {
        auto const community(decodeStringValue(snmpVersion.second)); try {
          auto const s2(decodeSequenceTypeAndLength(community.second)); 
          if (s2.first.first!=0xA5) {
            throw RequestTypeMismatch(s2.first.first,0xA5,XJU_TRACED);
          }
          try {
            auto const id(decodeIntValue(s2.second)); try {
              auto const numberOfNonRepeaters(decodeIntValue(id.second)); try {
                auto const maxRep(decodeIntValue(numberOfNonRepeaters.second)); try {
                  auto const s3(decodeSequenceTypeAndLength(maxRep.second)); try {
                    DecodeIterator at(s3.second);
                    std::vector<Oid> oids;
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
                        auto const value(decodeValue(oid.second));
                        oids.push_back(oid.first);
                        at=value.second;
                        if (!s.first.second.valid()) {
                          // X.690 indefinite length - skip 2x zero bytes
                          at=at+2;
                        }
                      }
                      catch(xju::Exception& e) {
                        std::transform(
                          oids.rbegin(),oids.rend(),
                          std::back_inserter(ok),
                          [](Oid x) {
                            std::ostringstream s;
                            s << x.toString();
                            return s.str();
                          });
                        std::ostringstream s;
                        s << "decode param oid and value sequence at offset " 
                          << at;
                        e.addContext(s.str(), XJU_TRACED);
                        throw;
                      }
                    }
                    std::vector<Oid> nonRepeaters(
                      oids.begin(), oids.begin()+std::min(oids.size(),numberOfNonRepeaters.first));
                    std::vector<Oid> repeaters(oids.begin()+nonRepeaters.size(), oids.end());
                    
                    return SnmpV2cGetBulkRequest(
                      Community(std::string(community.first.begin(),
                                            community.first.end())),
                      RequestId(id.first),
                      std::move(nonRepeaters),
                      {maxRep.first,std::move(repeaters)});
                  }
                  catch(xju::Exception& e) {
                    std::ostringstream s;
                    s << "3rd sequence type " 
                      << xju::format::hex(s3.first.first)
                      << " and length " << formatLength(s3.first.second)
                      << " at " << maxRep.second;
                    ok.push_back(s.str());
                    throw;
                  }
                }
                catch(xju::Exception const& e) {
                  std::ostringstream s;
                  s << "max repetitions " << maxRep.first
                    << " at " << numberOfNonRepeaters.second;
                  ok.push_back(s.str());
                  throw;
                }
              }
              catch(xju::Exception const& e) {
                std::ostringstream s;
                s << "number of non repeaters " << xju::format::hex(numberOfNonRepeaters.first)
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
    s << "decode snmp v2c get bulk request from " << data.size()
      << " bytes of data";
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
