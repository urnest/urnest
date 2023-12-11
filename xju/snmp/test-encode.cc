// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/encode.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/snmp/SnmpV1GetRequest.hh>
#include <xju/snmp/NullValue.hh>
#include <xju/snmp/SnmpV1SetRequest.hh>
#include <xju/snmp/SnmpV1GetNextRequest.hh>
#include <xju/snmp/SnmpV1Trap.hh>
#include <xju/snmp/SnmpV1Response.hh>
#include <xju/snmp/IntValue.hh>
#include <xju/snmp/StringValue.hh>
#include <xju/snmp/OidValue.hh>
#include <xju/snmp/SnmpV2cGetRequest.hh>
#include <xju/snmp/SnmpV2cSetRequest.hh>
#include <xju/snmp/SnmpV2cGetNextRequest.hh>
#include <xju/snmp/SnmpV2cTrap.hh>
#include <xju/snmp/SnmpV2cGetBulkRequest.hh>
#include <xju/snmp/decodeSnmpV1Response.hh>
#include <xju/snmp/SnmpV2cResponse.hh>
#include <xju/snmp/decodeSnmpV2cResponse.hh>
#include <xju/snmp/SnmpV3ScopedPDU.hh>
#include <xju/snmp/decodeSnmpV3ScopedPDU.hh>
#include <xju/snmp/SnmpV3Message.hh>
#include <xju/snmp/decodeSnmpV3Message.hh>
#include <xju/snmp/EngineBoots.hh>
#include <xju/snmp/EngineTime.hh>
#include <xju/UserName.hh>
#include <xju/snmp/SnmpV3UsmSecurityParameters.hh>
#include <xju/snmp/decodeSnmpV3UsmSecurityParameters.hh>
#include <tuple>

namespace xju
{
namespace snmp
{

void test1() throw()
{
  // encode(SnmpV1GetRequest)
  SnmpV1GetRequest r(
    Community("private"),
    RequestId(1),
    {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0")});
  std::vector<uint8_t> x(encode(r));
  xju::assert_equal(
    x,
    std::vector<uint8_t>({
        0x30,0x2c,0x02,0x01,0x00,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA0,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
          }));
}

void test2() throw()
{
  // encode(SnmpV1SetRequest)
  SnmpV1SetRequest r(
    Community("private"),
    RequestId(1),
    {{Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
          std::shared_ptr<Value const>(new NullValue)}});
  std::vector<uint8_t> x(encode(r));
  xju::assert_equal(
    x,
    std::vector<uint8_t>({
        0x30,0x2c,0x02,0x01,0x00,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA3,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
          }));
}

void test3() throw()
{
  // encode(SnmpV1GetNexRequest)
  SnmpV1GetNextRequest r(
    Community("private"),
    RequestId(1),
    {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0")});
  std::vector<uint8_t> x(encode(r));
  xju::assert_equal(
    x,
    std::vector<uint8_t>({
        0x30,0x2c,0x02,0x01,0x00,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA1,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
          }));
}

void test4() throw()
{
  {
    // encode(SnmpV1Trap)
    SnmpV1Trap t(
      Community("private"),
      Oid(".1.3.6"),
      xju::ip::v4::Address((192U<<24)+(168U<<16)+(0U<<8)+3U),
      SnmpV1Trap::GenericType(6),
      SnmpV1Trap::SpecificType(42),
      std::chrono::milliseconds(330),
      {{Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
            std::shared_ptr<Value const>(new IntValue(2))}});
    std::vector<uint8_t> x(encode(t));
    xju::assert_equal(
      x,
      std::vector<uint8_t>({
          0x30,55,
            0x02,0x01,0x00,//snmp v1
            0x04,0x07,'p','r','i','v','a','t','e',
            0xA4,41,//trap-pdu
            0x06,2,0x2B,6,//oid
            0x40,4,192,168,0,3, //origin
            0x02,1,6, //generic type
            0x02,1,42, //specific type
            0x43,1,33, //timestamp
            0x30,20, // vars:
            0x30,18, // var:
            0x06,0x0D,0x2B,6,1,4,1,0x94,0x78,1,2,7,3,2,0,//oid
            0x02,1,2 //value
            }));
  }
  {
    // encode(SnmpV1Trap)
    SnmpV1Trap t(
      Community("private"),
      Oid(".1.3.6"),
      xju::ip::v4::Address((192U<<24)+(168U<<16)+(0U<<8)+3U),
      SnmpV1Trap::GenericType(6),
      SnmpV1Trap::SpecificType(42),
      std::chrono::milliseconds(330),
      {{Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
            std::shared_ptr<Value const>(new StringValue("Once apon a time there were three little pigs, who each wanted to build a house. The first little pig built a straw house, with straw from the field. The second little pig built a stick house, with sticks from the forest. The third little pig built a brick house, with bricks from the brick works."))}});
    std::vector<uint8_t> x(encode(t));
    xju::assert_equal(
      x,
      std::vector<uint8_t>({
          0x30, 0x82, 0x01, 0x67,
            0x02, 0x01, 0x00, 0x04, 0x07, 0x70, 0x72, 0x69, 0x76, 0x61, 0x74, 0x65, 0xa4, 0x82, 0x01, 0x57,
            0x06, 0x02, 0x2b, 0x06, 0x40, 0x04, 0xc0, 0xa8, 0x00, 0x03, 0x02, 0x01, 0x06, 0x02, 0x01, 0x2a,
            0x43, 0x01, 0x21, 0x30, 0x82, 0x01, 0x40, 0x30, 0x82, 0x01, 0x3c, 0x06, 0x0d, 0x2b, 0x06, 0x01,
            0x04, 0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00, 0x04, 0x82, 0x01, 0x29, 0x4f, 0x6e,
            0x63, 0x65, 0x20, 0x61, 0x70, 0x6f, 0x6e, 0x20, 0x61, 0x20, 0x74, 0x69, 0x6d, 0x65, 0x20, 0x74,
            0x68, 0x65, 0x72, 0x65, 0x20, 0x77, 0x65, 0x72, 0x65, 0x20, 0x74, 0x68, 0x72, 0x65, 0x65, 0x20,
            0x6c, 0x69, 0x74, 0x74, 0x6c, 0x65, 0x20, 0x70, 0x69, 0x67, 0x73, 0x2c, 0x20, 0x77, 0x68, 0x6f,
            0x20, 0x65, 0x61, 0x63, 0x68, 0x20, 0x77, 0x61, 0x6e, 0x74, 0x65, 0x64, 0x20, 0x74, 0x6f, 0x20,
            0x62, 0x75, 0x69, 0x6c, 0x64, 0x20, 0x61, 0x20, 0x68, 0x6f, 0x75, 0x73, 0x65, 0x2e, 0x20, 0x54,
            0x68, 0x65, 0x20, 0x66, 0x69, 0x72, 0x73, 0x74, 0x20, 0x6c, 0x69, 0x74, 0x74, 0x6c, 0x65, 0x20,
            0x70, 0x69, 0x67, 0x20, 0x62, 0x75, 0x69, 0x6c, 0x74, 0x20, 0x61, 0x20, 0x73, 0x74, 0x72, 0x61,
            0x77, 0x20, 0x68, 0x6f, 0x75, 0x73, 0x65, 0x2c, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x73, 0x74,
            0x72, 0x61, 0x77, 0x20, 0x66, 0x72, 0x6f, 0x6d, 0x20, 0x74, 0x68, 0x65, 0x20, 0x66, 0x69, 0x65,
            0x6c, 0x64, 0x2e, 0x20, 0x54, 0x68, 0x65, 0x20, 0x73, 0x65, 0x63, 0x6f, 0x6e, 0x64, 0x20, 0x6c,
            0x69, 0x74, 0x74, 0x6c, 0x65, 0x20, 0x70, 0x69, 0x67, 0x20, 0x62, 0x75, 0x69, 0x6c, 0x74, 0x20,
            0x61, 0x20, 0x73, 0x74, 0x69, 0x63, 0x6b, 0x20, 0x68, 0x6f, 0x75, 0x73, 0x65, 0x2c, 0x20, 0x77,
            0x69, 0x74, 0x68, 0x20, 0x73, 0x74, 0x69, 0x63, 0x6b, 0x73, 0x20, 0x66, 0x72, 0x6f, 0x6d, 0x20,
            0x74, 0x68, 0x65, 0x20, 0x66, 0x6f, 0x72, 0x65, 0x73, 0x74, 0x2e, 0x20, 0x54, 0x68, 0x65, 0x20,
            0x74, 0x68, 0x69, 0x72, 0x64, 0x20, 0x6c, 0x69, 0x74, 0x74, 0x6c, 0x65, 0x20, 0x70, 0x69, 0x67,
            0x20, 0x62, 0x75, 0x69, 0x6c, 0x74, 0x20, 0x61, 0x20, 0x62, 0x72, 0x69, 0x63, 0x6b, 0x20, 0x68,
            0x6f, 0x75, 0x73, 0x65, 0x2c, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x62, 0x72, 0x69, 0x63, 0x6b,
            0x73, 0x20, 0x66, 0x72, 0x6f, 0x6d, 0x20, 0x74, 0x68, 0x65, 0x20, 0x62, 0x72, 0x69, 0x63, 0x6b,
            0x20, 0x77, 0x6f, 0x72, 0x6b, 0x73, 0x2e
            }));
  }
  
}

  
void test5() throw()
{
  // encode(SnmpV2cGetRequest)
  SnmpV2cGetRequest r(
    Community("private"),
    RequestId(1),
    {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0")});
  std::vector<uint8_t> x(encode(r));
  xju::assert_equal(
    x,
    std::vector<uint8_t>({
        0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA0,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
          }));
}

void test6() throw()
{
  // encode(SnmpV2cSetRequest)
  SnmpV2cSetRequest r(
    Community("private"),
    RequestId(1),
    {{Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
          std::shared_ptr<Value const>(new NullValue)}});
  std::vector<uint8_t> x(encode(r));
  xju::assert_equal(
    x,
    std::vector<uint8_t>({
        0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA3,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
          }));
}

void test7() throw()
{
  {
    // encode(SnmpV2cGetNexRequest)
    SnmpV2cGetNextRequest r(
      Community("private"),
      RequestId(1),
      {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0")});
    std::vector<uint8_t> x(encode(r));
    xju::assert_equal(
      x,
      std::vector<uint8_t>({
          0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA1,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
        }));
  }
  {
    // encode(SnmpV2cGetNexRequest)
    SnmpV2cGetNextRequest r(
      Community("public"),
      RequestId(0x32f0def9),
      {Oid(".1.3.6.1.2.1.1.9.1.4")});
    std::vector<uint8_t> x(encode(r));
    xju::assert_equal(
      x,
      std::vector<uint8_t>({
          0x30,0x2a,
          0x02,0x01,0x01, //snmp v2c
          0x04,0x06,0x70,0x75,0x62,0x6c,0x69,0x63, //public
          0xa1,0x1d,
          0x02,0x04,0x32,0xf0,0xde,0xf9, //requestid
          0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x0f,0x30,0x0d,0x06,0x09,0x2b,0x06,0x01,0x02,0x01,0x01,0x09,0x01,0x04,0x05,0x00
        }));
  }
}

void test8() throw()
{
  // encode(SnmpV2cTrap)
  SnmpV2cTrap r(
    Community("aaaa"),
    RequestId(1798143932),
    Oid(".1.3.6.1.4.1.7.8"),
    std::chrono::milliseconds(370),
    { {Oid(".1.3.6.1.4.1.7.8.1"),
          std::shared_ptr<Value const>(
            new IntValue(4))} });
  std::vector<uint8_t> x(encode(r));
  xju::assert_equal(
    x,
    std::vector<uint8_t>({
        0x30, 0x4e, 0x02, 0x01, 0x01, 0x04, 0x04, 0x61, 0x61, 0x61, 0x61, 0xa7, 0x43, 0x02, 0x04, 0x6b, 0x2d, 0x7f, 0xbc, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x35, 0x30, 0x0d, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x02, 0x01, 0x01, 0x03, 0x00, 0x43, 0x01, 0x25, 0x30, 0x15, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x06, 0x03, 0x01, 0x01, 0x04, 0x01, 0x00, 0x06, 0x07, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x07, 0x08, 0x30, 0x0d, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x07, 0x08, 0x01, 0x02, 0x01, 0x04
          }));
}

void test9() throw()
{
  {
    // encode(SnmpV2cGetBulkRequest)
    SnmpV2cGetBulkRequest r(
      Community("private"),
      RequestId(1),
      {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0")},
      {0,{}});
    std::vector<uint8_t> x(encode(r));
    xju::assert_equal(
      x,
      std::vector<uint8_t>({
          0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA5,0x1E,0x02,0x01,0x01,0x02,0x01,0x01,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
        }));
  }
  {
    SnmpV2cGetBulkRequest r(
      Community("public"),
      RequestId(0x32f0def9),
      {},
      {4U, {Oid(".1.3.6.1.2.1.1.9.1.4")}});
    std::vector<uint8_t> x(encode(r));
    xju::assert_equal(
      x,
      std::vector<uint8_t>({
          0x30,0x2a,
          0x02,0x01,0x01, //snmp v2c
          0x04,0x06,0x70,0x75,0x62,0x6c,0x69,0x63, //public
          0xa5,0x1d,
          0x02,0x04,0x32,0xf0,0xde,0xf9, //requestid
          0x02,0x01,0x00, //repeaters
          0x02,0x01,0x04, //max-repititions
          0x30,0x0f,0x30,0x0d,0x06,0x09,0x2b,0x06,0x01,0x02,0x01,0x01,0x09,0x01,0x04,0x05,0x00
        }));
  }
  {
    SnmpV2cGetBulkRequest r(
      Community("public"),
      RequestId(0x32f0def9),
      {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0")},
      {4U, {Oid(".1.3.6.1.2.1.1.9.1.4")}});
    std::vector<uint8_t> x(encode(r));
    xju::assert_equal(
      x,
      std::vector<uint8_t>({
          0x30,0x3d,
          0x02,0x01,0x01, //snmp v2c
          0x04,0x06,0x70,0x75,0x62,0x6c,0x69,0x63, //public
          0xa5,0x30,
          0x02,0x04,0x32,0xf0,0xde,0xf9, //requestid
          0x02,0x01,0x01, //repeaters
          0x02,0x01,0x04, //max-repetitions
          0x30,0x22,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00,0x30,0x0d,0x06,0x09,0x2b,0x06,0x01,0x02,0x01,0x01,0x09,0x01,0x04,0x05,0x00
        }));
  }
}

void test10()
{
  //encode(SnmpV1Response)
  {
    SnmpV1Response y(decodeSnmpV1Response(
                       encode(SnmpV1Response(
                                0xA2,
                                Community("private"),
                                RequestId(1),
                                SnmpV1Response::ErrorStatus::NO_ERROR,
                                SnmpV1Response::ErrorIndex(0),
                                { {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                   std::make_unique<NullValue>()} }))));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV1Response::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV1Response::ErrorIndex(0));
    xju::assert_equal(y.values_.size(),1);
    xju::assert_equal(y.values_[0].first,Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_not_equal(dynamic_cast<NullValue const*>(&*y.values_[0].second),(NullValue const*)0);
  }

  // variations
  
  // definite long length eg long string
  try {
    SnmpV1Response y(decodeSnmpV1Response(
                       encode(SnmpV1Response(
                                0xA2,
                                Community("private"),
                                RequestId(1),
                                SnmpV1Response::ErrorStatus::NO_ERROR,
                                SnmpV1Response::ErrorIndex(0),
                                { {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                   std::make_unique<StringValue>(
                                     std::vector<uint8_t>(128,'a'))} }))));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV1Response::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV1Response::ErrorIndex(0));
    xju::assert_equal(y.values_.size(),1);
    xju::assert_equal(y.values_[0].first,Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_not_equal(dynamic_cast<StringValue const*>(&*y.values_[0].second),(StringValue const*)0);
    xju::assert_equal(dynamic_cast<StringValue const*>(&*y.values_[0].second)->val_,std::vector<uint8_t>(128,'a'));
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  // multi-byte int value
  try {
    SnmpV1Response y(decodeSnmpV1Response(
                       encode(SnmpV1Response(
                                0xA2,
                                Community("private"),
                                RequestId(0x100),
                                SnmpV1Response::ErrorStatus::NO_ERROR,
                                SnmpV1Response::ErrorIndex(0),
                                { {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                      std::make_unique<NullValue>()} }))));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(0x100));
    xju::assert_equal(y.error_,SnmpV1Response::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV1Response::ErrorIndex(0));
    xju::assert_equal(y.values_.size(),1);
    xju::assert_equal(y.values_[0].first,Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_not_equal(dynamic_cast<NullValue const*>(&*y.values_[0].second),(NullValue const*)0);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
    
  // oid value
  try {
    SnmpV1Response y(decodeSnmpV1Response(
                       encode(SnmpV1Response(
                                0xA2,
                                Community("private"),
                                RequestId(1),
                                SnmpV1Response::ErrorStatus::GEN_ERR,
                                SnmpV1Response::ErrorIndex(1),
                                { {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                   std::make_unique<NullValue>()},
                                  {Oid(".1.3"),
                                   std::make_unique<OidValue>(Oid(".1.3.7"))}}))));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV1Response::ErrorStatus::GEN_ERR);
    xju::assert_equal(y.errorIndex_,SnmpV1Response::ErrorIndex(1));
    xju::assert_equal(y.values_.size(),2);
    xju::assert_equal(y.values_[0].first,Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_not_equal(dynamic_cast<NullValue const*>(&*y.values_[0].second),(NullValue const*)0);
    xju::assert_equal(y.values_[1].first,Oid(".1.3"));
    xju::assert_not_equal(dynamic_cast<OidValue const*>(&*y.values_[1].second),(OidValue const*)0);
    xju::assert_equal(dynamic_cast<OidValue const*>(&*y.values_[1].second)->val_,Oid(".1.3.7"));
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  
}

void test11()
{
  //encode(SnmpV2cResponse)
  
  SnmpV2cResponse y(decodeSnmpV2cResponse(
                      encode(SnmpV2cResponse(
                               0xA2,
                               Community("private"),
                               RequestId(1),
                               SnmpV2cResponse::ErrorStatus::NO_ERROR,
                               SnmpV2cResponse::ErrorIndex(0),
                               {
                                 {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                  std::make_unique<NullValue>()}
                               }))));
  xju::assert_equal(y.responseType_,0xA2);
  xju::assert_equal(y.community_,Community("private"));
  xju::assert_equal(y.id_,RequestId(1));
  xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
  xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
  xju::assert_equal(y.varResults_.size(),1);
  xju::assert_equal(y.varResults_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
  xju::assert_not_equal(dynamic_cast<NullValue const*>(&*y.varResults_[0].value()),(NullValue const*)0);

  {
    SnmpV2cResponse y(decodeSnmpV2cResponse(
                        encode(SnmpV2cResponse(
                                 0xA2,
                                 Community("private"),
                                 RequestId(1),
                                 SnmpV2cResponse::ErrorStatus::NO_CREATION,
                                 SnmpV2cResponse::ErrorIndex(1),
                                 {
                                   {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                    std::make_unique<NullValue>()}
                                 }))));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_CREATION);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(1));
    xju::assert_equal(y.varResults_.size(),1);
    xju::assert_equal(y.varResults_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_not_equal(dynamic_cast<NullValue const*>(&*y.varResults_[0].value()),(NullValue const*)0);
  }
  try {
    // decodeSnmpV2cResponse - var nosuchobject
    SnmpV2cResponse y(decodeSnmpV2cResponse(
                        encode(SnmpV2cResponse(
                                 0xA2,
                                 Community("private"),
                                 RequestId(1),
                                 SnmpV2cResponse::ErrorStatus::NO_ERROR,
                                 SnmpV2cResponse::ErrorIndex(0),
                                 {
                                   {SnmpVar(
                                       Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                       SnmpVar::NoSuchObject(Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                                             XJU_TRACED))}
                                 }))));
  xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResults_.size(),1);
    xju::assert_equal(y.varResults_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    try{
      y.varResults_[0].value();
      xju::assert_never_reached();
    }
    catch(SnmpVar::NoSuchObject const&){
    }
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  try {
    // decodeSnmpV2cResponse - var nosuchinstance
    
    SnmpV2cResponse y(decodeSnmpV2cResponse(
                        encode(SnmpV2cResponse(
                                 0xA2,
                                 Community("private"),
                                 RequestId(1),
                                 SnmpV2cResponse::ErrorStatus::NO_ERROR,
                                 SnmpV2cResponse::ErrorIndex(0),
                                 {
                                   SnmpVar(
                                     Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                     SnmpVar::NoSuchInstance(Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                                             XJU_TRACED))
                                 }))));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResults_.size(),1);
    xju::assert_equal(y.varResults_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    try{
      y.varResults_[0].value();
      xju::assert_never_reached();
    }
    catch(SnmpVar::NoSuchInstance const&){
    }
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  try {
    // decodeSnmpV2cResponse - var endofmibview
    
    SnmpV2cResponse y(decodeSnmpV2cResponse(
                        encode(SnmpV2cResponse(
                                 0xA2,
                                 Community("private"),
                                 RequestId(1),
                                 SnmpV2cResponse::ErrorStatus::NO_ERROR,
                                 SnmpV2cResponse::ErrorIndex(0),
                                 {
                                   SnmpVar(
                                     Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                     SnmpVar::EndOfMibView(Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                                           XJU_TRACED))
                                 }))));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResults_.size(),1);
    xju::assert_equal(y.varResults_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    try{
      y.varResults_[0].value();
      xju::assert_never_reached();
    }
    catch(SnmpVar::EndOfMibView const&){
    }
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  // decodeSnmpV2cResponse variations
  
  // long string
  try {
    SnmpV2cResponse y(decodeSnmpV2cResponse(
                        encode(SnmpV2cResponse(
                                 0xA2,
                                 Community("private"),
                                 RequestId(1),
                                 SnmpV2cResponse::ErrorStatus::NO_ERROR,
                                 SnmpV2cResponse::ErrorIndex(0),
                                 {
                                   {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                       std::make_unique<StringValue>(std::vector<uint8_t>(128,'a'))}
                                 }))));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResults_.size(),1);
    xju::assert_equal(y.varResults_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_not_equal(dynamic_cast<StringValue const*>(&*y.varResults_[0].value()),(StringValue const*)0);
    xju::assert_equal(dynamic_cast<StringValue const*>(&*y.varResults_[0].value())->val_,std::vector<uint8_t>(128,'a'));
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  

  // multi-byte int value
  try {
    SnmpV2cResponse y(decodeSnmpV2cResponse(
                        encode(SnmpV2cResponse(
                                 0xA2,
                                 Community("private"),
                                 RequestId(0x100),
                                 SnmpV2cResponse::ErrorStatus::NO_ERROR,
                                 SnmpV2cResponse::ErrorIndex(0),
                                 {
                                   {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                    std::make_unique<NullValue>()}
                                 }))));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(0x100));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResults_.size(),1);
    xju::assert_equal(y.varResults_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_not_equal(dynamic_cast<NullValue const*>(&*y.varResults_[0].value()),(NullValue const*)0);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
    
  // oid value
  try {
    SnmpV2cResponse y(decodeSnmpV2cResponse(
                        encode(SnmpV2cResponse(
                                 0xA2,
                                 Community("private"),
                                 RequestId(1),
                                 SnmpV2cResponse::ErrorStatus::NO_ERROR,
                                 SnmpV2cResponse::ErrorIndex(0),
                                 {
                                   {Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                    std::make_unique<NullValue>()},
                                   {Oid(".1.3"),
                                    std::make_unique<OidValue>(Oid(".1.3.7"))}
                                 }))));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResults_.size(),2);
    xju::assert_equal(y.varResults_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_not_equal(dynamic_cast<NullValue const*>(&*y.varResults_[0].value()),(NullValue const*)0);
    xju::assert_equal(y.varResults_[1].oid(),Oid(".1.3"));
    xju::assert_not_equal(dynamic_cast<OidValue const*>(&*y.varResults_[1].value()),(OidValue const*)0);
    xju::assert_equal(dynamic_cast<OidValue const*>(&*y.varResults_[1].value())->val_,Oid(".1.3.7"));
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
}

void test12(){
  xju::assert_equal(decodeSnmpV3ScopedPDU(
                      encode(SnmpV3ScopedPDU(
                               ContextEngineID(std::vector<uint8_t>{0x01, 0x02}),
                               ContextName("fred"),
                               PDU(RequestId(0x690f79b6),
                                   0x00,
                                   0x00,
                                   {SnmpVar(Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                            std::shared_ptr<Value>(new NullValue()))},
                                   0x0a)))),
                    SnmpV3ScopedPDU(ContextEngineID(std::vector<uint8_t>{0x01, 0x02}),
                                    ContextName("fred"),
                                    PDU(RequestId(0x690f79b6),
                                        0x00,
                                        0x00,
                                        {SnmpVar(Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                                 std::shared_ptr<Value>(new NullValue()))},
                                        0x0a)));
}

void test13(){
  xju::assert_equal(
    decodeSnmpV3Message(
      encode(SnmpV3Message(
               SnmpV3Message::ID(33),
               512,
               (SnmpV3Message::Flags)(SnmpV3Message::REPORTABLE|SnmpV3Message::AUTH),
               SnmpV3Message::SecurityModel(3),
               SnmpV3SecParams(std::vector<uint8_t>{0x01,0x02,0x03}),
               SnmpV3ScopedPduData(std::vector<uint8_t>{
                 0x30, 0x3c,
                   // contextEngineID
                   0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
                   0x00, 0x00,
                   // contextName = fred
                   0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
                   // data = Get PDU
                   0xa0, 0x21, 0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
                   0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06, 0x0d, 0x2b, 0x06, 0x01, 0x04,
                   0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00, 0x05, 0x00})))),
    SnmpV3Message(
               SnmpV3Message::ID(33),
               512,
               (SnmpV3Message::Flags)(SnmpV3Message::REPORTABLE|SnmpV3Message::AUTH),
               SnmpV3Message::SecurityModel(3),
               SnmpV3SecParams(std::vector<uint8_t>{0x01,0x02,0x03}),
               SnmpV3ScopedPduData(std::vector<uint8_t>{
                 0x30, 0x3c,
                   // contextEngineID
                   0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
                   0x00, 0x00,
                   // contextName = fred
                   0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
                   // data = Get PDU
                   0xa0, 0x21, 0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
                   0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06, 0x0d, 0x2b, 0x06, 0x01, 0x04,
                   0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00, 0x05, 0x00})));
                   
}


void test14(){
  xju::assert_equal(
    decodeSnmpV3UsmSecurityParameters(
      encode(SnmpV3UsmSecurityParameters(
               ContextEngineID(
                 std::vector<uint8_t>{0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00}),
               EngineBoots(0x775),
               EngineTime(0x60fd),
               UserName("fred")),
             SnmpV3UsmAuthData(std::vector<uint8_t>{}),
             SnmpV3UsmPrivData(std::vector<uint8_t>{0x01}))),
    std::make_tuple(SnmpV3UsmSecurityParameters(
                      ContextEngineID(
                        std::vector<uint8_t>{0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00}),
                      EngineBoots(0x775),
                      EngineTime(0x60fd),
                      UserName("fred")),
                    SnmpV3UsmAuthData(std::vector<uint8_t>{}),
                    SnmpV3UsmPrivData(std::vector<uint8_t>{0x01})));
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
  test8(), ++n;
  test9(), ++n;
  test10(), ++n;
  test11(), ++n;
  test12(), ++n;
  test13(), ++n;
  test14(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}


