// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/encode.hh>

#include <xju/countSignificantBits.hh>
#include <xju/snmp/SnmpV1GetNextRequest.hh>
#include <xju/snmp/SnmpV1Trap.hh>
#include <xju/assert.hh>
#include <algorithm>
#include <vector>
#include <xju/snmp/encodedLengthOfLength.hh>
#include <xju/snmp/encodeLength.hh>
#include <xju/snmp/SnmpV1SetRequest.hh>
#include <xju/snmp/SnmpV2cSetRequest.hh>
#include <xju/snmp/OidValue.hh>
#include <xju/snmp/IntValue.hh>
#include <xju/snmp/StringValue.hh>
#include <xju/snmp/NullValue.hh>
#include <xju/snmp/IPv4AddressValue.hh>
#include <xju/snmp/TimeTicksValue.hh>
#include <xju/snmp/SnmpV1GetRequest.hh>
#include <xju/snmp/SnmpV1Response.hh>
#include <xju/snmp/SnmpV2cGetRequest.hh>
#include <xju/snmp/SnmpV2cGetNextRequest.hh>
#include <xju/format.hh>
#include <xju/snmp/SnmpV2cTrap.hh>
#include <xju/snmp/SnmpV2cGetBulkRequest.hh>
#include <xju/snmp/encodePDU.hh>
#include <xju/snmp/SnmpV2cResponse.hh>
#include <xju/snmp/SnmpV2cVarResponse.hh>
#include <utility>
#include <xju/snmp/SnmpV3ScopedPDU.hh>
#include <xju/snmp/PreEncoded.hh>
#include <xju/snmp/SnmpV3Message.hh>
#include <xju/snmp/Sequence.hh>
#include <xju/snmp/SnmpV3UsmSecurityParameters.hh>

namespace xju
{
namespace snmp
{


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

std::vector<uint8_t> encode(SnmpV1Response const& response) throw()
{
  return encodePDU(
    0, // SNMP version 1
    response.community_,
    response.id_,
    (uint64_t)response.error_,
    response.errorIndex_.value(),
    response.values_,
    0xA2);
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


std::vector<uint8_t> encode(SnmpV2cGetBulkRequest const& request) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(request.getNext_.begin(),
                 request.getNext_.end(),
                 std::back_inserter(params),
                 [](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                         vp(new NullValue)},
                       0x30));
                 });
  std::transform(request.getNextN_.begin(),
                 request.getNextN_.end(),
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
      vp(new StringValue(
           std::vector<uint8_t>(request.community_._.begin(),
                                request.community_._.end()))),
      vp(new Sequence({
            vp(new IntValue(request.id_.value())),
            vp(new IntValue(request.getNext_.size())),//non-repeaters
            vp(new IntValue(request.n_)),//max-repetitions
            vp(new Sequence(params,0x30))},
          0xA5))}, // SNMP Get Bulk
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

std::vector<uint8_t> encode(SnmpV2cResponse const& response) throw()
{
  std::vector<SnmpV2cVarResponse> vars;
  for(auto v: response.varResults_){
    if (v.e_.valid()){
      switch(v.e_.value()){
      case SnmpV2cResponse::VarResult::E::NO_SUCH_OBJECT:
        vars.push_back(SnmpV2cVarResponse(v.oid_,
                                          SnmpV2cVarResponse::NoSuchObject(v.oid_,XJU_TRACED)));
        break;
      case SnmpV2cResponse::VarResult::E::NO_SUCH_INSTANCE:
        vars.push_back(SnmpV2cVarResponse(v.oid_,
                                          SnmpV2cVarResponse::NoSuchInstance(v.oid_,XJU_TRACED)));
        break;
      case SnmpV2cResponse::VarResult::E::END_OF_MIB_VIEW:
        vars.push_back(SnmpV2cVarResponse(v.oid_,
                                          SnmpV2cVarResponse::EndOfMibView(v.oid_,XJU_TRACED)));
        break;
      default:
        xju::assert_never_reached();
      }
    }else{
      vars.push_back(SnmpV2cVarResponse(v.oid_, v.v_));
    }
  }
  return encodePDU(
    response.community_,
    response.id_,
    (uint64_t)response.error_,
    response.errorIndex_.value(),
    vars,
    0xA2);
}


std::vector<uint8_t> encode(SnmpV3Message const& x) throw()
{
  typedef std::shared_ptr<Value const> vp;

  Sequence s({
      vp(new IntValue(3)), // SNMP version 3
      vp(new Sequence({
            vp(new IntValue(x.id_.value())),
            vp(new IntValue(x.maxSize_)),
            vp(new StringValue(std::vector<uint8_t>({x.flags_}))),
            vp(new IntValue(x.securityModel_.value()))},
          0x30)),
      vp(new StringValue(std::move(x.securityParameters_))),
      vp(new PreEncoded(std::move(x.scopedPduData_)))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}


std::vector<uint8_t> encode(SnmpV3ScopedPDU x) throw()
{
  typedef std::shared_ptr<Value const> vp;

  Sequence s({
      vp(new StringValue(std::move(x.contextEngineID_._))),
      vp(new StringValue(std::move(x.contextName_._))),
      vp(new PreEncoded(std::move(x.encodedPDU_)))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}


std::vector<uint8_t> encode(
  SnmpV3UsmSecurityParameters const& genericParams,
  std::vector<uint8_t> const& preEncodedAuthParams,
  std::vector<uint8_t> const& preEncodedPrivParams) throw()
{
  typedef std::shared_ptr<Value const> vp;

  Sequence s({
      vp(new StringValue(std::vector<uint8_t>(genericParams.engineID_._.begin(),
                                              genericParams.engineID_._.end()))),
      vp(new IntValue(genericParams.engineBoots_.value())),
      vp(new IntValue(genericParams.engineTime_.value())),
      vp(new StringValue(std::vector<uint8_t>(genericParams.userName_._.begin(),
                                              genericParams.userName_._.end()))),
      vp(new StringValue(std::vector<uint8_t>(preEncodedAuthParams.begin(),
                                              preEncodedAuthParams.end()))),
      vp(new StringValue(std::vector<uint8_t>(preEncodedPrivParams.begin(),
                                              preEncodedPrivParams.end())))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}
  
}
}
