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
class SnmpV1GetRequest;
class NoSuchName;

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

}
}

#endif
