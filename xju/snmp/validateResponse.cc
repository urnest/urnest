// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/snmp/validateResponse.h"
#include <sstream>

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
    case 0: break;
    case 0x01: throw TooBig(XJU_TRACED);
    case 0x02:
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
    case 0x05:
      throw GenErr(XJU_TRACED);
    default:
    {
      std::ostringstream s;
      s << "response has unknown error status " << response.error_;
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


}
}

