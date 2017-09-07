// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/snmp/decodeSnmpV2cTrap.hh"

#include <xju/Optional.hh>
#include <xju/format.hh>
#include <xju/snmp/DecodeIterator.hh>
#include <xju/snmp/decodeSequenceTypeAndLength.hh>
#include <xju/snmp/decodeIntValue.hh>
#include <xju/snmp/decodeStringValue.hh>
#include <xju/snmp/RequestTypeMismatch.hh>
#include <sstream>
#include <vector>
#include <xju/snmp/decodeOidValue.hh>
#include <xju/snmp/decodeValue.hh>
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

SnmpV2cTrap decodeSnmpV2cTrap(std::vector<uint8_t> const& data) throw(
    SnmpVersionMismatch,
    RequestTypeMismatch,
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
          if (s2.first.first!=0xA7) {
            RequestTypeMismatch e(s2.first.first,0xA7,XJU_TRACED);
            std::ostringstream s;
            s << "decode at " << community.second;
            e.addContext(s.str(),XJU_TRACED);
            throw e;
          }
          try {
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
                    if (values.size()<1){
                      std::ostringstream s;
                      s << "trap is missing sys uptime ie sysUpTime.0 var";
                      throw xju::Exception(s.str(),XJU_TRACED);
                    }
                    if (values[0].first!=Oid(".1.3.6.1.2.1.1.3.0")){
                      std::ostringstream s;
                      s << "first trap var oid is not .1.3.6.1.2.1.1.3.0"
                        << " (ie not sys uptime ie sysUpTime.0), it is "
                        << values[0].first;
                      throw xju::Exception(s.str(),XJU_TRACED);
                    }
                    std::chrono::milliseconds const sysUpTime(
                      values[0].second->timeTicksValue());
                    if (values.size()<2){
                      std::ostringstream s;
                      s << "trap is missing trap type ie snmpTrapOID.0 var";
                      throw xju::Exception(s.str(),XJU_TRACED);
                    }
                    if (values[1].first!=Oid(".1.3.6.1.6.3.1.1.4.1.0")){
                      std::ostringstream s;
                      s << "first trap var oid is not 1.3.6.1.6.3.1.1.4.1.0"
                        << " (ie not trap type ie not snmpTrapOID.0), it is "
                        << values[0].first;
                      throw xju::Exception(s.str(),XJU_TRACED);
                    }
                    Oid const trapType(values[1].second->oidValue());
                    return SnmpV2cTrap(
                      Community(std::string(community.first.begin(),
                                            community.first.end())),
                      RequestId(id.first),
                      trapType,
                      sysUpTime,
                      decltype(values)(values.begin()+2,values.end()));
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
    s << "decode snmp v2c trap from " 
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



