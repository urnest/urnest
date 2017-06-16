// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "decodeSnmpV1Response.hh"
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

SnmpV1Response decodeSnmpV1Response(std::vector<uint8_t> const& data) throw(
    // malformed
    xju::Exception)
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
      if (snmpVersion.first!=0) {
        std::ostringstream s;
        s << "expected integer 0 (SNMP V1), got integer " 
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
                      std::pair<Oid, std::shared_ptr<Value const> > > values;
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
                        values.push_back(
                          std::make_pair(oid.first,value.first));
                        at=value.second;
                        if (!s.first.second.valid()) {
                          // X.690 indefinite length - skip 2x zero bytes
                          at=at+2;
                        }
                      }
                      catch(xju::Exception& e) {
                        std::transform(
                          values.rbegin(),values.rend(),
                          std::back_inserter(ok),
                          [](std::pair<Oid, std::shared_ptr<Value const> > x) {
                            std::ostringstream s;
                            s << x.first.toString() << ": " << (*x.second);
                            return s.str();
                          });
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
                    return SnmpV1Response(
                      s2.first.first,
                      Community(std::string(community.first.begin(),
                                            community.first.end())),
                      RequestId(id.first),
                      (SnmpV1Response::ErrorStatus)errorStatus.first,
                      SnmpV1Response::ErrorIndex(ei.first),
                      values);
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
    s << "decode snmp v1 response from " << data.size() << " bytes of data";
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

