// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_ENCODERESPONSE_H
#define XJU_SNMP_ENCODERESPONSE_H

#include <vector>
#include <stdint.h>
#include <xju/snmp/Oid.hh>
#include <map>
#include <memory>
#include <xju/snmp/Value.hh>

namespace xju
{
namespace snmp
{
class SnmpV1GetRequest; // see SnmpV1GetRequest.hh
class SnmpV1SetRequest; // see SnmpV1SetRequest.hh
class SnmpV1GetNextRequest; // see SnmpV1GetNextRequest.hh

class SnmpV2cGetRequest; // see SnmpV2cGetRequest.hh
class SnmpV2cSetRequest; // see SnmpV2cSetRequest.hh
class SnmpV2cGetNextRequest; // see SnmpV2cGetNextRequest.hh
class SnmpV2cVarResponse; // see SnmpV2cVarResponse.hh

class NoSuchName;          // see NoSuchName.hh
class TooBig;              // see TooBig.hh
class GenErr;              // see GenErr.hh
class BadValue;            // see BadValue.hh
class ReadOnly;            // see ReadOnly.hh
class NoAccess;            // see NoAccess.hh
class NotWritable;         // see NotWritable.hh
class WrongType;           // see WrongType.hh
class WrongLength;         // see WrongLength.hh
class WrongEncoding;       // see WrongEncoding.hh
class WrongValue;          // see WrongValue.hh
class NoCreation;          // see NoCreation.hh
class InconsistentName;    // see InconsistentName.hh
class InconsistentValue;   // see InconsistentValue.hh
class ResourceUnavailable; // see ResourceUnavailable.hh
class CommitFailed;        // see CommitFailed.hh
class UndoFailed;          // see UndoFailed.hh


// encode response with values for each of the oids in paramOrder, the
// value for paramOrder[i] being results[i], and with
// other info (community,id) copied from request
std::vector<uint8_t> encodeResponse(
  SnmpV1GetRequest const& request,
  std::vector<Oid> const& paramOrder,
  std::map<Oid,std::shared_ptr<xju::snmp::Value const> > const& results)
    throw();

// encode response indicating one of the requested Oids is unrecognised
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV1GetRequest const& request,
  std::vector<Oid> const& paramOrder,
  NoSuchName const& error) throw();

// encode response indicating response would have been too big to encode
std::vector<uint8_t> encodeResponse(
  SnmpV1GetRequest const& request,
  std::vector<Oid> const& paramOrder,
  TooBig const& error) throw();

// encode response indicating general server error related to
// error.param_
std::vector<uint8_t> encodeResponse(
  SnmpV1GetRequest const& request,
  std::vector<Oid> const& paramOrder,
  GenErr const& error) throw();

// encode response to successful snmp set request
std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder) throw();

// encode response indicating one of the requested Oids is unrecognised
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder,
  NoSuchName const& error) throw();

// encode response indicating one of the requested Oid values is unacceptable
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder,
  BadValue const& error) throw();

// encode response indicating one of the requested Oid values is read-only
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder,
  ReadOnly const& error) throw();

// encode response indicating response would have been too big to encode
std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder,
  TooBig const& error) throw();

// encode response indicating general server error related to
// error.param_
std::vector<uint8_t> encodeResponse(
  SnmpV1SetRequest const& request,
  std::vector<Oid> const& paramOrder,
  GenErr const& error) throw();

// encode response with values for each of the request oids' successors
// taken in order from results
// pre: results.size()==request.oids_.size()
std::vector<uint8_t> encodeResponse(
  SnmpV1GetNextRequest const& request,
  std::vector<std::pair<Oid,std::shared_ptr<xju::snmp::Value const> > > const& results)
    throw();

// encode response indicating response would have been too big to encode
// pre: error.param_ is one of request.oids_
std::vector<uint8_t> encodeResponse(
  SnmpV1GetNextRequest const& request,
  TooBig const& error) throw();

// encode response indicating general server error related to error.param_
// pre: error.param_ is one of request.oids_
std::vector<uint8_t> encodeResponse(
  SnmpV1GetNextRequest const& request,
  GenErr const& error) throw();

// encode response with values for each of the oids in paramOrder, the
// value for paramOrder[i] being results[i], and with
// other info (community,id) copied from request
std::vector<uint8_t> encodeResponse(
  SnmpV2cGetRequest const& request,
  std::vector<Oid> const& paramOrder,
  std::map<Oid,SnmpV2cVarResponse> const& results)
    throw();

// encode response indicating one of the requested Oids is unrecognised
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV2cGetRequest const& request,
  std::vector<Oid> const& paramOrder,
  NoSuchName const& error) throw();

// encode response indicating response would have been too big to encode
std::vector<uint8_t> encodeResponse(
  SnmpV2cGetRequest const& request,
  std::vector<Oid> const& paramOrder,
  TooBig const& error) throw();

// encode response indicating general server error related to
// error.param_
std::vector<uint8_t> encodeResponse(
  SnmpV2cGetRequest const& request,
  std::vector<Oid> const& paramOrder,
  GenErr const& error) throw();


// encode response to successful snmp set request
std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  std::vector<Oid> const& paramOrder) throw();

// encode response indicating response would have been too big to encode
std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  TooBig const& error) throw();

// encode response indicating variable error.param_ is not in the
// appropriate MIB ivew
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  std::vector<Oid> const& paramOrder,
  NoAccess const& error) throw();

// encode response indicating variable error.param_ is not writable
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  std::vector<Oid> const& paramOrder,
  NotWritable const& error) throw();

// encode response indicating value given for variable error.param_ is 
// of incorrect type
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  std::vector<Oid> const& paramOrder,
  WrongType const& error) throw();

// encode response indicating value given for variable error.param_ is
// of incorrect length
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  std::vector<Oid> const& paramOrder,
  WrongLength const& error) throw();

// encode response indicating value given for variable error.param_ is
// incorrectly encoded
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  std::vector<Oid> const& paramOrder,
  WrongEncoding const& error) throw();

// encode response indicating value given for variable error.param_ is not 
// a valid value for that variable
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  std::vector<Oid> const& paramOrder,
  WrongValue const& error) throw();

// encode response indicating variable error.param_ is can not ever
// be created
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  std::vector<Oid> const& paramOrder,
  NoCreation const& error) throw();

// encode response indicating variable error.param_  is not
// creatable under present conditions
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  std::vector<Oid> const& paramOrder,
  InconsistentName const& error) throw();

// encode response indicating value given for variable error.param_ is not
// valid under present conditions
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  std::vector<Oid> const& paramOrder,
  InconsistentValue const& error) throw();

// encode response indicating insufficient resources to modify
// variable error.param_
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  std::vector<Oid> const& paramOrder,
  ResourceUnavailable const& error) throw();

// encode response indicating general server error related to
// error.param_
std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  std::vector<Oid> const& paramOrder,
  GenErr const& error) throw();

// encode response indicating unexpected failure to commit var error.param_
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  std::vector<Oid> const& paramOrder,
  CommitFailed const& error) throw();

// encode response indicating commit failure that cannot be undone
// pre: error.param_ in paramOrder
std::vector<uint8_t> encodeResponse(
  SnmpV2cSetRequest const& request,
  std::vector<Oid> const& paramOrder,
  UndoFailed const& error) throw();

// note we cannot encode a AuthorizationError / AUTHORIZATION_ERROR
// response until some RFC defines how to encode it
// (see RFC 1901 section 3)

}
}
#endif
