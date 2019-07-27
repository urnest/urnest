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
#include "xju/snmp/SnmpV2cVarResponse.hh"
#include "xju/snmp/SnmpV2cGetRequest.hh"
#include "xju/snmp/SnmpV2cSetRequest.hh"
#include "xju/snmp/SnmpV2cResponse.hh"
#include <xju/seq_less.hh>
#include <xju/snmp/SnmpV2cGetNextRequest.hh>

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
  // Value::
  bool less(Value const& yy) const throw() override
  {
    auto const y{dynamic_cast<Sequence const&>(yy)};
    return xju::seq_less(items_.begin(),items_.end(),
                         y.items_.begin(),y.items_.end(),
                         [](std::shared_ptr<Value const> const& x,
                            std::shared_ptr<Value const> const& y){
                           return (*x) < (*y);
                         });
  }
};

uint64_t encodedLengthOfItems(
  std::vector<SnmpV2cVarResponse> const& items) throw()
{
  return std::accumulate(
    items.begin(),
    items.end(),
    uint64_t{0},
    [](uint64_t t, SnmpV2cVarResponse const& x){
      return t+x.encodedLength();});
}

class V2cSequence : private xju::Int<V2cSequence,size_t>, //encoded length of items
                    public Value
{
public:
  V2cSequence(std::vector<SnmpV2cVarResponse> const& items,
              uint8_t sequenceType) throw():
      xju::Int<V2cSequence,size_t>(encodedLengthOfItems(items)),
      Value(1/*type*/+
            encodedLengthOfLength(value())+
            value()),
      items_(items),
      sequenceType_(sequenceType) {
  }
  std::vector<SnmpV2cVarResponse> const items_;
  uint8_t const sequenceType_;

  // Value::
  std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw() override
  {
    auto at=begin;
    *at++=sequenceType_;
    at=encodeLength(at,value());
    for(SnmpV2cVarResponse x: items_) {
      at=x.encodeTo(at);
    }
    xju::assert_equal(at-begin, encodedLength_);
    return at;
  }
  // Value::
  std::string str() const throw() override
  {
    return xju::format::join(items_.begin(),items_.end(),
                             [](SnmpV2cVarResponse const& x) {
                               return xju::format::str(x);
                             },", ");
  }
  // Value::
  bool less(Value const& yy) const throw() override
  {
    auto const y{dynamic_cast<V2cSequence const&>(yy)};
    return xju::seq_less(items_.begin(),items_.end(),
                         y.items_.begin(),y.items_.end());
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
      vp(new StringValue(std::vector<uint8_t>(community._.begin(),
                                              community._.end()))),
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

std::vector<uint8_t> encodePDU(
  Community const& community,
  RequestId requestId,
  uint64_t error,
  uint64_t errorIndex,
  std::vector<SnmpV2cVarResponse> const& vars,
  uint8_t pduType // 0xA2 snmp v1 get etc
  ) throw()
{
  Sequence s({
      vp(new IntValue(1)), //SNMP V2c
      vp(new StringValue(std::vector<uint8_t>(community._.begin(),
                                              community._.end()))),
      vp(new Sequence({
            vp(new IntValue(requestId.value())),
            vp(new IntValue(error)),//error
            vp(new IntValue(errorIndex)),//errorIndex
            vp(new V2cSequence(vars,0x30))},
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


std::vector<uint8_t> encodeResponse(
  SnmpV2cGetRequest const& request,
  std::vector<Oid> const& paramOrder,
  std::map<Oid,SnmpV2cVarResponse> const& results)
    throw()
{
  std::vector<SnmpV2cVarResponse> vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return ((*results.find(oid)).second);
                 });
  return encodePDU(request.community_,
                   request.id_,
                   0,//error
                   0,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV2cGetRequest const& request,
  std::vector<Oid> const& paramOrder,
  NoSuchName const& error) throw()
{
  std::vector<SnmpV2cVarResponse> vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return SnmpV2cVarResponse(
                     oid,
                     vp(new xju::snmp::NullValue));
                 });
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(paramOrder.begin(),paramOrder.end(),error.param_));
  xju::assert_not_equal(errorIndex,paramOrder.end());

  return encodePDU(request.community_,
                   request.id_,
                   (int)SnmpV2cResponse::ErrorStatus::NO_SUCH_NAME,//error
                   errorIndex-paramOrder.begin()+1,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV2cGetRequest const& request,
  std::vector<Oid> const& paramOrder,
  TooBig const& error) throw()
{
  std::vector<SnmpV2cVarResponse> vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return SnmpV2cVarResponse(
                     oid,
                     vp(new xju::snmp::NullValue));
                 });
  return encodePDU(request.community_,
                   request.id_,
                   (int)SnmpV2cResponse::ErrorStatus::TOO_BIG,//error
                   0,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV2cGetRequest const& request,
  std::vector<Oid> const& paramOrder,
  GenErr const& error) throw()
{
  std::vector<SnmpV2cVarResponse> vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return SnmpV2cVarResponse(
                     oid,
                     vp(new xju::snmp::NullValue));
                 });
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(paramOrder.begin(),paramOrder.end(),error.param_));
  xju::assert_not_equal(errorIndex,paramOrder.end());

  return encodePDU(request.community_,
                   request.id_,
                   (int)SnmpV2cResponse::ErrorStatus::GEN_ERR,//error
                   errorIndex-paramOrder.begin()+1,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  std::vector<Oid> const& paramOrder) throw()
{
  std::vector<SnmpV2cVarResponse> vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return SnmpV2cVarResponse(
                     oid,(*request.values_.find(oid)).second);
                 });
  return encodePDU(request.community_,
                   request.id_,
                   0,//error
                   0,//errorIndex
                   vars,
                   0xA2);
}


std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  TooBig const& error) throw()
{
  std::vector<SnmpV2cVarResponse> vars;
  return encodePDU(request.community_,
                   request.id_,
                   (int)SnmpV2cResponse::ErrorStatus::TOO_BIG,//error
                   0,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  UndoFailed const& error,
  std::vector<Oid> const& paramOrder) throw()
{
  std::vector<SnmpV2cVarResponse> vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return SnmpV2cVarResponse(
                     oid,(*request.values_.find(oid)).second);
                 });
  return encodePDU(request.community_,
                   request.id_,
                   (int)SnmpV2cResponse::ErrorStatus::UNDO_FAILED,//error
                   0,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV2cGetNextRequest const& request,
  std::vector<SnmpV2cVarResponse> const& results)
    throw()
{
  return encodePDU(request.community_,
                   request.id_,
                   0,//error
                   0,//errorIndex
                   results,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV2cGetNextRequest const& request,
  TooBig const& error) throw()
{
  std::vector<SnmpV2cVarResponse> vars;
  std::transform(request.oids_.begin(),
                 request.oids_.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return SnmpV2cVarResponse(
                     oid,
                     vp(new xju::snmp::NullValue));
                 });
  
  return encodePDU(request.community_,
                   request.id_,
                   (int)SnmpV2cResponse::ErrorStatus::TOO_BIG,//error
                   0,//errorIndex
                   vars,
                   0xA2);
}

std::vector<uint8_t> encodeResponse(
  SnmpV2cGetNextRequest const& request,
  GenErr const& error) throw()
{
  std::vector<SnmpV2cVarResponse> vars;
  std::transform(request.oids_.begin(),
                 request.oids_.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return SnmpV2cVarResponse(
                     oid,
                     vp(new xju::snmp::NullValue));
                 });
  
  std::vector<Oid>::const_iterator const errorIndex(
    std::find(request.oids_.begin(),request.oids_.end(),error.param_));
  xju::assert_not_equal(errorIndex,request.oids_.end());

  return encodePDU(request.community_,
                   request.id_,
                   (int)SnmpV2cResponse::ErrorStatus::GEN_ERR,//error
                   errorIndex-request.oids_.begin()+1,//errorIndex
                   vars,
                   0xA2);
}


}
}
// the rest of the implementations are generated by Odinfile into:
#include <xju/snmp/encodeResponse.gen.hh>
