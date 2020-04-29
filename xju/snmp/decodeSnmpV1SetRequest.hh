// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_DECODESNMPV1SETREQUEST_H
#define XJU_SNMP_DECODESNMPV1SETREQUEST_H

#include "xju/snmp/SnmpV1SetRequest.hh"
#include <vector>
#include <stdint.h>
#include "xju/Exception.hh"
#include "xju/snmp/RequestTypeMismatch.hh"
#include "xju/snmp/SnmpVersionMismatch.hh"

namespace xju
{
namespace snmp
{
// returns the request and the requested parameters in order
// ie std::set(result.second.begin(),result.second.end())==
// keys of result.first.values_
// note that if the request encoded in data contained duplicate oids then
// result.second.size() != result.first.values_.size()
std::pair<SnmpV1SetRequest,std::vector<Oid> > decodeSnmpV1SetRequest(
  std::vector<uint8_t> const& data) /*throw(
    // not snmp v1
    SnmpVersionMismatch,
    // not a set request
    RequestTypeMismatch,
    // malformed
    xju::Exception)*/;

}
}

#endif
