// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/snmp/validateResponse.hh"

#include <iostream>
#include <xju/assert.hh>
#include "xju/snmp/StringValue.hh"
#include "xju/snmp/IntValue.hh"
#include "xju/snmp/NullValue.hh"
#include <set>
#include "xju/snmp/SnmpV1GetRequest.hh"
#include "xju/snmp/SnmpV1SetRequest.hh"
#include "xju/snmp/SnmpV1GetNextRequest.hh"
#include "xju/snmp/SnmpV2cSetRequest.hh"
#include "xju/snmp/SnmpV2cGetRequest.hh"
#include "xju/snmp/SnmpV2cResponse.hh"
#include <xju/snmp/SnmpV2cGetNextRequest.hh>
#include <xju/snmp/SnmpV2cGetBulkRequest.hh>

namespace xju
{
namespace snmp
{
std::vector<uint8_t> const fred{'f','r','e','d'};
std::vector<uint8_t> const jock{'j','o','c','k'};
std::vector<uint8_t> const sal{'s','a','l'};

void test1() throw()
{
  // validateResponse
  std::vector<std::pair<Oid, std::shared_ptr<Value const> > > values {
    {Oid(".1.3.3"), std::shared_ptr<Value const>(new StringValue(fred))},
    {Oid(".1.3.9.3333"),std::shared_ptr<Value const>(new IntValue(3))}
  };
  auto x=validateResponse(
    SnmpV1GetRequest(Community("dje"),
                     RequestId(23),
                     std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
    SnmpV1Response(0xA2,
                   Community("dd2"),
                   RequestId(23),
                   SnmpV1Response::ErrorStatus(0),
                   SnmpV1Response::ErrorIndex(0),
                   values));
    
  xju::assert_equal(x.size(),2U);
  xju::assert_equal(x[Oid(".1.3.3")]->stringValue(),fred);
  xju::assert_equal(x[Oid(".1.3.9.3333")]->intValue(),3);

  try {
    std::vector<std::pair<Oid, std::shared_ptr<Value const> > > values {
      {Oid(".1.3.3"), std::shared_ptr<Value const>{new StringValue(fred)}},
      {Oid(".1.3.9.3333"),std::shared_ptr<Value const>{new IntValue(3)}}
    };
    auto x=validateResponse(
      SnmpV1GetRequest(Community("dje"),
                       RequestId(23),
                       std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV1Response(0xA0,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(0),
                     SnmpV1Response::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(ResponseTypeMismatch const& e) {
    xju::assert_equal(e.got_,0xa0);
    xju::assert_equal(e.expected_,0xa2);
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa0, community dd2, id 23, error status 0, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3 to request community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nexpected response of type 0xa2 but got response of type 0xa0.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  
  try {
    std::vector<std::pair<Oid, std::shared_ptr<Value const> > > values {
      {Oid(".1.3.3"), std::shared_ptr<Value const>{new StringValue(fred)}},
      {Oid(".1.3.9.3333"),std::shared_ptr<Value const>{new IntValue(3)}}
    };
    auto x=validateResponse(
      SnmpV1GetRequest(Community("dje"),
                       RequestId(23),
                       std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(24),
                     SnmpV1Response::ErrorStatus(0),
                     SnmpV1Response::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(ResponseIdMismatch const& e) {
    xju::assert_equal(e.got_,RequestId(24));
    xju::assert_equal(e.expected_,RequestId(23));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 24, error status 0, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3 to request community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nexpected response with id 23 but got response of id 24.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<std::pair<Oid, std::shared_ptr<Value const> > > values {
      {Oid(".1.3.3"), std::shared_ptr<Value const>{new StringValue(fred)}},
      {Oid(".1.3.9.3333"),std::shared_ptr<Value const>{new NullValue}}
    };
    auto x=validateResponse(
      SnmpV1GetRequest(Community("dje"),
                       RequestId(23),
                       std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(2),
                     SnmpV1Response::ErrorIndex(2),
                     values));
    
    xju::assert_never_reached();
  }
  catch(NoSuchName const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 2, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: null to request community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nserver has no object with oid .1.3.9.3333.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<std::pair<Oid, std::shared_ptr<Value const> > > values {
    };
    auto x=validateResponse(
      SnmpV1GetRequest(Community("dje"),
                       RequestId(23),
                       std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(1),
                     SnmpV1Response::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(TooBig const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 1, error index 0, values  to request community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nSNMP response would have exceeded server internal limit.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<std::pair<Oid, std::shared_ptr<Value const> > > values {
      {Oid(".1.3.3"), std::shared_ptr<Value const>{new StringValue(fred)}},
      {Oid(".1.3.9.3333"),std::shared_ptr<Value const>{new NullValue}}
    };
    auto x=validateResponse(
      SnmpV1GetRequest(Community("dje"),
                       RequestId(23),
                       std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(5),
                     SnmpV1Response::ErrorIndex(1),
                     values));
    
    xju::assert_never_reached();
  }
  catch(GenErr const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 5, error index 1, values .1.3.3: \"fred\", .1.3.9.3333: null to request community dje, id 23, oids .1.3.3, .1.3.9.3333 because\ngeneral error for oid .1.3.3.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<std::pair<Oid, std::shared_ptr<Value const> > > values {
    };
    auto x=validateResponse(
      SnmpV1GetRequest(Community("dje"),
                       RequestId(23),
                       std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(0),
                     SnmpV1Response::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 0, error index 0, values  to request community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nvalue not reported for oid(s) .1.3.3, .1.3.9.3333.");
  }
}

void test2() throw()
{
  // validateResponse
  std::vector<std::pair<Oid, std::shared_ptr<Value const> > > values {
    {Oid(".1.3.3"), std::shared_ptr<Value const>{new StringValue(fred)}},
    {Oid(".1.3.9.3333"),std::shared_ptr<Value const>{new IntValue(3)}}
  };
  std::map<Oid, std::shared_ptr<Value const> > requestValues(
    values.begin(),values.end());
  
  validateResponse(
    SnmpV1SetRequest(Community("dje"),
                     RequestId(23),
                     requestValues),
    SnmpV1Response(0xA2,
                   Community("dd2"),
                   RequestId(23),
                   SnmpV1Response::ErrorStatus(0),
                   SnmpV1Response::ErrorIndex(0),
                   values));
    
  try {
    validateResponse(
      SnmpV1SetRequest(Community("dje"),
                       RequestId(23),
                       requestValues),
      SnmpV1Response(0xA0,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(0),
                     SnmpV1Response::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(ResponseTypeMismatch const& e) {
    xju::assert_equal(e.got_,0xa0);
    xju::assert_equal(e.expected_,0xa2);
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa0, community dd2, id 23, error status 0, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV1SetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nexpected response of type 0xa2 but got response of type 0xa0.");
  }

  try {
    validateResponse(
      SnmpV1SetRequest(Community("dje"),
                       RequestId(23),
                       requestValues),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(24),
                     SnmpV1Response::ErrorStatus(0),
                     SnmpV1Response::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(ResponseIdMismatch const& e) {
    xju::assert_equal(e.got_,RequestId(24));
    xju::assert_equal(e.expected_,RequestId(23));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 24, error status 0, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV1SetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nexpected response with id 23 but got response of id 24.");
  }

  try {
    validateResponse(
      SnmpV1SetRequest(Community("dje"),
                       RequestId(23),
                       requestValues),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(2),
                     SnmpV1Response::ErrorIndex(2),
                     values));
    
    xju::assert_never_reached();
  }
  catch(NoSuchName const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 2, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV1SetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nserver has no object with oid .1.3.9.3333.");
  }

  try {
    validateResponse(
      SnmpV1SetRequest(Community("dje"),
                       RequestId(23),
                       requestValues),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(3),
                     SnmpV1Response::ErrorIndex(2),
                     values));
    
    xju::assert_never_reached();
  }
  catch(BadValue const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 3, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV1SetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nvalue of .1.3.9.3333 is invalid.");
  }

  try {
    validateResponse(
      SnmpV1SetRequest(Community("dje"),
                       RequestId(23),
                       requestValues),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(4),
                     SnmpV1Response::ErrorIndex(2),
                     values));
    
    xju::assert_never_reached();
  }
  catch(ReadOnly const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 4, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV1SetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nobject oid .1.3.9.3333 is read-only.");
  }

  try {
    validateResponse(
      SnmpV1SetRequest(Community("dje"),
                       RequestId(23),
                       requestValues),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(1),
                     SnmpV1Response::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(TooBig const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 1, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV1SetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nSNMP response would have exceeded server internal limit.");
  }

  try {
    validateResponse(
      SnmpV1SetRequest(Community("dje"),
                       RequestId(23),
                       requestValues),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(5),
                     SnmpV1Response::ErrorIndex(1),
                     values));
    
    xju::assert_never_reached();
  }
  catch(GenErr const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 5, error index 1, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV1SetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\ngeneral error for oid .1.3.3.");
  }
  try {
    std::vector<std::pair<Oid, std::shared_ptr<Value const> > > values {
    };
    validateResponse(
      SnmpV1SetRequest(Community("dje"),
                       RequestId(23),
                       requestValues),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(0),
                     SnmpV1Response::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 0, error index 0, values  to SnmpV1SetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nresponse did not return oids .1.3.3, .1.3.9.3333.");
  }
  try {
    std::vector<std::pair<Oid, std::shared_ptr<Value const> > > values {
      {Oid(".1.3.4"), std::shared_ptr<Value const>{new StringValue(fred)}},
    };
    validateResponse(
      SnmpV1SetRequest(Community("dje"),
                       RequestId(23),
                       requestValues),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(0),
                     SnmpV1Response::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 0, error index 0, values .1.3.4: \"fred\" to SnmpV1SetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nresponse did not return oids .1.3.3, .1.3.9.3333 and response returned unrequested oids .1.3.4.");
  }
}

void test3() throw()
{
  // validateResponse(SnmpV1GetNextRequest,SnmpV1Response)

  std::vector<std::pair<Oid, std::shared_ptr<Value const> > > values {
    {Oid(".1.3.3"), std::shared_ptr<Value const>{new StringValue(fred)}},
    {Oid(".1.3.9.3333"),std::shared_ptr<Value const>{new IntValue(3)}}
  };
  std::vector<std::pair<Oid, std::shared_ptr<Value const> > > nextValues {
    {Oid(".1.3.4"), std::shared_ptr<Value const>{new StringValue(jock)}},
    {Oid(".1.3.9.3334"),std::shared_ptr<Value const>{new IntValue(5)}}
  };
  try
  {
    auto x=validateResponse(
      SnmpV1GetNextRequest(Community("dje"),
                           RequestId(23),
                           std::vector<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(0),
                     SnmpV1Response::ErrorIndex(0),
                     nextValues));
  
    xju::assert_equal(x.size(),2U);
    xju::assert_equal(x[0].first,Oid(".1.3.4"));
    xju::assert_equal(x[0].second->stringValue(),jock);
    xju::assert_equal(x[1].first,Oid(".1.3.9.3334"));
    xju::assert_equal(x[1].second->intValue(),5);
  }
  catch(xju::Exception const& e)
  {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  
  try {
    auto x=validateResponse(
      SnmpV1GetNextRequest(Community("dje"),
                       RequestId(23),
                       std::vector<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV1Response(0xA0,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(0),
                     SnmpV1Response::ErrorIndex(0),
                     nextValues));
    
    xju::assert_never_reached();
  }
  catch(ResponseTypeMismatch const& e) {
    xju::assert_equal(e.got_,0xa0);
    xju::assert_equal(e.expected_,0xa2);
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa0, community dd2, id 23, error status 0, error index 0, values .1.3.4: \"jock\", .1.3.9.3334: 5 to SnmpV1GetNextRequest community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nexpected response of type 0xa2 but got response of type 0xa0.");
  }

  try {
    auto x=validateResponse(
      SnmpV1GetNextRequest(Community("dje"),
                       RequestId(23),
                       std::vector<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(24),
                     SnmpV1Response::ErrorStatus(0),
                     SnmpV1Response::ErrorIndex(0),
                     nextValues));
    
    xju::assert_never_reached();
  }
  catch(ResponseIdMismatch const& e) {
    xju::assert_equal(e.got_,RequestId(24));
    xju::assert_equal(e.expected_,RequestId(23));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 24, error status 0, error index 0, values .1.3.4: \"jock\", .1.3.9.3334: 5 to SnmpV1GetNextRequest community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nexpected response with id 23 but got response of id 24.");
  }

  try {
    auto x=validateResponse(
      SnmpV1GetNextRequest(Community("dje"),
                       RequestId(23),
                       std::vector<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(2),
                     SnmpV1Response::ErrorIndex(2),
                     nextValues));

    xju::assert_equal(x.size(),2U);
    xju::assert_equal(x[0].first,Oid(".1.3"));
    xju::assert_equal(x[1].first,Oid(".1.3"));
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));//never reached
  }

  try {
    auto x=validateResponse(
      SnmpV1GetNextRequest(Community("dje"),
                       RequestId(23),
                       std::vector<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(1),
                     SnmpV1Response::ErrorIndex(0),
                     nextValues));
    
    xju::assert_never_reached();
  }
  catch(TooBig const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 1, error index 0, values .1.3.4: \"jock\", .1.3.9.3334: 5 to SnmpV1GetNextRequest community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nSNMP response would have exceeded server internal limit.");
  }

  try {
    auto x=validateResponse(
      SnmpV1GetNextRequest(Community("dje"),
                       RequestId(23),
                       std::vector<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(5),
                     SnmpV1Response::ErrorIndex(1),
                     nextValues));
    
    xju::assert_never_reached();
  }
  catch(GenErr const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 5, error index 1, values .1.3.4: \"jock\", .1.3.9.3334: 5 to SnmpV1GetNextRequest community dje, id 23, oids .1.3.3, .1.3.9.3333 because\ngeneral error for oid .1.3.3.");
  }
  try {
    std::vector<std::pair<Oid, std::shared_ptr<Value const> > > nextValues {
    };
    auto x=validateResponse(
      SnmpV1GetNextRequest(Community("dje"),
                       RequestId(23),
                       std::vector<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(0),
                     SnmpV1Response::ErrorIndex(0),
                     nextValues));
    
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 0, error index 0, values  to SnmpV1GetNextRequest community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nresponse has less values than request.");
  }
  try {
    std::vector<std::pair<Oid, std::shared_ptr<Value const> > > nextValues {
      {Oid(".1.3.4"), std::shared_ptr<Value const>{new StringValue(jock)}},
      {Oid(".1.3.4.1"), std::shared_ptr<Value const>{new StringValue(sal)}},
      {Oid(".1.3.9.3334"),std::shared_ptr<Value const>{new IntValue(5)}}
    };
    auto x=validateResponse(
      SnmpV1GetNextRequest(Community("dje"),
                       RequestId(23),
                       std::vector<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV1Response(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV1Response::ErrorStatus(0),
                     SnmpV1Response::ErrorIndex(0),
                     nextValues));
    
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 0, error index 0, values .1.3.4: \"jock\", .1.3.4.1: \"sal\", .1.3.9.3334: 5 to SnmpV1GetNextRequest community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nresponse has more values than request.");
  }
}

void test4() throw()
{
  // validateResponse
  std::vector<SnmpVar> values {
    SnmpVar(Oid(".1.3.3"),std::shared_ptr<Value const>(
                                 new StringValue(fred))),
    SnmpVar(Oid(".1.3.9.3333"),std::shared_ptr<Value const>(
                                 new IntValue(3)))
  };
  auto x=validateResponse(
    SnmpV2cGetRequest(Community("dje"),
                     RequestId(23),
                     std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
    SnmpV2cResponse(0xA2,
                   Community("dd2"),
                   RequestId(23),
                   SnmpV2cResponse::ErrorStatus(0),
                   SnmpV2cResponse::ErrorIndex(0),
                   values));
    
  xju::assert_equal(x.size(),2U);
  xju::assert_equal((*x.find(Oid(".1.3.3"))).second->stringValue(),fred);
  xju::assert_equal((*x.find(Oid(".1.3.9.3333"))).second->intValue(),3);

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),std::shared_ptr<Value const>(
                                   new StringValue(fred))),
        SnmpVar(Oid(".1.3.9.3333"),std::shared_ptr<Value const>(
                                     new IntValue(3)))
        };
    auto x=validateResponse(
      SnmpV2cGetRequest(Community("dje"),
                       RequestId(23),
                       std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV2cResponse(0xA0,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(0),
                     SnmpV2cResponse::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(ResponseTypeMismatch const& e) {
    xju::assert_equal(e.got_,0xa0);
    xju::assert_equal(e.expected_,0xa2);
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa0, community dd2, id 23, error status 0, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SNMP V2c Get request community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nexpected response of type 0xa2 but got response of type 0xa0.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  
  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),std::shared_ptr<Value const>(
                                                   new StringValue(fred))),
        SnmpVar(Oid(".1.3.9.3333"),std::shared_ptr<Value const>(
                                                       new IntValue(3)))
    };
    auto x=validateResponse(
      SnmpV2cGetRequest(Community("dje"),
                       RequestId(23),
                       std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(24),
                     SnmpV2cResponse::ErrorStatus(0),
                     SnmpV2cResponse::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(ResponseIdMismatch const& e) {
    xju::assert_equal(e.got_,RequestId(24));
    xju::assert_equal(e.expected_,RequestId(23));
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 24, error status 0, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SNMP V2c Get request community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nexpected response with id 23 but got response of id 24.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),std::shared_ptr<Value const>(
                                                   new StringValue(fred))),
        SnmpVar(Oid(".1.3.9.3333"),std::shared_ptr<Value const>(
                                                       new NullValue))
    };
    auto x=validateResponse(
      SnmpV2cGetRequest(Community("dje"),
                       RequestId(23),
                       std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(2),
                     SnmpV2cResponse::ErrorIndex(2),
                     values));
    
    xju::assert_never_reached();
  }
  catch(NoSuchName const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 23, error status 2, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: null to SNMP V2c Get request community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nserver has no object with oid .1.3.9.3333.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<SnmpVar> values {
    };
    auto x=validateResponse(
      SnmpV2cGetRequest(Community("dje"),
                       RequestId(23),
                       std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(1),
                     SnmpV2cResponse::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(TooBig const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 23, error status 1, error index 0, values  to SNMP V2c Get request community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nSNMP response would have exceeded server internal limit.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"), std::shared_ptr<Value const>(
                                   new StringValue(fred))),
        SnmpVar(Oid(".1.3.9.3333"), std::shared_ptr<Value const>(
                                     new NullValue))
        };
    auto x=validateResponse(
      SnmpV2cGetRequest(Community("dje"),
                       RequestId(23),
                       std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(5),
                     SnmpV2cResponse::ErrorIndex(1),
                     values));
    
    xju::assert_never_reached();
  }
  catch(GenErr const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 23, error status 5, error index 1, values .1.3.3: \"fred\", .1.3.9.3333: null to SNMP V2c Get request community dje, id 23, oids .1.3.3, .1.3.9.3333 because\ngeneral error for oid .1.3.3.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<SnmpVar> values {
    };
    auto x=validateResponse(
      SnmpV2cGetRequest(Community("dje"),
                       RequestId(23),
                       std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(0),
                     SnmpV2cResponse::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 23, error status 0, error index 0, values  to SNMP V2c Get request community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nvalue not reported for oid(s) .1.3.3, .1.3.9.3333.");
  }

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),std::shared_ptr<Value const>(
                                                 new StringValue(fred))),
      SnmpVar(
        Oid(".1.3.9.3333"),
        SnmpVar::NoSuchObject(Oid(".1.3.9.3333"),XJU_TRACED))
      };
    auto x=validateResponse(
      SnmpV2cGetRequest(Community("dje"),
                       RequestId(23),
                       std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(0),
                     SnmpV2cResponse::ErrorIndex(0),
                     values));
    
    xju::assert_equal(x.size(),2U);
    xju::assert_equal((*x.find(Oid(".1.3.3"))).second->stringValue(),fred);
    try {
      (*x.find(Oid(".1.3.9.3333"))).second->intValue();
      xju::assert_never_reached();
    }
    catch(SnmpVar::NoSuchObject const& e) {
      xju::assert_equal(readableRepr(e),"no such object .1.3.9.3333.");
    }
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),std::shared_ptr<Value const>(
                                                 new StringValue(fred))),
      SnmpVar(
        Oid(".1.3.9.3333"),
        SnmpVar::NoSuchInstance(Oid(".1.3.9.3333"),XJU_TRACED))
      };
    auto x=validateResponse(
      SnmpV2cGetRequest(Community("dje"),
                       RequestId(23),
                       std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(0),
                     SnmpV2cResponse::ErrorIndex(0),
                     values));
    
    xju::assert_equal(x.size(),2U);
    xju::assert_equal((*x.find(Oid(".1.3.3"))).second->stringValue(),fred);
    try {
      (*x.find(Oid(".1.3.9.3333"))).second->intValue();
      xju::assert_never_reached();
    }
    catch(SnmpVar::NoSuchInstance const& e) {
      xju::assert_equal(readableRepr(e),"no such instance .1.3.9.3333.");
    }
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),std::shared_ptr<Value const>(
                                                 new StringValue(fred))),
      SnmpVar(
        Oid(".1.3.9.3333"),
        SnmpVar::EndOfMibView(Oid(".1.3.9.3333"),XJU_TRACED))
      };
    auto x=validateResponse(
      SnmpV2cGetRequest(Community("dje"),
                       RequestId(23),
                       std::set<Oid>({Oid(".1.3.3"),Oid(".1.3.9.3333")})),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(0),
                     SnmpV2cResponse::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 23, error status 0, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: end of MIB view .1.3.9.3333. to SNMP V2c Get request community dje, id 23, oids .1.3.3, .1.3.9.3333 because\nfailed to verify value .1.3.9.3333: end of MIB view .1.3.9.3333. is not end-of-mib-view because\nend of MIB view .1.3.9.3333.");
  }
}

void test5() throw()
{
  // validateResponse(SnmpV2cSetRequest)
  std::vector<SnmpVar> values {
    SnmpVar(Oid(".1.3.3"),std::shared_ptr<Value const>(
                                                 new StringValue(fred))),
      SnmpVar(Oid(".1.3.9.3333"),std::shared_ptr<Value const>(
                                                     new IntValue(3)))
  };
  std::map<Oid, std::shared_ptr<Value const> > requestValues {
    std::make_pair(Oid(".1.3.3"), std::shared_ptr<Value const>(
                     new StringValue(fred))),
      std::make_pair(Oid(".1.3.9.3333"),std::shared_ptr<Value const>(
                       new IntValue(3)))
  };
  
  validateResponse(
    SnmpV2cSetRequest(Community("dje"),
                     RequestId(23),
                     requestValues),
    SnmpV2cResponse(0xA2,
                    Community("dd2"),
                    RequestId(23),
                    SnmpV2cResponse::ErrorStatus(0),
                    SnmpV2cResponse::ErrorIndex(0),
                    values));
    
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA0,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(0),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    
    xju::assert_never_reached();
  }
  catch(ResponseTypeMismatch const& e) {
    xju::assert_equal(e.got_,0xa0);
    xju::assert_equal(e.expected_,0xa2);
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa0, community dd2, id 23, error status 0, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nexpected response of type 0xa2 but got response of type 0xa0.");
  }

  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(24),
                      SnmpV2cResponse::ErrorStatus(0),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    
    xju::assert_never_reached();
  }
  catch(ResponseIdMismatch const& e) {
    xju::assert_equal(e.got_,RequestId(24));
    xju::assert_equal(e.expected_,RequestId(23));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 24, error status 0, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nexpected response with id 23 but got response of id 24.");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(2),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(NoSuchName const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 2, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nserver has no object with oid .1.3.9.3333.");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(3),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(BadValue const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 3, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nvalue of .1.3.9.3333 is invalid.");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(4),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(ReadOnly const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 4, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nobject oid .1.3.9.3333 is read-only.");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(1),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    
    xju::assert_never_reached();
  }
  catch(TooBig const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 1, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nSNMP response would have exceeded server internal limit.");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(5),
                      SnmpV2cResponse::ErrorIndex(1),
                      values));
    
    xju::assert_never_reached();
  }
  catch(GenErr const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 5, error index 1, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\ngeneral error for oid .1.3.3.");
  }
  try {
    std::vector<SnmpVar> values {
    };
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(0),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 0, error index 0, values  to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nresponse did not return oids .1.3.3, .1.3.9.3333.");
  }
  try {
    std::vector<SnmpVar> values {
      SnmpVar(
        Oid(".1.3.4"), std::shared_ptr<Value const>(new StringValue(fred)))
        };
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(0),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 0, error index 0, values .1.3.4: \"fred\" to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nresponse did not return oids .1.3.3, .1.3.9.3333 and response returned unrequested oids .1.3.4.");
  }

  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(6),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(NoAccess const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 6, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nserver denies write access to object .1.3.9.3333.");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(17),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(NotWritable const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 17, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nserver has no object .1.3.9.3333 or that object is not writable.");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(7),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(WrongType const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 7, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\ntype of value specified for object .1.3.9.3333 is unsuitable.");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(8),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(WrongLength const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 8, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nlength ie size of value specified for object .1.3.9.3333 was unsuitable.");
  }

  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(9),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(WrongEncoding const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 9, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nvalue specified for object .1.3.9.3333 was incorrectly encoded.");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(10),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(WrongValue const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 10, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nvalue specified for object .1.3.9.3333 could never be valid for that object.");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(11),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(NoCreation const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 11, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nobject .1.3.9.3333 could never be created.");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(18),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(InconsistentName const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 18, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nobject .1.3.9.3333 could not be created this time eg another variable has an incompatible value.");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(12),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(InconsistentValue const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 12, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nvalue specified for object .1.3.9.3333 was not valid this time.");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(13),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(ResourceUnavailable const& e) {
    xju::assert_equal(e.param_,Oid(".1.3.9.3333"));
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 13, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nobject .1.3.9.3333 could not be set due to unavailable resource.");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(14),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(CommitFailed const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 14, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\ncommit failed at object .1.3.9.3333 (no changes have been made).");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(15),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(UndoFailed const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 15, error index 2, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nRequest was valid but could not be applied (some changes have been made).");
  }
  
  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(17),
                      SnmpV2cResponse::ErrorIndex(3),
                      values));
    
    xju::assert_never_reached();
  }
  catch(NotWritable const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 17, error index 3, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nresponse specifies NoWritable (17) error but error index is beyond last oid in response.");
  }

  try {
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(17),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    
    xju::assert_never_reached();
  }
  catch(NotWritable const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 17, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nresponse specifies NoWritable (17) error but error index does not identify which oid was not found (index must be > 0).");
  }

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),std::shared_ptr<Value const>(
                                   new StringValue(fred))),
        SnmpVar(Oid(".1.3.9.3334"),std::shared_ptr<Value const>(
                                     new IntValue(3)))
        };
    validateResponse(
      SnmpV2cSetRequest(Community("dje"),
                        RequestId(23),
                        requestValues),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(17),
                      SnmpV2cResponse::ErrorIndex(2),
                      values));
    
    xju::assert_never_reached();
  }
  catch(NotWritable const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate response type 0xa2, community dd2, id 23, error status 17, error index 2, values .1.3.3: \"fred\", .1.3.9.3334: 3 to SnmpV2cSetRequest community dje, id 23, values .1.3.3: \"fred\", .1.3.9.3333: 3 because\nresponse error index indicates NoWritable (17) for oid .1.3.9.3334, but that oid was not requested.");
  }

}

void test6() throw()
{
  // validateResponse
  std::vector<SnmpVar> values {
    SnmpVar(Oid(".1.3.3"),std::shared_ptr<Value const>(
                                 new StringValue(fred))),
    SnmpVar(Oid(".1.3.9.3333"),std::shared_ptr<Value const>(
                                 new IntValue(3)))
  };
  auto x=validateResponse(
    SnmpV2cGetNextRequest(Community("dje"),
                          RequestId(23),
                          std::vector<Oid>({Oid(".1.3.2"),
                                            Oid(".1.3.9.3300")})),
    SnmpV2cResponse(0xA2,
                    Community("dd2"),
                    RequestId(23),
                    SnmpV2cResponse::ErrorStatus(0),
                    SnmpV2cResponse::ErrorIndex(0),
                    values));
    
  xju::assert_equal(x.size(),2U);
  auto i(x.begin());
  xju::assert_equal((*i++)->stringValue(),fred);
  xju::assert_equal((*i++)->intValue(),3);

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),std::shared_ptr<Value const>(
                                   new StringValue(fred))),
        SnmpVar(Oid(".1.3.9.3333"),std::shared_ptr<Value const>(
                                     new IntValue(3)))
        };
    auto x=validateResponse(
      SnmpV2cGetNextRequest(Community("dje"),
                            RequestId(23),
                            std::vector<Oid>({Oid(".1.3.2"),
                                              Oid(".1.3.9.3300")})),
      SnmpV2cResponse(0xA0,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(0),
                     SnmpV2cResponse::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(ResponseTypeMismatch const& e) {
    xju::assert_equal(e.got_,0xa0);
    xju::assert_equal(e.expected_,0xa2);
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa0, community dd2, id 23, error status 0, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SNMP V2c GetNext request community dje, id 23, oids .1.3.2, .1.3.9.3300 because\nexpected response of type 0xa2 but got response of type 0xa0.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  
  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),
                                 std::shared_ptr<Value const>(
                                   new StringValue(fred))),
      SnmpVar(Oid(".1.3.9.3333"),
                                 std::shared_ptr<Value const>(
                                   new IntValue(3)))
    };
    auto x=validateResponse(
      SnmpV2cGetNextRequest(Community("dje"),
                            RequestId(23),
                            std::vector<Oid>({Oid(".1.3.2"),
                                              Oid(".1.3.9.3300")})),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(24),
                     SnmpV2cResponse::ErrorStatus(0),
                     SnmpV2cResponse::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(ResponseIdMismatch const& e) {
    xju::assert_equal(e.got_,RequestId(24));
    xju::assert_equal(e.expected_,RequestId(23));
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 24, error status 0, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SNMP V2c GetNext request community dje, id 23, oids .1.3.2, .1.3.9.3300 because\nexpected response with id 23 but got response of id 24.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<SnmpVar> values {
    };
    auto x=validateResponse(
      SnmpV2cGetNextRequest(Community("dje"),
                            RequestId(23),
                            std::vector<Oid>({Oid(".1.3.2"),
                                              Oid(".1.3.9.3300")})),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(1),
                     SnmpV2cResponse::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(TooBig const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 23, error status 1, error index 0, values  to SNMP V2c GetNext request community dje, id 23, oids .1.3.2, .1.3.9.3300 because\nSNMP response would have exceeded server internal limit.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.2"),
                                 std::shared_ptr<Value const>(
                                   new StringValue(fred))),
        SnmpVar(Oid(".1.3.9.3333"),
                                   std::shared_ptr<Value const>(
                                     new NullValue))
        };
    auto x=validateResponse(
      SnmpV2cGetNextRequest(Community("dje"),
                            RequestId(23),
                            std::vector<Oid>({Oid(".1.3.2"),
                                              Oid(".1.3.9.3300")})),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(5),
                     SnmpV2cResponse::ErrorIndex(1),
                     values));
    
    xju::assert_never_reached();
  }
  catch(GenErr const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 23, error status 5, error index 1, values .1.3.2: \"fred\", .1.3.9.3333: null to SNMP V2c GetNext request community dje, id 23, oids .1.3.2, .1.3.9.3300 because\ngeneral error for oid .1.3.2.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    // too few values
    std::vector<SnmpVar> values {
    };
    auto x=validateResponse(
      SnmpV2cGetNextRequest(Community("dje"),
                            RequestId(23),
                            std::vector<Oid>({Oid(".1.3.2"),
                                              Oid(".1.3.9.3300")})),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(0),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 23, error status 0, error index 0, values  to SNMP V2c GetNext request community dje, id 23, oids .1.3.2, .1.3.9.3300 because\nvalue(s) not reported for oid(s) .1.3.2, .1.3.9.3300.");
  }

  try {
    // too many values
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),
                                 std::shared_ptr<Value const>(
                                   new StringValue(fred))),
      SnmpVar(Oid(".1.3.9.3333"),
                                 std::shared_ptr<Value const>(
                                   new IntValue(3))),
      SnmpVar(Oid(".1.3.10.3333"),
                                 std::shared_ptr<Value const>(
                                   new IntValue(7)))
    };
    auto x=validateResponse(
      SnmpV2cGetNextRequest(Community("dje"),
                            RequestId(23),
                            std::vector<Oid>({Oid(".1.3.2"),
                                              Oid(".1.3.9.3300")})),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(0),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 23, error status 0, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3, .1.3.10.3333: 7 to SNMP V2c GetNext request community dje, id 23, oids .1.3.2, .1.3.9.3300 because\nvalues reported for unrequested oid(s) .1.3.10.3333.");
  }

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),
              std::shared_ptr<Value const>(
                new StringValue(fred))),
      SnmpVar(Oid(".1.3.9.3300"),
              SnmpVar::NoSuchObject(Oid(".1.3.9.3300"),XJU_TRACED))
      };
    auto x=validateResponse(
      SnmpV2cGetNextRequest(Community("dje"),
                            RequestId(23),
                            std::vector<Oid>({Oid(".1.3.2"),
                                              Oid(".1.3.9.3300")})),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(0),
                     SnmpV2cResponse::ErrorIndex(0),
                     values));
    
    xju::assert_equal(x.size(),2U);
    auto i{x.begin()};
    xju::assert_equal((*i++)->stringValue(),fred);
    try {
      (*i++)->intValue();
      xju::assert_never_reached();
    }
    catch(SnmpVar::NoSuchObject const& e) {
      xju::assert_equal(readableRepr(e),"no such object .1.3.9.3300.");
    }
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),
                                 std::shared_ptr<Value const>(
                                   new StringValue(fred))),
      SnmpVar(Oid(".1.3.9.3300"),
              SnmpVar::NoSuchInstance(Oid(".1.3.9.3300"),XJU_TRACED))
      };
    auto x=validateResponse(
      SnmpV2cGetNextRequest(Community("dje"),
                            RequestId(23),
                            std::vector<Oid>({Oid(".1.3.3"),
                                              Oid(".1.3.9.3333")})),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(0),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    
    xju::assert_equal(x.size(),2U);
    auto i{x.begin()};
    xju::assert_equal((*i++)->stringValue(),fred);
    try {
      (*i++)->intValue();
      xju::assert_never_reached();
    }
    catch(SnmpVar::NoSuchInstance const& e) {
      xju::assert_equal(readableRepr(e),"no such instance .1.3.9.3300.");
    }
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),
                                 std::shared_ptr<Value const>(
                                   new StringValue(fred))),
      SnmpVar(Oid(".1.3.9.3300"),
              SnmpVar::EndOfMibView(Oid(".1.3.9.3300"),XJU_TRACED))
      };
    auto x=validateResponse(
      SnmpV2cGetNextRequest(Community("dje"),
                            RequestId(23),
                            std::vector<Oid>({Oid(".1.3.3"),
                                              Oid(".1.3.9.3300")})),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(0),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    auto i{x.begin()};
    xju::assert_equal((*i++)->stringValue(),fred);
    try {
      (*i++)->intValue();
      xju::assert_never_reached();
    }
    catch(SnmpVar::EndOfMibView const& e) {
      xju::assert_equal(readableRepr(e),"end of MIB view .1.3.9.3300.");
    }
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"");
    xju::assert_never_reached();
  }
}


// SnmpV2cGetBulkRequest
void test7() throw()
{
  // validateResponse
  std::vector<SnmpVar> values {
    SnmpVar(Oid(".1.3.3"),std::shared_ptr<Value const>(
                                 new StringValue(fred))),
    SnmpVar(Oid(".1.3.9.3333"),std::shared_ptr<Value const>(
                                 new IntValue(3)))
  };
  auto x=validateResponse(
    SnmpV2cGetBulkRequest(Community("dje"),
                          RequestId(23),
                          {},
                          {1U,std::vector<Oid>{Oid(".1.3.2"),Oid(".1.3.9.3300")}}),
    SnmpV2cResponse(0xA2,
                    Community("dd2"),
                    RequestId(23),
                    SnmpV2cResponse::ErrorStatus(0),
                    SnmpV2cResponse::ErrorIndex(0),
                    values));
    
  xju::assert_equal(x.first.size(),0U);
  xju::assert_equal(x.second.size(),1U);
  xju::assert_equal(x.second[0].size(),2U);
  auto i(x.second[0].begin());
  xju::assert_equal((*i++)->stringValue(),fred);
  xju::assert_equal((*i++)->intValue(),3);

  {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".9.8.0"),std::shared_ptr<Value const>(
                                   new StringValue(sal))),
      SnmpVar(Oid(".9.9.0"),std::shared_ptr<Value const>(
                                   new IntValue(8))),
      SnmpVar(Oid(".1.3.3"),std::shared_ptr<Value const>(
                                   new StringValue(fred))),
      SnmpVar(Oid(".1.3.9.3333"),std::shared_ptr<Value const>(
                                   new IntValue(3))),
      SnmpVar(Oid(".1.3.4"),std::shared_ptr<Value const>(
                                   new StringValue(jock))),
      SnmpVar(Oid(".1.3.9.3343"),std::shared_ptr<Value const>(
                                   new IntValue(32)))      
    };
    auto x=validateResponse(
      SnmpV2cGetBulkRequest(Community("dje"),
                            RequestId(23),
                            {Oid(".9.8"),Oid(".9.9")},
                            {1U,std::vector<Oid>{Oid(".1.3.2"),Oid(".1.3.9.3300")}}),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(0),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    
    xju::assert_equal(x.first.size(),2U);
    {
      auto i(x.first.begin());
      xju::assert_equal((*i).first,Oid(".9.8"));
      xju::assert_equal((*i).second->stringValue(),sal);
      ++i;
      xju::assert_equal((*i).first,Oid(".9.9"));
      xju::assert_equal((*i).second->intValue(),8);
    }
    xju::assert_equal(x.second.size(),2U);
    xju::assert_equal(x.second[0].size(),2U);
    {
      auto i(x.second[0].begin());
      xju::assert_equal((*i++)->stringValue(),fred);
      xju::assert_equal((*i++)->intValue(),3);
    }
    {
      xju::assert_equal(x.second[1].size(),2U);
      auto i(x.second[1].begin());
      xju::assert_equal((*i++)->stringValue(),jock);
      xju::assert_equal((*i++)->intValue(),32);
    }
  }
  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),std::shared_ptr<Value const>(
                                   new StringValue(fred))),
        SnmpVar(Oid(".1.3.9.3333"),std::shared_ptr<Value const>(
                                     new IntValue(3)))
        };
    auto x=validateResponse(
      SnmpV2cGetBulkRequest(Community("dje"),
                            RequestId(23),
                            {},
                            {1U,std::vector<Oid>{Oid(".1.3.2"),Oid(".1.3.9.3300")}}),
      SnmpV2cResponse(0xA0,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(0),
                     SnmpV2cResponse::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(ResponseTypeMismatch const& e) {
    xju::assert_equal(e.got_,0xa0);
    xju::assert_equal(e.expected_,0xa2);
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa0, community dd2, id 23, error status 0, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SNMP V2c GetBulk request community dje, id 23, oids  and up to 1 values of each of oids .1.3.2, .1.3.9.3300 because\nexpected response of type 0xa2 but got response of type 0xa0.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  
  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),
                                 std::shared_ptr<Value const>(
                                   new StringValue(fred))),
      SnmpVar(Oid(".1.3.9.3333"),
                                 std::shared_ptr<Value const>(
                                   new IntValue(3)))
    };
    auto x=validateResponse(
      SnmpV2cGetBulkRequest(Community("dje"),
                            RequestId(23),
                            {},
                            {1U,std::vector<Oid>{Oid(".1.3.2"),Oid(".1.3.9.3300")}}),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(24),
                     SnmpV2cResponse::ErrorStatus(0),
                     SnmpV2cResponse::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(ResponseIdMismatch const& e) {
    xju::assert_equal(e.got_,RequestId(24));
    xju::assert_equal(e.expected_,RequestId(23));
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 24, error status 0, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3 to SNMP V2c GetBulk request community dje, id 23, oids  and up to 1 values of each of oids .1.3.2, .1.3.9.3300 because\nexpected response with id 23 but got response of id 24.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<SnmpVar> values {
    };
    auto x=validateResponse(
      SnmpV2cGetBulkRequest(Community("dje"),
                            RequestId(23),
                            {},
                            {1U,std::vector<Oid>{Oid(".1.3.2"),Oid(".1.3.9.3300")}}),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(1),
                     SnmpV2cResponse::ErrorIndex(0),
                     values));
    
    xju::assert_never_reached();
  }
  catch(TooBig const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 23, error status 1, error index 0, values  to SNMP V2c GetBulk request community dje, id 23, oids  and up to 1 values of each of oids .1.3.2, .1.3.9.3300 because\nSNMP response would have exceeded server internal limit.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.2"),
                                 std::shared_ptr<Value const>(
                                   new StringValue(fred))),
        SnmpVar(Oid(".1.3.9.3333"),
                                   std::shared_ptr<Value const>(
                                     new NullValue))
        };
    auto x=validateResponse(
      SnmpV2cGetBulkRequest(Community("dje"),
                            RequestId(23),
                            {},
                            {1U,std::vector<Oid>{Oid(".1.3.2"),
                                                 Oid(".1.3.9.3300")}}),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(5),
                     SnmpV2cResponse::ErrorIndex(1),
                     values));
    
    xju::assert_never_reached();
  }
  catch(GenErr const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 23, error status 5, error index 1, values .1.3.2: \"fred\", .1.3.9.3333: null to SNMP V2c GetBulk request community dje, id 23, oids  and up to 1 values of each of oids .1.3.2, .1.3.9.3300 because\ngeneral error for oid .1.3.2.");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    // too few values
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),std::shared_ptr<Value const>(
                                   new StringValue(fred)))
    };
    auto x=validateResponse(
      SnmpV2cGetBulkRequest(Community("dje"),
                            RequestId(23),
                            {},
                            {1U,std::vector<Oid>{Oid(".1.3.2"),
                                                 Oid(".1.3.9.3300")}}),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(0),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 23, error status 0, error index 0, values .1.3.3: \"fred\" to SNMP V2c GetBulk request community dje, id 23, oids  and up to 1 values of each of oids .1.3.2, .1.3.9.3300 because\n\"next values\" row 0 only has 1 values, which is less than the row size of 2.");
  }

  try {
    // too few values
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".9.8"),std::shared_ptr<Value const>(
                                   new StringValue(fred)))
    };
    auto x=validateResponse(
      SnmpV2cGetBulkRequest(Community("dje"),
                            RequestId(23),
                            {Oid(".9.8"),Oid(".9.9")},
                            {1U,std::vector<Oid>{Oid(".1.3.2"),
                                                 Oid(".1.3.9.3300")}}),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(0),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 23, error status 0, error index 0, values .9.8: \"fred\" to SNMP V2c GetBulk request community dje, id 23, oids .9.8, .9.9 and up to 1 values of each of oids .1.3.2, .1.3.9.3300 because\nresponse contains only 1 value(s), which is less than then number (2) of \"get\" oids requested.");
  }

  try {
    // too many values
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),
                                 std::shared_ptr<Value const>(
                                   new StringValue(fred))),
      SnmpVar(Oid(".1.3.9.3333"),
                                 std::shared_ptr<Value const>(
                                   new IntValue(3))),
      SnmpVar(Oid(".1.3.10.3333"),
                                 std::shared_ptr<Value const>(
                                   new IntValue(7)))
    };
    auto x=validateResponse(
      SnmpV2cGetBulkRequest(Community("dje"),
                            RequestId(23),
                            {},
                            {1U,std::vector<Oid>{Oid(".1.3.2"),
                                                 Oid(".1.3.9.3300")}}),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(0),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to validate SNMP V2c response type 0xa2, community dd2, id 23, error status 0, error index 0, values .1.3.3: \"fred\", .1.3.9.3333: 3, .1.3.10.3333: 7 to SNMP V2c GetBulk request community dje, id 23, oids  and up to 1 values of each of oids .1.3.2, .1.3.9.3300 because\n\"next values\" row 1 only has 1 values, which is less than the row size of 2.");
  }

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),
                                 std::shared_ptr<Value const>(
                                   new StringValue(fred))),
      SnmpVar(Oid(".1.3.9.3300"),
              SnmpVar::NoSuchObject(Oid(".1.3.9.3300"),XJU_TRACED))
      };
    auto x=validateResponse(
      SnmpV2cGetBulkRequest(Community("dje"),
                            RequestId(23),
                            {},
                            {1U,std::vector<Oid>{Oid(".1.3.2"),
                                                 Oid(".1.3.9.3300")}}),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                     RequestId(23),
                     SnmpV2cResponse::ErrorStatus(0),
                     SnmpV2cResponse::ErrorIndex(0),
                     values));
    
    xju::assert_equal(x.first.size(),0U);
    xju::assert_equal(x.second.size(),1U);
    auto i{x.second[0].begin()};
    xju::assert_equal((*i++)->stringValue(),fred);
    try {
      (*i++)->intValue();
      xju::assert_never_reached();
    }
    catch(SnmpVar::NoSuchObject const& e) {
      xju::assert_equal(readableRepr(e),"no such object .1.3.9.3300.");
    }
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),
                                 std::shared_ptr<Value const>(
                                   new StringValue(fred))),
      SnmpVar(Oid(".1.3.9.3300"),
              SnmpVar::NoSuchInstance(Oid(".1.3.9.3300"),XJU_TRACED))
      };
    auto x=validateResponse(
      SnmpV2cGetBulkRequest(Community("dje"),
                            RequestId(23),
                            {},
                            {1U,std::vector<Oid>{Oid(".1.3.3"),
                                                 Oid(".1.3.9.3333")}}),
      SnmpV2cResponse(0xA2,
                     Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(0),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    
    xju::assert_equal(x.first.size(),0U);
    xju::assert_equal(x.second.size(),1U);
    xju::assert_equal(x.second[0].size(),2U);
    auto i{x.second[0].begin()};
    xju::assert_equal((*i++)->stringValue(),fred);
    try {
      (*i++)->intValue();
      xju::assert_never_reached();
    }
    catch(SnmpVar::NoSuchInstance const& e) {
      xju::assert_equal(readableRepr(e),"no such instance .1.3.9.3300.");
    }
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<SnmpVar> values {
      SnmpVar(Oid(".1.3.3"),
                                 std::shared_ptr<Value const>(
                                   new StringValue(fred))),
      SnmpVar(Oid(".1.3.9.3300"),
              SnmpVar::EndOfMibView(Oid(".1.3.9.3300"),XJU_TRACED))
      };
    auto x=validateResponse(
      SnmpV2cGetBulkRequest(Community("dje"),
                            RequestId(23),
                            {},
                            {1U,std::vector<Oid>{Oid(".1.3.3"),
                                                 Oid(".1.3.9.3300")}}),
      SnmpV2cResponse(0xA2,
                      Community("dd2"),
                      RequestId(23),
                      SnmpV2cResponse::ErrorStatus(0),
                      SnmpV2cResponse::ErrorIndex(0),
                      values));
    xju::assert_equal(x.first.size(),0U);
    xju::assert_equal(x.second.size(),1U);
    xju::assert_equal(x.second[0].size(),2U);
    auto i{x.second[0].begin()};
    xju::assert_equal((*i++)->stringValue(),fred);
    try {
      (*i++)->intValue();
      xju::assert_never_reached();
    }
    catch(SnmpVar::EndOfMibView const& e) {
      xju::assert_equal(readableRepr(e),"end of MIB view .1.3.9.3300.");
    }
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"");
    xju::assert_never_reached();
  }
}


}
}

using namespace xju::snmp;


int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  test3(), ++n;
  test4(), ++n;
  test5(), ++n;
  test6(), ++n;
  test7(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}
