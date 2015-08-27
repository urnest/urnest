// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_VALIDATERESPONSE_H
#define XJU_SNMP_VALIDATERESPONSE_H

#include <map>
#include "xju/snmp/Oid.hh"
#include "xju/snmp/Value.hh"
#include <memory>
#include "xju/snmp/ResponseTypeMismatch.hh"
#include "xju/snmp/ResponseIdMismatch.hh"
#include "xju/snmp/NoSuchName.hh"
#include "xju/snmp/TooBig.hh"
#include "xju/snmp/GenErr.hh"
#include "xju/Exception.hh"
#include "xju/snmp/ReadOnly.hh"
#include "xju/snmp/BadValue.hh"
#include "xju/snmp/SnmpV1Response.hh"
#include "xju/snmp/SnmpV2cVarResponse.hh"

namespace xju
{
namespace snmp
{
class SnmpV1Response;
class SnmpV1GetRequest;
class SnmpV1SetRequest;
class SnmpV1GetNextRequest;
class SnmpV2cGetRequest;
class SnmpV2cResponse;

// validate reponse to specified request
// post: *result[x] valid for all x in request.oids_
// - returns the requested values
std::map<Oid, std::shared_ptr<Value const> > validateResponse(
  SnmpV1GetRequest const& request,
  SnmpV1Response const& response) throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    NoSuchName,
    TooBig,
    GenErr,
    // response malformed eg not all requested oids present in response
    xju::Exception);

// validate reponse to specified request
// - note that RFC 1157 says that no values are modified if an error
//   is returned
void validateResponse(
  SnmpV1SetRequest const& request, 
  SnmpV1Response const& response) throw(
    // response.responseType_ != 0xA3
    ResponseTypeMismatch,
    // response.id_ != request.id_
    ResponseIdMismatch,
    // server does not know NoSuchName.oid_ 
    NoSuchName,
    // server forbids request.values_[BadValue.oid] as value of BadValue.oid
    BadValue,
    // ReadOnly.oid_ is read-only
    ReadOnly,
    // request was too big to process or responsd to
    // (note no values were set)
    TooBig,
    // SNMP General Error
    GenErr,
    // response malformed eg not all requested oids present in response
    xju::Exception);

// validate reponse to specified request
// - result suitable to pass to SnmpV1Table.add()
//   note: NoSuchName error is translated to a result with all its oids
//         set to 1.3, to signal end of table (see SnmpV1Table)
std::vector<std::pair<Oid, std::shared_ptr<Value const> > > validateResponse(
  SnmpV1GetNextRequest const& request,
  SnmpV1Response const& response) throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    TooBig,
    GenErr,
    // response malformed eg not all requested oids present in response
    xju::Exception);


// validate reponse to specified request
// post: result[x] for all x in request.oids_
// - returns the requested values
std::map<Oid, SnmpV2cVarResponse> validateResponse(
  SnmpV2cGetRequest const& request,
  SnmpV2cResponse const& response) throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    NoSuchName,
    TooBig,
    GenErr,
    // response malformed eg 
    // - not all requested oids present in response
    // - error code was other than those above (which are not explicitly
    //   associated with SnmpV2cGetRequest in RFC 1905)
    // - one or more var responses was END_OF_MIB_VIEW (which is not 
    //   explicitly associated with SnmpV2cGetRequest in RFC 1905)
    xju::Exception);


}
}

#endif

