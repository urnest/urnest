// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "encode.hh"

#include <xju/countSignificantBits.hh>
#include "xju/snmp/SnmpV1GetNextRequest.hh"
#include "xju/snmp/SnmpV1Trap.hh"
#include "xju/assert.hh"
#include <algorithm>
#include <vector>
#include "xju/snmp/encodedLengthOfLength.hh"
#include "xju/snmp/encodeLength.hh"
#include "xju/snmp/SnmpV1SetRequest.hh"
#include "xju/snmp/SnmpV2cSetRequest.hh"
#include "xju/snmp/OidValue.hh"
#include "xju/snmp/IntValue.hh"
#include "xju/snmp/StringValue.hh"
#include "xju/snmp/NullValue.hh"
#include "xju/snmp/IPv4AddressValue.hh"
#include "xju/snmp/TimeTicksValue.hh"
#include "xju/snmp/SnmpV1GetRequest.hh"
#include "xju/snmp/SnmpV2cGetRequest.hh"
#include "xju/snmp/SnmpV2cGetNextRequest.hh"
#include "xju/format.hh"
#include <xju/snmp/SnmpV2cTrap.hh>

namespace xju
{
namespace snmp
{


namespace
{

uint64_t encodedLengthOfItems(
  std::vector<std::shared_ptr<Value const> > const& items) throw()
{
  return std::accumulate(
    items.begin(),
    items.end(),
    uint64_t{0},
    [](uint64_t t, std::shared_ptr<Value const> x){
      return t+x->encodedLength();});
}

class Sequence : private Int<Sequence,size_t>, //encoded length of items
                 public Value
{
public:
  Sequence(std::vector<std::shared_ptr<Value const> > const& items,
           uint8_t sequenceType) throw():
      Int<Sequence,size_t>(encodedLengthOfItems(items)),
      Value(1/*type*/+
            encodedLengthOfLength(value())+
            value()),
      items_(items),
      sequenceType_(sequenceType) {
  }
  std::vector<std::shared_ptr<Value const> > const items_;
  uint8_t const sequenceType_;

  // Value::
  std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw() override
  {
    auto at=begin;
    *at++=sequenceType_;
    at=encodeLength(at,value());
    for(std::shared_ptr<Value const> x: items_) {
      at=x->encodeTo(at);
    }
    xju::assert_equal(at-begin, encodedLength_);
    return at;
  }
  // Value::
  std::string str() const throw() override
  {
    return xju::format::join(items_.begin(),items_.end(),
                             [](std::shared_ptr<Value const> x) {
                               return x->str();
                             },", ");
  }
  
};

}

std::vector<uint8_t> encode(SnmpV1GetRequest const& request) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(request.oids_.begin(),
                 request.oids_.end(),
                 std::back_inserter(params),
                 [](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                         vp(new NullValue)},
                       0x30));
                 });
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1 -see RFC 1157
      vp(new StringValue(std::vector<uint8_t>(request.community_._.begin(),
                                              request.community_._.end()))),
      vp(new Sequence({
            vp(new IntValue(request.id_.value())),
            vp(new IntValue(0)),//error
            vp(new IntValue(0)),//errorIndex
            vp(new Sequence(params,0x30))},
        0xA0))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encode(SnmpV1SetRequest const& request) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(request.values_.begin(),
                 request.values_.end(),
                 std::back_inserter(params),
                 [](std::pair<Oid const,vp> const& x) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(x.first)),
                         x.second},
                       0x30));
                 });
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1 -see RFC 1157
      vp(new StringValue(std::vector<uint8_t>(request.community_._.begin(),
                                              request.community_._.end()))),
      vp(new Sequence({
            vp(new IntValue(request.id_.value())),
            vp(new IntValue(0)),//error
            vp(new IntValue(0)),//errorIndex
            vp(new Sequence(params,0x30))},
          0xA3))}, // SNMP Set
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encode(SnmpV1GetNextRequest const& request) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(request.oids_.begin(),
                 request.oids_.end(),
                 std::back_inserter(params),
                 [](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                         vp(new NullValue)},
                       0x30));
                 });
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1 -see RFC 1157
      vp(new StringValue(std::vector<uint8_t>(request.community_._.begin(),
                                              request.community_._.end()))),
      vp(new Sequence({
            vp(new IntValue(request.id_.value())),
            vp(new IntValue(0)),//error
            vp(new IntValue(0)),//errorIndex
            vp(new Sequence(params,0x30))},
        0xA1))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encode(SnmpV1Trap const& trap) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(trap.vars_.begin(),
                 trap.vars_.end(),
                 std::back_inserter(params),
                 [](std::pair<Oid const,vp> const& x) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(x.first)),
                         x.second},
                       0x30));
                 });
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1 -see RFC 1157
      vp(new StringValue(std::vector<uint8_t>(trap.community_._.begin(),
                                              trap.community_._.end()))),
      vp(new Sequence({
            vp(new OidValue(trap.trapType_)),
            vp(new IPv4AddressValue(trap.origin_)),
            vp(new IntValue((int)trap.genericType_)),
            vp(new IntValue(trap.specificType_.value())),
            vp(new TimeTicksValue(trap.timestamp_)),
            vp(new Sequence(params,0x30))},
          0xA4))}, // SNMP Trap
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encode(SnmpV2cGetRequest const& request) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(request.oids_.begin(),
                 request.oids_.end(),
                 std::back_inserter(params),
                 [](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                         vp(new NullValue)},
                       0x30));
                 });
  Sequence s({
      vp(new IntValue(1)), // SNMP version 2c - see RFC 1901
      vp(new StringValue(std::vector<uint8_t>(request.community_._.begin(),
                                              request.community_._.end()))),
      vp(new Sequence({
            vp(new IntValue(request.id_.value())),
            vp(new IntValue(0)),//error
            vp(new IntValue(0)),//errorIndex
            vp(new Sequence(params,0x30))},
        0xA0))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encode(SnmpV2cGetNextRequest const& request) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(request.oids_.begin(),
                 request.oids_.end(),
                 std::back_inserter(params),
                 [](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                         vp(new NullValue)},
                       0x30));
                 });
  Sequence s({
      vp(new IntValue(1)), // SNMP version 2c -see RFC 1901
      vp(new StringValue(std::vector<uint8_t>(request.community_._.begin(),
                                              request.community_._.end()))),
      vp(new Sequence({
            vp(new IntValue(request.id_.value())),
            vp(new IntValue(0)),//error
            vp(new IntValue(0)),//errorIndex
            vp(new Sequence(params,0x30))},
          0xA1))}, // SNMP Get Next
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encode(SnmpV2cSetRequest const& request) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(request.values_.begin(),
                 request.values_.end(),
                 std::back_inserter(params),
                 [](std::pair<Oid const,vp> const& x) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(x.first)),
                         x.second},
                       0x30));
                 });
  Sequence s({
      vp(new IntValue(1)), // SNMP version 2c
      vp(new StringValue(std::vector<uint8_t>(request.community_._.begin(),
                                              request.community_._.end()))),
      vp(new Sequence({
            vp(new IntValue(request.id_.value())),
            vp(new IntValue(0)),//error
            vp(new IntValue(0)),//errorIndex
            vp(new Sequence(params,0x30))},
          0xA3))}, // SNMP Set
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encode(SnmpV2cTrap const& trap) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  params.push_back(
    vp(new Sequence(
         {vp(new OidValue(Oid(".1.3.6.1.2.1.1.3.0"))),
          vp(new TimeTicksValue(trap.timestamp_))},
         0x30)));
  params.push_back(
    vp(new Sequence(
         {vp(new OidValue(Oid(".1.3.6.1.6.3.1.1.4.1.0"))),
          vp(new OidValue(trap.trapType_))},
         0x30)));
  std::transform(trap.vars_.begin(),
                 trap.vars_.end(),
                 std::back_inserter(params),
                 [](std::pair<Oid const,vp> const& x) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(x.first)),
                         x.second},
                       0x30));
                 });
  Sequence s({
      vp(new IntValue(1)), // SNMP version 2c
      vp(new StringValue(std::vector<uint8_t>(trap.community_._.begin(),
                                              trap.community_._.end()))),
      vp(new Sequence({
            vp(new IntValue(trap.id_.value())),
            vp(new IntValue(0)),//error
            vp(new IntValue(0)),//errorIndex
            vp(new Sequence(params,0x30))},
          0xA7))}, // SNMP Trap
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}



}
}


