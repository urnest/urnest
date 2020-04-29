// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/validateResponse.hh>

#include <sstream>
#include <xju/snmp/SnmpV1SetRequest.hh>
#include <xju/snmp/SnmpV2cSetRequest.hh>
#include <xju/snmp/SnmpV1GetNextRequest.hh>
#include <xju/snmp/SnmpV1GetRequest.hh>
#include <xju/snmp/SnmpV2cGetRequest.hh>
#include <xju/format.hh>
#include <xju/functional.hh>
#include <xju/snmp/NullValue.hh>
#include <xju/snmp/SnmpV2cResponse.hh>
#include <xju/snmp/SnmpV2cGetNextRequest.hh>
#include <xju/snmp/SnmpV2cGetBulkRequest.hh>

namespace xju
{
namespace snmp
{

namespace
{
Oid getErrorOid(size_t const errorIndex,
                std::vector<Oid> const& responseOids,
                std::set<Oid> const& requestOids,
                std::string const& error) /*throw(
                  xju::Exception)*/
{
  if (errorIndex==0) {
    std::ostringstream s;
    s << "response specifies " << error << " error but error index does "
      << "not identify which oid was not found (index must be > 0)";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  auto const i(errorIndex-1);
  if (i >= responseOids.size()) {
    std::ostringstream s;
    s << "response specifies " << error << " error but error index"
      << " is beyond last oid in response";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  if (requestOids.find(responseOids[i])==requestOids.end()) {
    std::ostringstream s;
    s << "response error index indicates " << error << " for oid "
      << responseOids[i] << ", but that oid "
      << "was not requested";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return responseOids[i];
}

Oid getErrorOid(
  SnmpV1Response::ErrorIndex const errorIndex,
  std::vector<std::pair<Oid, std::shared_ptr<Value const> > > const& responseValues,
  std::set<Oid> const& requestOids,
  std::string const& error) /*throw(
    xju::Exception)*/
{
  std::vector<Oid> responseOids;
  std::transform(responseValues.begin(),responseValues.end(),
                 std::back_inserter(responseOids),
                 [](std::pair<Oid const, std::shared_ptr<Value const> > x) {
                   return x.first;
                 });
                 
  return getErrorOid(errorIndex.value(),
                     responseOids,
                     requestOids,
                     error);

}
Oid getErrorOid(
  SnmpV1Response::ErrorIndex const errorIndex,
  std::vector<std::pair<Oid, std::shared_ptr<Value const> > > const& responseValues,
  std::map<Oid, std::shared_ptr<Value const> > const& values,
  std::string const& error) /*throw(
    xju::Exception)*/
{
  std::vector<Oid> responseOids;
  std::set<Oid> requestOids;
                
  std::transform(responseValues.begin(),responseValues.end(),
                 std::back_inserter(responseOids),
                 [](std::pair<Oid const, std::shared_ptr<Value const> > x) {
                   return x.first;
                 });
                 
  std::transform(values.begin(),values.end(),
                 std::inserter(requestOids,requestOids.end()),
                 [](std::pair<Oid const, std::shared_ptr<Value const> > x) {
                   return x.first;
                 });
                 
  return getErrorOid(errorIndex.value(),
                     responseOids,
                     requestOids,
                     error);

}
Oid getErrorOid(SnmpV2cResponse::ErrorIndex const errorIndex,
                std::vector<SnmpV2cResponse::VarResult> const& varResults,
                std::set<Oid> const& requestOids,
                std::string const& error) /*throw(
                  xju::Exception)*/
{
  std::vector<Oid> responseOids;
                
  std::transform(varResults.begin(),varResults.end(),
                 std::back_inserter(responseOids),
                 [](SnmpV2cResponse::VarResult x) {
                   return x.oid_;
                 });
                 
  return getErrorOid(errorIndex.value(),
                     responseOids,
                     requestOids,
                     error);
  
}
Oid getErrorOid(SnmpV2cResponse::ErrorIndex const errorIndex,
                std::vector<SnmpV2cResponse::VarResult> const& varResults,
                std::map<Oid, std::shared_ptr<Value const> > const& values,
                std::string const& error) /*throw(
                  xju::Exception)*/
{
  std::set<Oid> requestOids;
                
  std::transform(values.begin(),values.end(),
                 std::inserter(requestOids,requestOids.end()),
                 [](std::pair<Oid const, std::shared_ptr<Value const> > x) {
                   return x.first;
                 });
                 
  return getErrorOid(errorIndex,
                     varResults,
                     requestOids,
                     error);
  
}
Oid getErrorOid(SnmpV2cResponse::ErrorIndex const errorIndex,
                std::vector<SnmpV2cResponse::VarResult> const& varResults,
                std::vector<Oid> const& oids,
                std::string const& error) /*throw(
                  xju::Exception)*/
{
  std::set<Oid> requestOids;
  std::copy(oids.begin(),oids.end(),
            std::inserter(requestOids,requestOids.end()));
                
  std::vector<Oid> resultOids;
  std::transform(varResults.begin(),varResults.end(),
                 std::back_inserter(resultOids),
                 [](decltype(*varResults.begin()) const& x){
                   return x.oid_;
                 });
                 
  return getErrorOid(errorIndex.value(),
                     resultOids,
                     requestOids,
                     error);
  
}

Oid getErrorOid(SnmpV2cResponse::ErrorIndex const errorIndex,
                std::vector<SnmpV2cResponse::VarResult> const& varResults,
                std::set<Oid> const& get,
                std::vector<Oid> const& getNextN,
                std::string const& error) /*throw(
                  xju::Exception)*/
{
  std::set<Oid> requestOids;
  std::copy(get.begin(),get.end(),
            std::inserter(requestOids,requestOids.end()));
  std::copy(getNextN.begin(),getNextN.end(),
            std::inserter(requestOids,requestOids.end()));
                
  std::vector<Oid> resultOids;
  std::transform(varResults.begin(),varResults.end(),
                 std::back_inserter(resultOids),
                 [](decltype(*varResults.begin()) const& x){
                   return x.oid_;
                 });
                 
  return getErrorOid(errorIndex.value(),
                     resultOids,
                     requestOids,
                     error);
  
}

}


std::map<Oid, std::shared_ptr<Value const> > validateResponse(
  SnmpV1GetRequest const& request,
  SnmpV1Response const& response) /*throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    NoSuchName,
    TooBig,
    GenErr,
    // response malformed eg not all requested oids present in response
    xju::Exception)*/
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
      throw NoSuchName(getErrorOid(response.errorIndex_,
                                   response.values_,
                                   request.oids_,
                                   "NoSuchName (2)"),
                       XJU_TRACED);
    }
    case SnmpV1Response::ErrorStatus::GEN_ERR:
    {
      throw GenErr(getErrorOid(response.errorIndex_,
                               response.values_,
                               request.oids_,
                               "GenErr (5)"),
                   XJU_TRACED);
    }
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
  SnmpV1Response const& response) /*throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    NoSuchName,
    BadValue,
    ReadOnly,
    TooBig,
    GenErr,
    xju::Exception)*/
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
      throw NoSuchName(getErrorOid(response.errorIndex_,
                                   response.values_,
                                   request.values_,
                                   "NoSuchName (2)"),
                       XJU_TRACED);
    }
    case SnmpV1Response::ErrorStatus::BAD_VALUE:
    {
      throw BadValue(getErrorOid(response.errorIndex_,
                                 response.values_,
                                 request.values_,
                                 "BadValue (3)"),
                     XJU_TRACED);
    }
    case SnmpV1Response::ErrorStatus::READ_ONLY:
    {
      throw ReadOnly(getErrorOid(response.errorIndex_,
                                 response.values_,
                                 request.values_,
                                 "ReadOnly (4)"),
                     XJU_TRACED);
    }
    case SnmpV1Response::ErrorStatus::GEN_ERR:
    {
      throw GenErr(getErrorOid(response.errorIndex_,
                               response.values_,
                               request.values_,
                               "GenErr (5)"),
                   XJU_TRACED);
    }
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
  SnmpV1Response const& response) /*throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    TooBig,
    GenErr,
    xju::Exception)*/
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
    {
      if (response.errorIndex_==SnmpV1Response::ErrorIndex(0)) {
        std::ostringstream s;
        s << "response specifies GenErr (0x05) error but error index does "
          << "not identify which oid was not found (index must be > 0)";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      auto const i(response.errorIndex_.value()-1);
      if (i >= response.values_.size()) {
        std::ostringstream s;
        s << "response specifies GenErr (0x05) error but error index"
          << " is beyond last oid in response";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      if (request.oids_.size()<=i) {
        std::ostringstream s;
        s << "response error index indicates that general error for oid "
          << "with index " << i << ", but not that many oids were requested";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      throw GenErr(request.oids_[i],XJU_TRACED);
    }
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


std::map<Oid, SnmpV2cVarResponse> validateResponse(
  SnmpV2cGetRequest const& request,
  SnmpV2cResponse const& response) /*throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    NoSuchName,
    TooBig,
    GenErr,
    xju::Exception)*/
{
  try {
    if (request.id_ != response.id_) {
      throw ResponseIdMismatch(response.id_,request.id_,XJU_TRACED);
    }
    if (response.responseType_!=0xA2) {
      throw ResponseTypeMismatch(response.responseType_,0xA2,XJU_TRACED);
    }
    switch(response.error_) {
    case SnmpV2cResponse::ErrorStatus::NO_ERROR: break;
    case SnmpV2cResponse::ErrorStatus::TOO_BIG: throw TooBig(XJU_TRACED);
    case SnmpV2cResponse::ErrorStatus::NO_SUCH_NAME:
    {
      throw NoSuchName(getErrorOid(response.errorIndex_,
                                   response.varResults_,
                                   request.oids_,
                                   "NoSuchName (2)"),
                       XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::GEN_ERR:
    {
      throw GenErr(getErrorOid(response.errorIndex_,
                               response.varResults_,
                               request.oids_,
                               "GenErr (5)"),
                   XJU_TRACED);
    }
    default:
    {
      std::ostringstream s;
      s << "response has unknown error status " << (int)response.error_;
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    }
    std::map<Oid, SnmpV2cVarResponse> result;
    std::transform(
      response.varResults_.begin(),
      response.varResults_.end(),
      std::inserter(result,result.end()),
      [](SnmpV2cResponse::VarResult x)
      {
        if (x.e_.valid()) {
          switch(x.e_.value()) {
          case SnmpV2cResponse::VarResult::NO_SUCH_OBJECT:
            return std::make_pair(
              x.oid_,SnmpV2cVarResponse(
                x.oid_,SnmpV2cVarResponse::NoSuchObject(x.oid_,XJU_TRACED)));
          case SnmpV2cResponse::VarResult::NO_SUCH_INSTANCE:
            return std::make_pair(
              x.oid_,SnmpV2cVarResponse(
                x.oid_,SnmpV2cVarResponse::NoSuchInstance(x.oid_,XJU_TRACED)));
          }
          std::ostringstream s;
          s << x.e_.value() << " (" <<(int)x.e_.value() << ")"
            << " (for oid " << x.oid_ << ") is not valid in response to "
            << "SNMP V2c Get request, only NO_SUCH_OBJECT ("
            << (int)SnmpV2cResponse::VarResult::NO_SUCH_OBJECT << ") and "
            << "NO_SUCH_INSTANCE (" 
            << (int)SnmpV2cResponse::VarResult::NO_SUCH_INSTANCE 
            << ") are valid";
          throw xju::Exception(s.str(),XJU_TRACED);
        }
        return std::make_pair(
          x.oid_,SnmpV2cVarResponse(
            x.oid_,x.v_));
      });
    
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
    s << "validate SNMP V2c response " << response 
      << " to SNMP V2c Get request " << request;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}


void validateResponse(
  SnmpV2cSetRequest const& request, 
  SnmpV2cResponse const& response) /*throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    TooBig,
    NoAccess,
    NotWritable,
    WrongType,
    WrongLength,
    WrongEncoding,
    WrongValue,
    NoSuchName,
    BadValue,
    NoCreation,
    InconsistentName,
    ReadOnly,
    InconsistentValue,
    ResourceUnavailable,
    CommitFailed,
    UndoFailed,
    GenErr,
    xju::Exception)*/
{
  try {
    if (request.id_ != response.id_) {
      throw ResponseIdMismatch(response.id_,request.id_,XJU_TRACED);
    }
    if (response.responseType_!=0xA2) {
      throw ResponseTypeMismatch(response.responseType_,0xA2,XJU_TRACED);
    }
    switch(response.error_) {
    case SnmpV2cResponse::ErrorStatus::NO_ERROR: break;
    case SnmpV2cResponse::ErrorStatus::TOO_BIG: throw TooBig(XJU_TRACED);
    case SnmpV2cResponse::ErrorStatus::NO_SUCH_NAME:
    {
      throw NoSuchName(getErrorOid(response.errorIndex_,
                                   response.varResults_,
                                   request.values_,
                                   "NoSuchName (2)"),
                       XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::BAD_VALUE:
    {
      throw BadValue(getErrorOid(response.errorIndex_,
                                 response.varResults_,
                                 request.values_,
                                 "BadValue (3)"),
                     XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::READ_ONLY:
    {
      throw ReadOnly(getErrorOid(response.errorIndex_,
                                 response.varResults_,
                                 request.values_,
                                 "ReadOnly (4)"),
                     XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::GEN_ERR:
    {
      throw GenErr(getErrorOid(response.errorIndex_,
                               response.varResults_,
                               request.values_,
                               "GenErr (5)"),
                   XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::NO_ACCESS:
    {
      throw NoAccess(getErrorOid(response.errorIndex_,
                                 response.varResults_,
                                 request.values_,
                                 "NoAccess (6)"),
                     XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::WRONG_TYPE:
    {
      throw WrongType(getErrorOid(response.errorIndex_,
                                  response.varResults_,
                                  request.values_,
                                  "WrongType (7)"),
                      XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::WRONG_LENGTH:
    {
      throw WrongLength(getErrorOid(response.errorIndex_,
                                      response.varResults_,
                                      request.values_,
                                      "WrongLength (8)"),
                        XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::WRONG_ENCODING:
    {
      throw WrongEncoding(getErrorOid(response.errorIndex_,
                                      response.varResults_,
                                      request.values_,
                                      "WrongEncoding (8)"),
                          XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::WRONG_VALUE:
    {
      throw WrongValue(getErrorOid(response.errorIndex_,
                                   response.varResults_,
                                   request.values_,
                                   "WrongValue (10)"),
                       XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::NO_CREATION:
    {
      throw NoCreation(getErrorOid(response.errorIndex_,
                                   response.varResults_,
                                   request.values_,
                                   "NoCreation (11)"),
                       XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::INCONSISTENT_VALUE:
    {
      throw InconsistentValue(getErrorOid(response.errorIndex_,
                                          response.varResults_,
                                          request.values_,
                                          "InconsistentValue (12)"),
                              XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::RESOURCE_UNAVAILABLE:
    {
      throw ResourceUnavailable(getErrorOid(response.errorIndex_,
                                            response.varResults_,
                                            request.values_,
                                            "ResourceUnavailable (11)"),
                                XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::COMMIT_FAILED: 
    {
      throw CommitFailed(getErrorOid(response.errorIndex_,
                                     response.varResults_,
                                     request.values_,
                                     "ResourceUnavailable (11)"),
                         XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::UNDO_FAILED:
    {
      throw UndoFailed(XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::NOT_WRITABLE:
    {
      throw NotWritable(getErrorOid(response.errorIndex_,
                                    response.varResults_,
                                    request.values_,
                                    "NoWritable (17)"),
                        XJU_TRACED);
    }
    case SnmpV2cResponse::ErrorStatus::INCONSISTENT_NAME:
    {
      throw InconsistentName(getErrorOid(response.errorIndex_,
                                         response.varResults_,
                                         request.values_,
                                         "InconsistentName (18)"),
                             XJU_TRACED);
    }
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
    std::transform(response.varResults_.begin(),
                   response.varResults_.end(),
                   std::inserter(responseOids,responseOids.end()),
                   [](const xju::snmp::SnmpV2cResponse::VarResult& x) {
                     return x.oid_;
                   });
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
    s << "validate response " << response 
      << " to SnmpV2cSetRequest " << request;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}


std::vector<SnmpV2cVarResponse> validateResponse(
  SnmpV2cGetNextRequest const& request,
  SnmpV2cResponse const& response) /*throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    TooBig,
    GenErr,
    xju::Exception)*/
{
  try {
    if (request.id_ != response.id_) {
      throw ResponseIdMismatch(response.id_,request.id_,XJU_TRACED);
    }
    if (response.responseType_!=0xA2) {
      throw ResponseTypeMismatch(response.responseType_,0xA2,XJU_TRACED);
    }
    switch(response.error_) {
    case SnmpV2cResponse::ErrorStatus::NO_ERROR: break;
    case SnmpV2cResponse::ErrorStatus::TOO_BIG: throw TooBig(XJU_TRACED);
    case SnmpV2cResponse::ErrorStatus::GEN_ERR:
    {
      throw GenErr(getErrorOid(response.errorIndex_,
                               response.varResults_,
                               request.oids_,
                               "GenErr (5)"),
                   XJU_TRACED);
    }
    default:
    {
      std::ostringstream s;
      s << "response has unknown error status " << (int)response.error_;
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    }
    std::vector<SnmpV2cVarResponse> result;
    std::transform(
      response.varResults_.begin(),
      response.varResults_.end(),
      std::back_inserter(result),
      [](SnmpV2cResponse::VarResult x)
      {
        if (x.e_.valid()) {
          switch(x.e_.value()) {
          case SnmpV2cResponse::VarResult::NO_SUCH_OBJECT:
            return SnmpV2cVarResponse(
              x.oid_,SnmpV2cVarResponse::NoSuchObject(x.oid_,XJU_TRACED));
          case SnmpV2cResponse::VarResult::NO_SUCH_INSTANCE:
            return SnmpV2cVarResponse(
              x.oid_,SnmpV2cVarResponse::NoSuchInstance(x.oid_,XJU_TRACED));
          case SnmpV2cResponse::VarResult::END_OF_MIB_VIEW:
            return SnmpV2cVarResponse(
              x.oid_,SnmpV2cVarResponse::EndOfMibView(x.oid_,XJU_TRACED));
          }
          std::ostringstream s;
          s << x.e_.value() << " (" <<(int)x.e_.value() << ")"
            << " (for oid " << x.oid_ << ") is not valid in response to "
            << "SNMP V2c Get request, only NO_SUCH_OBJECT ("
            << (int)SnmpV2cResponse::VarResult::NO_SUCH_OBJECT << "), "
            << "NO_SUCH_INSTANCE (" 
            << (int)SnmpV2cResponse::VarResult::NO_SUCH_INSTANCE 
            << ") and "
            << "END_OF_MIB_VIEW (" 
            << (int)SnmpV2cResponse::VarResult::END_OF_MIB_VIEW 
            << ") are valid";
          throw xju::Exception(s.str(),XJU_TRACED);
        }
        return SnmpV2cVarResponse(x.oid_,x.v_);
      });

    if (result.size()<request.oids_.size()){
      std::vector<std::string> missing;
      for(auto i=request.oids_.begin()+result.size();
          i!=request.oids_.end();
          ++i) {
        missing.push_back((*i).toString());
      }
      std::ostringstream s;
      s << "value(s) not reported for oid(s) "
        << xju::format::join(missing.begin(),missing.end(), ", ");
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    if (result.size()>request.oids_.size()){
      std::vector<std::string> extra;
      for(auto i=result.begin()+request.oids_.size();
          i!=result.end();
          ++i) {
        extra.push_back((*i).oid().toString());
      }
      std::ostringstream s;
      s << "values reported for unrequested oid(s) "
        << xju::format::join(extra.begin(),extra.end(), ", ");
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    return result;
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "validate SNMP V2c response " << response 
      << " to SNMP V2c GetNext request " << request;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

namespace
{
SnmpV2cVarResponse convertResponseVar(
  SnmpV2cResponse::VarResult const& x) /*throw(
    SnmpV2cVarResponse::NoSuchObject,
    SnmpV2cVarResponse::NoSuchInstance,
    SnmpV2cVarResponse::EndOfMibView,
    xju::Exception)*/
{
  if (x.e_.valid()) {
    switch(x.e_.value()) {
    case SnmpV2cResponse::VarResult::NO_SUCH_OBJECT:
      return SnmpV2cVarResponse(
        x.oid_,SnmpV2cVarResponse::NoSuchObject(x.oid_,XJU_TRACED));
    case SnmpV2cResponse::VarResult::NO_SUCH_INSTANCE:
      return SnmpV2cVarResponse(
        x.oid_,SnmpV2cVarResponse::NoSuchInstance(x.oid_,XJU_TRACED));
    case SnmpV2cResponse::VarResult::END_OF_MIB_VIEW:
      return SnmpV2cVarResponse(
        x.oid_,SnmpV2cVarResponse::EndOfMibView(x.oid_,XJU_TRACED));
    }
    std::ostringstream s;
    s << x.e_.value() << " (" <<(int)x.e_.value() << ")"
      << " (for oid " << x.oid_ << ") is not valid in response to "
      << "SNMP V2c Get request, only NO_SUCH_OBJECT ("
      << (int)SnmpV2cResponse::VarResult::NO_SUCH_OBJECT << "), "
      << "NO_SUCH_INSTANCE (" 
      << (int)SnmpV2cResponse::VarResult::NO_SUCH_INSTANCE 
      << ") and "
      << "END_OF_MIB_VIEW (" 
      << (int)SnmpV2cResponse::VarResult::END_OF_MIB_VIEW 
      << ") are valid";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return SnmpV2cVarResponse(x.oid_,x.v_);
}
}

std::pair<
  std::map<xju::snmp::Oid,SnmpV2cVarResponse>,
  std::vector<
    std::vector<SnmpV2cVarResponse> //row
    >
  > validateResponse(
    SnmpV2cGetBulkRequest const& request,
    SnmpV2cResponse const& response) /*throw(
      ResponseTypeMismatch,
      ResponseIdMismatch,
      TooBig,
      GenErr,
      xju::Exception)*/
{
  try {
    if (request.id_ != response.id_) {
      throw ResponseIdMismatch(response.id_,request.id_,XJU_TRACED);
    }
    if (response.responseType_!=0xA2) {
      throw ResponseTypeMismatch(response.responseType_,0xA2,XJU_TRACED);
    }
    switch(response.error_) {
    case SnmpV2cResponse::ErrorStatus::NO_ERROR: break;
    case SnmpV2cResponse::ErrorStatus::TOO_BIG: throw TooBig(XJU_TRACED);
    case SnmpV2cResponse::ErrorStatus::GEN_ERR:
    {
      throw GenErr(getErrorOid(response.errorIndex_,
                               response.varResults_,
                               request.get_,
                               request.getNextN_,
                               "GenErr (5)"),
                   XJU_TRACED);
    }
    default:
    {
      std::ostringstream s;
      s << "response has unknown error status " << (int)response.error_;
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    }
    if(response.varResults_.size()<request.get_.size()){
      std::ostringstream s;
      s << "response contains only " << response.varResults_.size()
        << " value(s), which is less than then number ("
        << request.get_.size() << ") of \"get\" oids requested";
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    std::map<xju::snmp::Oid,SnmpV2cVarResponse> values;
    auto i{request.get_.begin()};
    auto n{0};
    for(;i!=request.get_.end();++i,++n){
      if (*i != response.varResults_[n].oid_){
        std::ostringstream s;
        s << "expected oid " << (*i) << " as response var[" << n
          << "] but got oid " << response.varResults_[n].oid_;
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      values.insert({*i,convertResponseVar(response.varResults_[n])});
    }
    auto const rowSize{request.getNextN_.size()};
    if (rowSize==0 && n!=response.varResults_.size()){
      std::ostringstream s;
      s << "response contains " << (response.varResults_.size()-n)
        << " extra vars";
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    std::vector<
      std::vector<SnmpV2cVarResponse> //row
      > nextValues;
    for(; n!=response.varResults_.size(); n+=rowSize){
      if (response.varResults_.size()-n < rowSize){
        std::ostringstream s;
        s << "\"next values\" row "
          << (n-request.get_.size())/rowSize
          << " only has " << response.varResults_.size()-n
          << " values, which is less than the row size of "
          << rowSize;
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      nextValues.push_back(std::vector<SnmpV2cVarResponse>());
      transform(response.varResults_.begin()+n,
                response.varResults_.begin()+n+rowSize,
                std::back_inserter(nextValues.back()),
                convertResponseVar);
    }
    return std::make_pair(values,nextValues);
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "validate SNMP V2c response " << response 
      << " to SNMP V2c GetBulk request " << request;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}



}
}

