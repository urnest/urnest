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
#include "xju/snmp/SnmpV1Response.hh"
#include "xju/snmp/Value.hh"
#include "xju/snmp/SnmpV1GetRequest.hh"
#include <memory>
#include "xju/snmp/ResponseTypeMismatch.hh"
#include "xju/snmp/ResponseIdMismatch.hh"
#include "xju/snmp/NoSuchName.hh"
#include "xju/TooBig.hh"
#include "xju/snmp/GenErr.hh"
#include "xju/Exception.hh"

namespace xju
{
namespace snmp
{

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


}
}

#endif

