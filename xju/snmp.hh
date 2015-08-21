// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_HH
#define XJU_SNMP_HH

/*
The intended use of this module:

snmp-get:
  xju::snmp::SnmpV1GetRequest request(...);
  std::vector<uint8_t> requestData(xju::snmp::encode(request));
  ... send requestData to server
  ... receive responseData as std::vector<uint8_t>
  std::map<xju::snmp::Oid, std::shared_ptr<xju::snmp::Value> > values(
    xju::snmp::validateResponse(
      request,xju::snmp::decodeSnmpV1Response(responseData)));
  ... use values

snmp-set:
  xju::snmp::SnmpV1SetRequest request(...);
  std::vector<uint8_t> requestData(xju::snmp::encode(request));
  ... send requestData to server
  ... receive responseData as std::vector<uint8_t>
  xju::snmp::validateResponse(
    request,xju::snmp::decodeSnmpV1Response(responseData));

snmp-get-next:
  xju::snmp::SnmpV1Table t{oid1,oid2,oid3};
  while(t.incomplete()) {
    xju::snmp::SnmpV1GetNextRequest request(..., t.nextOids());
    std::vector<uint8_t> requestData(xju::snmp::encode(request);
    ... send requestData to server
    ... receive responseData as std::vector<uint8_t>
    t.add(xju::snmp::validateResponse(
      request,xju::snmp::decodeSnmpV1Response(responseData));
  }
  for(auto i=0; i!=t.size(); ++i)
  {
    ... do something with t[oid1][i], t[oid2][i], t[oid3][i]
  }
 
snmp-trap:
  SnmpV1Trap t(...);
  std::vector<uint8_t> requestData(xju::snmp::encode(request));
  ... send requestData somewhere
*/

#include "snmp/SnmpV1GetRequest.hh"
#include "snmp/SnmpV1SetRequest.hh"
#include "snmp/SnmpV1GetNextRequest.hh"
#include "snmp/encode.hh"
#include "snmp/decodeSnmpV1Response.hh"
#include "snmp/validateResponse.hh"
#include "snmp/SnmpV1Table.hh"
#include "snmp/SnmpV1Trap.hh"


#endif
