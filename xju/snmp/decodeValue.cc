// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "decodeValue.hh"
#include "xju/snmp/decodeLength.hh"
#include "xju/snmp/NullValue.hh"
#include "xju/snmp/OidValue.hh"
#include "xju/snmp/decodeOidValue.hh"
#include "xju/snmp/decodeStringValue.hh"
#include "xju/snmp/StringValue.hh"
#include "xju/snmp/decodeIntValue.hh"
#include "xju/snmp/IntValue.hh"
#include <xju/format.hh>
#include "xju/Exception.hh"
#include <xju/snmp/TimeTicksValue.hh>
#include <xju/snmp/decodeIPv4AddressValue.hh>
#include <xju/snmp/IPv4AddressValue.hh>
#include <xju/snmp/Counter32Value.hh>
#include <xju/snmp/Gauge32Value.hh>
#include <xju/snmp/OpaqueValue.hh>
#include <xju/snmp/Counter64Value.hh>

namespace xju
{
namespace snmp
{

std::pair<std::shared_ptr<Value const>, DecodeIterator> decodeValue(
  DecodeIterator const i) /*throw(xju::Exception)*/
{
  try {
    switch(*i) {
    case 0x05:
      try
      {
        auto const l(decodeLength(i+1));
        if (!l.first.valid()) {
          auto j(i+2);
          while((*j)!=0 || (*j+1) !=0) {
            ++j;
          }
          return std::make_pair(std::shared_ptr<Value const>(new NullValue),
                                j);
        }
        return std::make_pair(std::shared_ptr<Value const>(new NullValue),
                              l.second+l.first.value());
      }
      catch(xju::Exception& e) {
        std::ostringstream s;
        s << "decode null at " << i;
        e.addContext(s.str(), XJU_TRACED);
        throw;
      }
      break;
    case 0x06:
    {
      auto v(decodeOidValue(i));
      return std::make_pair(std::shared_ptr<Value const>(
                              new OidValue(v.first)),
                            v.second);
    }
    case 0x04:
    {
      auto v(decodeStringValue(i));
      return std::make_pair(std::shared_ptr<Value const>(
                              new StringValue(v.first)),
                            v.second);
    }
    case 0x02:
    {
      auto v(decodeIntValue(i));
      return std::make_pair(std::shared_ptr<Value const>(
                              new IntValue(v.first)),
                            v.second);
    }
    case 0x40:
    {
      auto v(decodeIPv4AddressValue(i));
      return std::make_pair(std::shared_ptr<Value const>(
                              new IPv4AddressValue(v.first)),
                            v.second);
    }
    case 0x41:
    {
      auto v(decodeIntValue(i,0x41));
      if (v.first>UINT32_MAX){
        std::ostringstream s;
        s << v.first << " is too big to be a Counter32 (UINT32_MAX == "
          << UINT32_MAX << ")";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      return std::make_pair(std::shared_ptr<Value const>(
                              new Counter32Value(v.first)),
                            v.second);
    }
    case 0x42:
    {
      auto v(decodeIntValue(i,0x42));
      if (v.first>UINT32_MAX){
        std::ostringstream s;
        s << v.first << " is too big to be a Gauge32 (UINT32_MAX == "
          << UINT32_MAX << ")";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      return std::make_pair(std::shared_ptr<Value const>(
                              new Gauge32Value(v.first)),
                            v.second);
    }
    case 0x43:
    {
      auto v(decodeIntValue(i,0x43));
      return std::make_pair(std::shared_ptr<Value const>(
                              new TimeTicksValue(
                                std::chrono::milliseconds(v.first*10))),
                            v.second);
    }
    case 0x44:
    {
      auto v(decodeStringValue(i,0x44));
      return std::make_pair(std::shared_ptr<Value const>(
                              new OpaqueValue(v.first)),
                            v.second);
    }
    case 0x46:
    {
      auto v(decodeIntValue(i,0x46));
      return std::make_pair(std::shared_ptr<Value const>(
                              new Counter64Value(v.first)),
                            v.second);
    }
    }
    std::ostringstream s;
    s << "decoding of type " << xju::format::hex(*i) << " is not implemented";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "decode one int/string/oid/null etc value at " << i;
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}


}
}


