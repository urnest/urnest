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
#include <xju/snmp/encodePDU.hh>

namespace xju
{
namespace snmp
{

std::vector<uint8_t> encodeResponse(
  SnmpV1GetRequest const& request,
  std::vector<Oid> const& paramOrder,
  std::map<Oid,std::shared_ptr<xju::snmp::Value const> > const& results)
    throw()
{
  std::vector<std::pair<Oid,std::shared_ptr<Value const>> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     std::shared_ptr<Value const>((*results.find(oid)).second));
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
  std::vector<std::pair<Oid,std::shared_ptr<Value const>> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     std::shared_ptr<Value const>(new xju::snmp::NullValue));
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
  std::vector<std::pair<Oid,std::shared_ptr<Value const>> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     std::shared_ptr<Value const>(new xju::snmp::NullValue));
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
  std::vector<std::pair<Oid,std::shared_ptr<Value const>> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     std::shared_ptr<Value const>(new xju::snmp::NullValue));
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
  std::vector<std::pair<Oid,std::shared_ptr<Value const>> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     std::shared_ptr<Value const>((*request.values_.find(oid)).second));
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
  std::vector<std::pair<Oid,std::shared_ptr<Value const>> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     std::shared_ptr<Value const>((*request.values_.find(oid)).second));
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
  std::vector<std::pair<Oid,std::shared_ptr<Value const>> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     std::shared_ptr<Value const>((*request.values_.find(oid)).second));
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
  std::vector<std::pair<Oid,std::shared_ptr<Value const>> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     std::shared_ptr<Value const>((*request.values_.find(oid)).second));
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
  std::vector<std::pair<Oid,std::shared_ptr<Value const>> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     std::shared_ptr<Value const>((*request.values_.find(oid)).second));
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
  std::vector<std::pair<Oid,std::shared_ptr<Value const>> > vars;
  std::transform(paramOrder.begin(),
                 paramOrder.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     std::shared_ptr<Value const>((*request.values_.find(oid)).second));
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
  std::vector<std::pair<Oid,std::shared_ptr<Value const>> > vars;
  std::transform(request.oids_.begin(),
                 request.oids_.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     std::shared_ptr<Value const>(new xju::snmp::NullValue));
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
  std::vector<std::pair<Oid,std::shared_ptr<Value const>> > vars;
  std::transform(request.oids_.begin(),
                 request.oids_.end(),
                 std::back_inserter(vars),
                 [&](Oid const& oid) {
                   return std::make_pair(
                     oid,
                     std::shared_ptr<Value const>(new xju::snmp::NullValue));
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
                     std::shared_ptr<Value const>(new xju::snmp::NullValue));
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
                     std::shared_ptr<Value const>(new xju::snmp::NullValue));
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
                     std::shared_ptr<Value const>(new xju::snmp::NullValue));
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
                     std::shared_ptr<Value const>(new xju::snmp::NullValue));
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
                     std::shared_ptr<Value const>(new xju::snmp::NullValue));
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
