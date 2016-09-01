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
Client-side SNMP functions (plus SNMP Trap encoding).

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


Server-side SNMP functions:

  ... receive requestData as std::vector<uint8_t>
  std::vector<uint8_t> responseData(handleRequest(requestData));

  ... where:
  std::vector<uint8_t> handleRequest(std::vector<uint8_t> const& requestData)
    throw(xju::Exception)
  {
    try
    {
      return handleV1Request(requestData);
    }
    catch(xju::snmp::SnmpVersionMismatch& e1)
    {
      try
      {
        return handleV2cRequest(requestData);
      }
      catch(xju::snmp::SnmpVersionMismatch& e2)
      {
        std::ostringstream s;
        s << "request is not SNMP v1 (" << readableRepr(e1)
          << ") and request is not SNMP v2 (" << readableRepr(e2) << ")";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
    }
  }

  std::vector<uint8_t> handleV1Request(std::vector<uint8_t> const& requestData)
    throw(xju::Exception)
  {
    try {
      // handle snmp v1 get request
      std::pair<xju::snmp::SnmpV1GetRequest, std::vector<xju::snmp::Oid> > const request(
        xju::snmp::decodeSnmpV1GetRequest(requestData));
      verifyCommunity(request.community_);
      std::map<xju::snmp::Oid, std::shared_ptr<xju::snmp::Value const> > values(...);
      return xju::snmp::encodeResponse(request.first,request.second,values);
    }
    catch(xju::snmp::RequestTypeMismatch& e1) {
      try {
        // handle snmp v1 get next request
        xju::snmp::SnmpV1GetNextRequest const request(
          xju::snmp::decodeSnmpV1GetNextRequest(requestData));
        verifyCommunity(request.community_);
        std::vector<std::pair<xju::snmp::Oid, std::shared_ptr<xju::snmp::Value const> > > values(...);
        return xju::snmp::encodeResponse(request,values);
      }
      catch(xju::snmp::RequestTypeMismatch& e2) 
      {
        try {
          // handle snmp v1 set request
          std::pair<xju::snmp::SnmpV1SetRequest, std::vector<xju::snmp::Oid> > const request(
            xju::snmp::decodeSnmpV1SetRequest(requestData));
          try {
            verifyCommunity(request.community_);
            apply(request.values_);
            return xju::snmp::encodeResponse(request.first,request.second);
          }
          catch(NoSuchName& e) {
            return xju::snmp::encodeResponse(request.first,request.second.e);
          }
          ... translate other exceptions - see encodeResponse.hh
        }
        catch(xju::snmp::RequestTypeMismatch& e3)
        {
          std::ostringstream s;
          s << readableRepr(e1) << " and " << readableRepr(e2)
            << " and " << readableRepr(e3);
          throw xju::Exception(s.str(),XJU_TRACED);
        }
      }
    }
  }

  std::vector<uint8_t> handleV2cRequest(std::vector<uint8_t> const& requestData)
    throw(xju::Exception)
  {
    try {
      // handle snmp v2c get request
      std::pair<xju::snmp::SnmpV2cGetRequest, std::vector<xju::snmp::Oid> > const request(
        xju::snmp::decodeSnmpV2cGetRequest(requestData));
      verifyCommunity(request.community_);
      std::map<xju::snmp::Oid, std::shared_ptr<xju::snmp::Value const> > values(...);
      return xju::snmp::encodeResponse(request.first,request.second,values);
    }
    catch(xju::snmp::RequestTypeMismatch& e1) {
      try {
        // handle snmp v2c get next request
        xju::snmp::SnmpV2cGetNextRequest const request(
          xju::snmp::decodeSnmpV2cGetNextRequest(requestData));
        verifyCommunity(request.community_);
        std::vector<std::pair<xju::snmp::Oid, std::shared_ptr<xju::snmp::Value const> > > values(...);
        return xju::snmp::encodeResponse(request,values);
      }
      catch(xju::snmp::RequestTypeMismatch& e2) 
      {
        try {
          // handle snmp v2c set request
          std::pair<xju::snmp::SnmpV2cSetRequest, std::vector<xju::snmp::Oid> > const request(
            xju::snmp::decodeSnmpV2cSetRequest(requestData));
          try {
            verifyCommunity(request.community_);
            apply(request.values_);
            return xju::snmp::encodeResponse(request.first,request.second);
          catch(NoSuchName& e) {
            return xju::snmp::encodeResponse(request.first,request.second.e);
          }
          ... translate other exceptions - see encodeResponse.hh
        }
        catch(xju::snmp::RequestTypeMismatch& e3)
        {
          std::ostringstream s;
          s << readableRepr(e1) << " and " << readableRepr(e2)
            << " and " << readableRepr(e3);
          throw xju::Exception(s.str(),XJU_TRACED);
        }
      }
    }
  }


See snmp/README.txt also.

*/

// client-side headers
#include "snmp/SnmpV1GetRequest.hh"
#include "snmp/SnmpV1SetRequest.hh"
#include "snmp/SnmpV1GetNextRequest.hh"
#include "snmp/encode.hh"
#include "snmp/decodeSnmpV1Response.hh"
#include "snmp/validateResponse.hh"
#include "snmp/SnmpV1Table.hh"
#include "snmp/SnmpV1Trap.hh"

// server-side headers
#include "snmp/encodeResponse.hh"
#include "snmp/decodeSnmpV1GetRequest.hh"
#include "snmp/decodeSnmpV1GetNextRequest.hh"
#include "snmp/decodeSnmpV1SetRequest.hh"
#include "snmp/decodeSnmpV2cGetRequest.hh"
#include "snmp/decodeSnmpV2cGetNextRequest.hh"
#include "snmp/decodeSnmpV2cSetRequest.hh"

#endif
