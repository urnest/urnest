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
#include "xju/snmp/SnmpV1GetNextRequest.hh"

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

typedef std::shared_ptr<Value const> vp;

std::vector<uint8_t> encodePDU(
  int snmpVersion, //0 == SNMP version 1
  Community const& community,
  RequestId requestId,
  uint64_t error,
  uint64_t errorIndex,
  std::vector<std::pair<Oid, vp> > const& vars,
  uint8_t pduType // 0xA2 snmp v1 get etc
  ) throw()
{
  std::vector<vp > params;
  std::transform(vars.begin(),
                 vars.end(),
                 std::back_inserter(params),
                 [](std::pair<Oid,vp> const& var) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(var.first)),
                           vp(var.second)
                           },
                       0x30));
                 });
                       
  Sequence s({
      vp(new IntValue(snmpVersion)),
      vp(new StringValue(community._)),
      vp(new Sequence({
            vp(new IntValue(requestId.value())),
            vp(new IntValue(error)),//error
            vp(new IntValue(errorIndex)),//errorIndex
            vp(new Sequence(params,0x30))},
        pduType))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

}

std::vector<uint8_t> encodeResponse(
  SnmpV1GetRequest const& request,
  std::vector<Oid> const& paramOrder,
  std::map<Oid,std::shared_ptr<xju::snmp::Value const> > const& results)
    throw()
{
  std::vector<std::pair<Oid,vp> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     vp((*results.find(oid)).second));
                 });
  return encodePDU(0, // SNMP version 1
                   request.community_,
                   request.id_,
                   0,//error
                   0,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV1GetRequest const& request,
  std::vector<Oid> const& paramOrder,
  NoSuchName const& error) throw()
{
  std::vector<std::pair<Oid,vp> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     vp(new xju::snmp::NullValue));
                 });
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(paramOrder.begin(),paramOrder.end(),error.param_));
  xju::assert_not_equal(errorIndex,paramOrder.end());

  return encodePDU(0, // SNMP version 1
                   request.community_,
                   request.id_,
                   (int)SnmpV1Response::ErrorStatus::NO_SUCH_NAME,//error
                   errorIndex-paramOrder.begin()+1,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV1GetRequest const& request,
  std::vector<Oid> const& paramOrder,
  TooBig const& error) throw()
{
  std::vector<std::pair<Oid,vp> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     vp(new xju::snmp::NullValue));
                 });
  return encodePDU(0, // SNMP version 1
                   request.community_,
                   request.id_,
                   (int)SnmpV1Response::ErrorStatus::TOO_BIG,//error
                   0,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV1GetRequest const& request,
  std::vector<Oid> const& paramOrder,
  GenErr const& error) throw()
{
  std::vector<std::pair<Oid,vp> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     vp(new xju::snmp::NullValue));
                 });
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(paramOrder.begin(),paramOrder.end(),error.param_));
  xju::assert_not_equal(errorIndex,paramOrder.end());

  return encodePDU(0, // SNMP version 1
                   request.community_,
                   request.id_,
                   (int)SnmpV1Response::ErrorStatus::GEN_ERR,//error
                   errorIndex-paramOrder.begin()+1,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder)
    throw()
{
  std::vector<std::pair<Oid,vp> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     vp((*request.values_.find(oid)).second));
                 });
  return encodePDU(0, // SNMP version 1
                   request.community_,
                   request.id_,
                   0,//error
                   0,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder,
  NoSuchName const& error) throw()
{
  std::vector<std::pair<Oid,vp> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     vp((*request.values_.find(oid)).second));
                 });
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(paramOrder.begin(),paramOrder.end(),error.param_));
  xju::assert_not_equal(errorIndex,paramOrder.end());
  return encodePDU(0, // SNMP version 1
                   request.community_,
                   request.id_,
                   (int)SnmpV1Response::ErrorStatus::NO_SUCH_NAME,//error
                   errorIndex-paramOrder.begin()+1,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder,
  BadValue const& error) throw()
{
  std::vector<std::pair<Oid,vp> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     vp((*request.values_.find(oid)).second));
                 });
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(paramOrder.begin(),paramOrder.end(),error.param_));
  xju::assert_not_equal(errorIndex,paramOrder.end());
  return encodePDU(0, // SNMP version 1
                   request.community_,
                   request.id_,
                   (int)SnmpV1Response::ErrorStatus::BAD_VALUE,//error
                   errorIndex-paramOrder.begin()+1,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder,
  ReadOnly const& error) throw()
{
  std::vector<std::pair<Oid,vp> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     vp((*request.values_.find(oid)).second));
                 });
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(paramOrder.begin(),paramOrder.end(),error.param_));
  xju::assert_not_equal(errorIndex,paramOrder.end());
  return encodePDU(0, // SNMP version 1
                   request.community_,
                   request.id_,
                   (int)SnmpV1Response::ErrorStatus::READ_ONLY,//error
                   errorIndex-paramOrder.begin()+1,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder,
  TooBig const& error) throw()
{
  std::vector<std::pair<Oid,vp> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     vp((*request.values_.find(oid)).second));
                 });
  return encodePDU(0, // SNMP version 1
                   request.community_,
                   request.id_,
                   (int)SnmpV1Response::ErrorStatus::TOO_BIG,//error
                   0,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder,
  GenErr const& error) throw()
{
  std::vector<std::pair<Oid,vp> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     vp((*request.values_.find(oid)).second));
                 });
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(paramOrder.begin(),paramOrder.end(),error.param_));
  xju::assert_not_equal(errorIndex,paramOrder.end());
  return encodePDU(0, // SNMP version 1
                   request.community_,
                   request.id_,
                   (int)SnmpV1Response::ErrorStatus::GEN_ERR,//error
                   errorIndex-paramOrder.begin()+1,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV1GetNextRequest const& request,
  std::vector<std::pair<Oid,std::shared_ptr<xju::snmp::Value const> > > const& results)
    throw()
{
  return encodePDU(0, // SNMP version 1
                   request.community_,
                   request.id_,
                   0,//error
                   0,//errorIndex
                   results,
                   0xA2);
}


std::vector<uint8_t> encodeResponse(
  SnmpV1GetNextRequest const& request,
  TooBig const& error) throw()
{
  std::vector<std::pair<Oid,vp> > vars;
  std::transform(request.oids_.begin(),
                 request.oids_.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     vp(new xju::snmp::NullValue));
                 });
  return encodePDU(0, // SNMP version 1
                   request.community_,
                   request.id_,
                   (int)SnmpV1Response::ErrorStatus::TOO_BIG,//error
                   0,//errorIndex
                   vars,
                   0xA2);
}


std::vector<uint8_t> encodeResponse(
  SnmpV1GetNextRequest const& request,
  GenErr const& error) throw()
{
  std::vector<std::pair<Oid,vp> > vars;
  std::transform(request.oids_.begin(),
                 request.oids_.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     vp(new xju::snmp::NullValue));
                 });
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(request.oids_.begin(),request.oids_.end(),error.param_));
  xju::assert_not_equal(errorIndex,request.oids_.end());
  return encodePDU(0, // SNMP version 1
                   request.community_,
                   request.id_,
                   (int)SnmpV1Response::ErrorStatus::GEN_ERR,//error
                   errorIndex-request.oids_.begin()+1,//errorIndex
                   vars,
                   0xA2);
}



}
}
