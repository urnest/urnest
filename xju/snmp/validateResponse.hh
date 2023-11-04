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
#include <xju/snmp/Oid.hh>
#include <xju/snmp/Value.hh>
#include <memory>
#include <xju/snmp/ResponseTypeMismatch.hh>
#include <xju/snmp/ResponseIdMismatch.hh>
#include <xju/snmp/NoSuchName.hh>
#include <xju/snmp/TooBig.hh>
#include <xju/snmp/GenErr.hh>
#include <xju/Exception.hh>
#include <xju/snmp/ReadOnly.hh>
#include <xju/snmp/BadValue.hh>
#include <xju/snmp/SnmpV1Response.hh>
#include <xju/snmp/SnmpVar.hh>
#include <xju/snmp/NoAccess.hh>
#include <xju/snmp/NotWritable.hh>
#include <xju/snmp/WrongType.hh>
#include <xju/snmp/WrongLength.hh>
#include <xju/snmp/WrongEncoding.hh>
#include <xju/snmp/WrongValue.hh>
#include <xju/snmp/NoCreation.hh>
#include <xju/snmp/InconsistentName.hh>
#include <xju/snmp/InconsistentValue.hh>
#include <xju/snmp/ResourceUnavailable.hh>
#include <xju/snmp/CommitFailed.hh>
#include <xju/snmp/UndoFailed.hh>

namespace xju
{
namespace snmp
{
class SnmpV1Response;
class SnmpV1GetRequest;
class SnmpV1SetRequest;
class SnmpV1GetNextRequest;
class SnmpV2cGetRequest;
class SnmpV2cGetNextRequest;
class SnmpV2cSetRequest;
class SnmpV2cResponse;
class SnmpV2cGetBulkRequest;

// validate reponse to specified request
// post: *result[x] valid for all x in request.oids_
// - returns the requested values
std::map<Oid, std::shared_ptr<Value const> > validateResponse(
  SnmpV1GetRequest const& request,
  SnmpV1Response const& response) /*throw(
    // in priority order, eg if both type and id mismatch, ResponseTypeMismatch
    // is thrown
    ResponseTypeMismatch,
    ResponseIdMismatch,
    NoSuchName,
    TooBig,
    GenErr,
    // response malformed eg not all requested oids present in response
    xju::Exception)*/;

// validate reponse to specified request
// - note that RFC 1157 says that no values are modified if an error
//   is returned
void validateResponse(
  SnmpV1SetRequest const& request, 
  SnmpV1Response const& response) /*throw(
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
    xju::Exception)*/;

// validate reponse to specified request
// - result suitable to pass to SnmpV1Table.add()
//   note: NoSuchName error is translated to a result with all its oids
//         set to 1.3, to signal end of table (see SnmpV1Table)
std::vector<std::pair<Oid, std::shared_ptr<Value const> > > validateResponse(
  SnmpV1GetNextRequest const& request,
  SnmpV1Response const& response) /*throw(
    // in priority order, eg if both type and id mismatch, ResponseTypeMismatch
    // is thrown
    ResponseTypeMismatch,
    ResponseIdMismatch,
    TooBig,
    GenErr,
    // response malformed eg not all requested oids present in response
    xju::Exception)*/;


// validate reponse to specified request
// post: result[x] for all x in request.oids_
// - returns the requested values
std::map<Oid, SnmpVar> validateResponse(
  SnmpV2cGetRequest const& request,
  SnmpV2cResponse const& response) /*throw(
    // in priority order, eg if both type and id mismatch, ResponseTypeMismatch
    // is thrown
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
    xju::Exception)*/;

// validate reponse to specified request
// - note that RFC  says that no values are modified if an error
//   is returned
void validateResponse(
  SnmpV2cSetRequest const& request, 
  SnmpV2cResponse const& response) /*throw(
    // note: exceptions are listed in priority order, eg if both type and 
    // id mismatch, ResponseTypeMismatch is thrown
    //
    // response.responseType_ != 0xA3
    ResponseTypeMismatch,
    // response.id_ != request.id_
    ResponseIdMismatch,
    // request was too big to process or respond to
    // (note no values were set)
    TooBig,
    // server denies access to request.values_[NoAccess.oid_]
    NoAccess,
    // NotWritable.oid_ does not exist or is not writable
    NotWritable,
    // request.values_[WrongType.oid_] is not of a type suitable to set
    // WrongType.oid_
    WrongType,
    // request.values_[WrongLength.oid_] is not of a size suitable to set
    // WrongLength.oid_
    WrongLength,
    // request.values_[WrongEncoding.oid_] was incorrectly encoded
    WrongEncoding,
    // request.values_[WrongValue.oid_] could never be a valid value for
    // WrongValue.oid_
    WrongValue,
    // server does not know NoSuchName.oid_ 
    NoSuchName,
    // server forbids request.values_[BadValue.oid] as value of BadValue.oid_
    BadValue,
    // NoCreation.oid_ could never be created
    NoCreation,
    // InconsistentName.oid_ could not be created this time
    // (ie under present conditions, eg another variable has an incompatible
    //  setting)
    InconsistentName,
    // ReadOnly.oid_ is read-only (ie never writable)
    ReadOnly,
    // request.values_[InconsistentValue.oid_] is not a valid value
    // for InconsistentValue.oid_ this time
    InconsistentValue,
    // ResourceUnavailable.oid_ could not be set due to lack of resources
    ResourceUnavailable,
    // Request was valid but could not be applied (no changes have been made)
    CommitFailed,
    // Request was valid but could not be applied (some changes have been made)
    UndoFailed,
    // SNMP General Error
    GenErr,
    // response malformed eg not all requested oids present in response
    xju::Exception)*/;

// validate reponse to specified request
// - for each requested oid in order returns its successor oid and its value
//   or returns the requested oid with value endOfMibView
// - note that all result  variables might be endOfMibView, which must be
//   treated as end-of-sequence
std::vector<SnmpVar> validateResponse(
  SnmpV2cGetNextRequest const& request,
  SnmpV2cResponse const& response) /*throw(
    // in priority order, eg if both type and id mismatch, ResponseTypeMismatch
    // is thrown
    ResponseTypeMismatch,
    ResponseIdMismatch,
    TooBig,
    GenErr,
    // response malformed eg 
    // - not all requested oids present in response
    // - error code was other than those above (which are not explicitly
    //   associated with SnmpV2cGetNextRequest in RFC 1905)
    xju::Exception)*/;


// validate reponse to specified request
// - response.first has the values of "get" oids
// - response.second has repeater rows, each in order of request.getNextN_
// - note that all result  variables might be endOfMibView, which must be
//   treated as end-of-sequence
std::pair<
  std::map<xju::snmp::Oid,SnmpVar>,
  std::vector<
    std::vector<SnmpVar> //row
    >
  > validateResponse(
  SnmpV2cGetBulkRequest const& request,
  SnmpV2cResponse const& response) /*throw(
    // in priority order, eg if both type and id mismatch, ResponseTypeMismatch
    // is thrown
    ResponseTypeMismatch,
    ResponseIdMismatch,
    TooBig,
    GenErr,
    // response malformed eg 
    // - not all requested oids present in response
    // - error code was other than those above (which are not explicitly
    //   associated with SnmpV2cGetNextRequest in RFC 1905)
    xju::Exception)*/;


}
}

#endif
