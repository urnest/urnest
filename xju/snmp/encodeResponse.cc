// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/encodeResponse.hh>
#include <vector>
#include <algorithm>
#include <xju/Int.hh>
#include <unistd.h>
#include "xju/assert.hh"
#include <xju/format.hh>
#include <iterator>
#include "xju/snmp/SnmpV1GetRequest.hh"
#include "xju/snmp/encodedLengthOfLength.hh"
#include "xju/snmp/encodeLength.hh"
#include "xju/snmp/OidValue.hh"
#include "xju/snmp/IntValue.hh"
#include "xju/snmp/StringValue.hh"
#include "xju/snmp/NullValue.hh"
#include "xju/snmp/NoSuchName.hh"
#include "xju/snmp/SnmpV1Response.hh"
#include "xju/snmp/TooBig.hh"
#include "xju/snmp/GenErr.hh"
#include "xju/snmp/SnmpV1SetRequest.hh"
#include "xju/snmp/BadValue.hh"
#include "xju/snmp/ReadOnly.hh"

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

class Sequence : private xju::Int<Sequence,size_t>, //encoded length of items
                 public Value
{
public:
  Sequence(std::vector<std::shared_ptr<Value const> > const& items,
           uint8_t sequenceType) throw():
      xju::Int<Sequence,size_t>(encodedLengthOfItems(items)),
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

std::vector<uint8_t> encodeResponse(
  SnmpV1GetRequest const& request,
  std::vector<Oid> const& paramOrder,
  std::map<Oid,std::shared_ptr<xju::snmp::Value const> > const& results)
    throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(params),
                 [&](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                           vp((*results.find(oid)).second)},
                       0x30));
                 });
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1
      vp(new StringValue(request.community_._)),
      vp(new Sequence({
            vp(new IntValue(request.id_.value())),
            vp(new IntValue(0)),//error
            vp(new IntValue(0)),//errorIndex
            vp(new Sequence(params,0x30))},
        0xA2))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encodeResponse(
  SnmpV1GetRequest const& request,
  std::vector<Oid> const& paramOrder,
  NoSuchName const& error) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(params),
                 [&](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                           vp(new xju::snmp::NullValue)},
                       0x30));
                 });
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(paramOrder.begin(),paramOrder.end(),error.param_));
  xju::assert_not_equal(errorIndex,paramOrder.end());
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1
      vp(new StringValue(request.community_._)),
      vp(new Sequence({
              vp(new IntValue(request.id_.value())),
              vp(new IntValue((int)SnmpV1Response::ErrorStatus::NO_SUCH_NAME)),//error
              vp(new IntValue(errorIndex-paramOrder.begin()+1)),//errorIndex
              vp(new Sequence(params,0x30))},
        0xA2))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encodeResponse(
  SnmpV1GetRequest const& request,
  std::vector<Oid> const& paramOrder,
  TooBig const& error) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(params),
                 [&](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                           vp(new xju::snmp::NullValue)},
                       0x30));
                 });
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1
      vp(new StringValue(request.community_._)),
      vp(new Sequence({
              vp(new IntValue(request.id_.value())),
              vp(new IntValue((int)SnmpV1Response::ErrorStatus::TOO_BIG)),//error
              vp(new IntValue(0)),//errorIndex
              vp(new Sequence(params,0x30))},
        0xA2))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encodeResponse(
  SnmpV1GetRequest const& request,
  std::vector<Oid> const& paramOrder,
  GenErr const& error) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(params),
                 [&](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                           vp(new xju::snmp::NullValue)},
                       0x30));
                 });
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(paramOrder.begin(),paramOrder.end(),error.param_));
  xju::assert_not_equal(errorIndex,paramOrder.end());
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1
      vp(new StringValue(request.community_._)),
      vp(new Sequence({
              vp(new IntValue(request.id_.value())),
              vp(new IntValue((int)SnmpV1Response::ErrorStatus::GEN_ERR)),//error
              vp(new IntValue(errorIndex-paramOrder.begin()+1)),//errorIndex
              vp(new Sequence(params,0x30))},
        0xA2))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder)
    throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(params),
                 [&](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                         vp((*request.values_.find(oid)).second)},
                       0x30));
                 });
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1
      vp(new StringValue(request.community_._)),
      vp(new Sequence({
            vp(new IntValue(request.id_.value())),
            vp(new IntValue(0)),//error
            vp(new IntValue(0)),//errorIndex
            vp(new Sequence(params,0x30))},
        0xA2))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder,
  NoSuchName const& error) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(params),
                 [&](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                         vp((*request.values_.find(oid)).second)},
                       0x30));
                 });
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(paramOrder.begin(),paramOrder.end(),error.param_));
  xju::assert_not_equal(errorIndex,paramOrder.end());
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1
      vp(new StringValue(request.community_._)),
      vp(new Sequence({
              vp(new IntValue(request.id_.value())),
              vp(new IntValue((int)SnmpV1Response::ErrorStatus::NO_SUCH_NAME)),//error
              vp(new IntValue(errorIndex-paramOrder.begin()+1)),//errorIndex
              vp(new Sequence(params,0x30))},
        0xA2))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder,
  BadValue const& error) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(params),
                 [&](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                         vp((*request.values_.find(oid)).second)},
                       0x30));
                 });
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(paramOrder.begin(),paramOrder.end(),error.param_));
  xju::assert_not_equal(errorIndex,paramOrder.end());
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1
      vp(new StringValue(request.community_._)),
      vp(new Sequence({
              vp(new IntValue(request.id_.value())),
              vp(new IntValue((int)SnmpV1Response::ErrorStatus::BAD_VALUE)),//error
              vp(new IntValue(errorIndex-paramOrder.begin()+1)),//errorIndex
              vp(new Sequence(params,0x30))},
        0xA2))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder,
  ReadOnly const& error) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(params),
                 [&](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                         vp((*request.values_.find(oid)).second)},
                       0x30));
                 });
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(paramOrder.begin(),paramOrder.end(),error.param_));
  xju::assert_not_equal(errorIndex,paramOrder.end());
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1
      vp(new StringValue(request.community_._)),
      vp(new Sequence({
              vp(new IntValue(request.id_.value())),
              vp(new IntValue((int)SnmpV1Response::ErrorStatus::READ_ONLY)),//error
              vp(new IntValue(errorIndex-paramOrder.begin()+1)),//errorIndex
              vp(new Sequence(params,0x30))},
        0xA2))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder,
  TooBig const& error) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(params),
                 [&](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                         vp((*request.values_.find(oid)).second)},
                       0x30));
                 });
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1
      vp(new StringValue(request.community_._)),
      vp(new Sequence({
              vp(new IntValue(request.id_.value())),
              vp(new IntValue((int)SnmpV1Response::ErrorStatus::TOO_BIG)),//error
              vp(new IntValue(0)),//errorIndex
              vp(new Sequence(params,0x30))},
        0xA2))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder,
  GenErr const& error) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(params),
                 [&](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                         vp((*request.values_.find(oid)).second)},
                       0x30));
                 });
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(paramOrder.begin(),paramOrder.end(),error.param_));
  xju::assert_not_equal(errorIndex,paramOrder.end());
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1
      vp(new StringValue(request.community_._)),
      vp(new Sequence({
              vp(new IntValue(request.id_.value())),
              vp(new IntValue((int)SnmpV1Response::ErrorStatus::GEN_ERR)),//error
              vp(new IntValue(errorIndex-paramOrder.begin()+1)),//errorIndex
              vp(new Sequence(params,0x30))},
        0xA2))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}


}
}
