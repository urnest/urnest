// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "validateResponse.hh"
#include <sstream>
#include "xju/snmp/SnmpV1SetRequest.hh"
#include "xju/snmp/SnmpV1GetNextRequest.hh"
#include "xju/snmp/SnmpV1GetRequest.hh"
#include <xju/format.hh>
#include "xju/functional.hh"
#include "xju/snmp/NullValue.hh"

namespace xju
{
namespace snmp
{

std::map<Oid, std::shared_ptr<Value const> > validateResponse(
  SnmpV1GetRequest const& request,
  SnmpV1Response const& response) throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    NoSuchName,
    TooBig,
    GenErr,
    // response malformed eg not all requested oids present in response
    xju::Exception)
{
  try {
    if (request.id_ != response.id_) {
      throw ResponseIdMismatch(response.id_,request.id_,XJU_TRACED);
    }
    if (response.responseType_!=0xA2) {
      throw ResponseTypeMismatch(response.responseType_,0xA2,XJU_TRACED);
    }
    switch(response.error_) {
    case SnmpV1Response::ErrorStatus::NO_ERROR: break;
    case SnmpV1Response::ErrorStatus::TOO_BIG: throw TooBig(XJU_TRACED);
    case SnmpV1Response::ErrorStatus::NO_SUCH_NAME:
    {
      if (response.errorIndex_==SnmpV1Response::ErrorIndex(0)) {
        std::ostringstream s;
        s << "response specifies NoSuchName (0x02) error but error index does "
          << "not identify which oid was not found (index must be > 0)";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      auto const i(response.errorIndex_.value()-1);
      if (i >= response.values_.size()) {
        std::ostringstream s;
        s << "response specifies NoSuchName (0x02) error but error index"
          << " is beyond last oid in response";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      if (request.oids_.find(response.values_[i].first)==request.oids_.end()) {
        std::ostringstream s;
        s << "response error index indicates that oid "
          << response.values_[i].first << " was not found, but that oid "
          << "was not requested?";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      throw NoSuchName(response.values_[i].first,XJU_TRACED);
    }
    case SnmpV1Response::ErrorStatus::GEN_ERR:
      throw GenErr(XJU_TRACED);
    default:
    {
      std::ostringstream s;
      s << "response has unknown error status " << (int)response.error_;
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    }
    std::map<Oid, std::shared_ptr<Value const> > result(
      response.values_.begin(),
      response.values_.end());
    std::vector<std::string> missing;
    for(auto i: request.oids_) {
      if (result.find(i)==result.end()) {
        missing.push_back((i).toString());
      }
    }
    if (missing.size()) {
      std::ostringstream s;
      s << "value not reported for oid(s) "
        << xju::format::join(missing.begin(),missing.end(), ", ");
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    return result;
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "validate response " << response << " to request " << request;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}


void validateResponse(
  SnmpV1SetRequest const& request, 
  SnmpV1Response const& response) throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    NoSuchName,
    BadValue,
    ReadOnly,
    TooBig,
    GenErr,
    xju::Exception)
{
  try {
    if (request.id_ != response.id_) {
      throw ResponseIdMismatch(response.id_,request.id_,XJU_TRACED);
    }
    if (response.responseType_!=0xA2) {
      throw ResponseTypeMismatch(response.responseType_,0xA2,XJU_TRACED);
    }
    switch(response.error_) {
    case SnmpV1Response::ErrorStatus::NO_ERROR: break;
    case SnmpV1Response::ErrorStatus::TOO_BIG: throw TooBig(XJU_TRACED);
    case SnmpV1Response::ErrorStatus::NO_SUCH_NAME:
    {
      if (response.errorIndex_==SnmpV1Response::ErrorIndex(0)) {
        std::ostringstream s;
        s << "response specifies NoSuchName (0x02) error but error index does "
          << "not identify which oid was not found (index must be > 0)";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      auto const i(response.errorIndex_.value()-1);
      if (i >= response.values_.size()) {
        std::ostringstream s;
        s << "response specifies NoSuchName (0x02) error but error index"
          << " is beyond last oid in response";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      if (request.values_.find(response.values_[i].first)==request.values_.end()) {
        std::ostringstream s;
        s << "response error index indicates that oid "
          << response.values_[i].first << " was not found, but that oid "
          << "was not requested?";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      throw NoSuchName(response.values_[i].first,XJU_TRACED);
    }
    case SnmpV1Response::ErrorStatus::BAD_VALUE:
    {
      if (response.errorIndex_==SnmpV1Response::ErrorIndex(0)) {
        std::ostringstream s;
        s << "response specifies BadValue (0x03) error but error index does "
          << "not identify which oid was not found (index must be > 0)";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      auto const i(response.errorIndex_.value()-1);
      if (i >= response.values_.size()) {
        std::ostringstream s;
        s << "response specifies BadValue (0x03) error but error index"
          << " is beyond last oid in response";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      if (request.values_.find(response.values_[i].first)==request.values_.end()) {
        std::ostringstream s;
        s << " specifies BadValue (0x03) error and response error index "
          << "indicates that oid "
          << response.values_[i].first 
          << " was not found, but that oid was not requested to be set?";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      throw BadValue(response.values_[i].first,XJU_TRACED);
    }
    case SnmpV1Response::ErrorStatus::READ_ONLY:
    {
      if (response.errorIndex_==SnmpV1Response::ErrorIndex(0)) {
        std::ostringstream s;
        s << "response specifies ReadOnly (0x04) error but error index does "
          << "not identify which oid was not found (index must be > 0)";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      auto const i(response.errorIndex_.value()-1);
      if (i >= response.values_.size()) {
        std::ostringstream s;
        s << "response specifies ReadOnly (0x04) error but error index"
          << " is beyond last oid in response";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      if (request.values_.find(response.values_[i].first)==request.values_.end()) {
        std::ostringstream s;
        s << "response error index indicates that oid "
          << response.values_[i].first << " was not found, but that oid "
          << "was not requested?";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      throw ReadOnly(response.values_[i].first,XJU_TRACED);
    }
    case SnmpV1Response::ErrorStatus::GEN_ERR:
      throw GenErr(XJU_TRACED);
    default:
    {
      std::ostringstream s;
      s << "response has unknown error status " << (int)response.error_;
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    }
    std::set<Oid> requestOids;
    std::transform(request.values_.begin(),
                   request.values_.end(),
                   std::inserter(requestOids,requestOids.end()),
                   xju::functional::First());
    std::set<Oid> responseOids;
    std::transform(response.values_.begin(),
                   response.values_.end(),
                   std::inserter(responseOids,responseOids.end()),
                   xju::functional::First());
    std::set<Oid> missing;
    std::set_difference(requestOids.begin(),requestOids.end(),
                        responseOids.begin(),responseOids.end(),
                        std::inserter(missing,missing.end()));
    std::set<Oid> extra;
    std::set_difference(responseOids.begin(),responseOids.end(),
                        requestOids.begin(),requestOids.end(),
                        std::inserter(extra,extra.end()));
    if (missing.size()||extra.size()) {
      std::vector<std::string> errors;
      if (missing.size()) {
        errors.push_back(
          "response did not return oids "+
          xju::format::join(missing.begin(),missing.end(),", "));
      }
      if (extra.size()) {
        errors.push_back(
          "response returned unrequested oids "+
          xju::format::join(extra.begin(),extra.end(),", "));
      }
      throw xju::Exception(
        xju::format::join(errors.begin(),errors.end()," and "),XJU_TRACED);
    }
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "validate response " << response << " to SnmpV1SetRequest " << request;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

std::vector<std::pair<Oid, std::shared_ptr<Value const> > > validateResponse(
  SnmpV1GetNextRequest const& request,
  SnmpV1Response const& response) throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    TooBig,
    GenErr,
    xju::Exception)
{
  try {
    if (request.id_ != response.id_) {
      throw ResponseIdMismatch(response.id_,request.id_,XJU_TRACED);
    }
    if (response.responseType_!=0xA2) {
      throw ResponseTypeMismatch(response.responseType_,0xA2,XJU_TRACED);
    }
    switch(response.error_) {
    case SnmpV1Response::ErrorStatus::NO_ERROR: break;
    case SnmpV1Response::ErrorStatus::TOO_BIG: throw TooBig(XJU_TRACED);
    case SnmpV1Response::ErrorStatus::NO_SUCH_NAME:
    {
      // translate NoSuchName to suit SnmpV1Table.add()
      // note that RFC 1157 leaves an ambiguity where traversal
      // reaches the end of the MIB - there are no subsequent
      // OIDs to return, so we end up with NoSuchName, but we can't
      // distinguish this from "requested oid not known to server"
      std::vector<std::pair<Oid, std::shared_ptr<Value const> > > result;
      std::transform(request.oids_.begin(),
                     request.oids_.end(),
                     std::inserter(result,result.end()),
                     [](Oid const& x) throw() {
                       return std::make_pair(
                         Oid(".1.3"),
                         std::shared_ptr<Value const>(
                           new NullValue));
                     });
      return result;
    }
    case SnmpV1Response::ErrorStatus::GEN_ERR:
      throw GenErr(XJU_TRACED);
    default:
    {
      std::ostringstream s;
      s << "response has unknown error status " << (int)response.error_;
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    }
    if (response.values_.size()<request.oids_.size()) {
      throw xju::Exception("response has less values than request",XJU_TRACED);
    }
    else if (response.values_.size()>request.oids_.size()) {
      throw xju::Exception("response has more values than request",XJU_TRACED);
    }
    return response.values_;
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "validate response " << response << " to SnmpV1GetNextRequest " 
      << request;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}



}
}

