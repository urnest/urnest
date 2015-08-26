// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/snmp/decodeSnmpV2cResponse.hh"

#include <iostream>
#include <xju/assert.hh>
#include "xju/snmp/NullValue.hh"
#include "xju/snmp/StringValue.hh"
#include "xju/snmp/OidValue.hh"

namespace xju
{
namespace snmp
{

void test1() {
  // decodeSnmpV2cResponse
  std::vector<uint8_t> x({
      0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA2,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
        });
  
  SnmpV2cResponse y(decodeSnmpV2cResponse(x));
  xju::assert_equal(y.responseType_,0xA2);
  xju::assert_equal(y.community_,Community("private"));
  xju::assert_equal(y.id_,RequestId(1));
  xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
  xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
  xju::assert_equal(y.varResponses_.size(),1);
  xju::assert_equal(y.varResponses_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
  xju::assert_not_equal(dynamic_cast<NullValue const*>(&*y.varResponses_[0]),(NullValue const*)0);

  try {
    // decodeSnmpV2cResponse - var nosuchobject
    std::vector<uint8_t> x({
        0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA2,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x80,0x00
          });
    
    SnmpV2cResponse y(decodeSnmpV2cResponse(x));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResponses_.size(),1);
    xju::assert_equal(y.varResponses_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    try {
      xju::assert_not_equal(&*y.varResponses_[0],&*y.varResponses_[0]);
    }
    catch(SnmpV2cVarResponse::NoSuchObject const& e) {
      xju::assert_equal(readableRepr(e), "no such object .1.3.6.1.4.1.2680.1.2.7.3.2.0.");
    }
    catch(xju::Exception const& e) {
      xju::assert_not_equal(readableRepr(e), readableRepr(e));
    }
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  try {
    // decodeSnmpV2cResponse - var nosuchinstance
    std::vector<uint8_t> x({
        0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA2,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x81,0x00
          });
    
    SnmpV2cResponse y(decodeSnmpV2cResponse(x));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResponses_.size(),1);
    xju::assert_equal(y.varResponses_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    try {
      xju::assert_not_equal(&*y.varResponses_[0],&*y.varResponses_[0]);
    }
    catch(SnmpV2cVarResponse::NoSuchInstance const& e) {
      xju::assert_equal(readableRepr(e),"no such instance .1.3.6.1.4.1.2680.1.2.7.3.2.0.");
    }
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  try {
    // decodeSnmpV2cResponse - var endofmibview
    std::vector<uint8_t> x({
        0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA2,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x82,0x00
          });
    
    SnmpV2cResponse y(decodeSnmpV2cResponse(x));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResponses_.size(),1);
    xju::assert_equal(y.varResponses_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    try {
      xju::assert_not_equal(&*y.varResponses_[0],&*y.varResponses_[0]);
    }
    catch(SnmpV2cVarResponse::EndOfMibView const& e) {
      xju::assert_equal(readableRepr(e),"end of mib view at .1.3.6.1.4.1.2680.1.2.7.3.2.0.");
    }
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
}

void test2() throw()
{
  // decodeSnmpV2cResponse variations
  
  // definite long length eg long string
  try {
    std::vector<uint8_t> x({
        0x30,0x81,0xb0,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA2,0x81,0xA1,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x81,0x95,0x30,0x81,0x92,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x04,0x81,128,'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'
          });
    
    SnmpV2cResponse y(decodeSnmpV2cResponse(x));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResponses_.size(),1);
    xju::assert_equal(y.varResponses_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_equal(dynamic_cast<StringValue const&>(*y.varResponses_[0]).val_,std::string(128,'a'));
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  
  // indefinite length s1
  try {
    std::vector<uint8_t> x({
        0x30,0x80,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA2,0x81,0xA1,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x81,0x95,0x30,0x81,0x92,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x04,0x81,128,'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a',0,0
          });
    
    SnmpV2cResponse y(decodeSnmpV2cResponse(x));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResponses_.size(),1);
    xju::assert_equal(y.varResponses_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_equal(dynamic_cast<StringValue const&>(*y.varResponses_[0]).val_,std::string(128,'a'));
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  
  // indefinite length s2
  try {
    std::vector<uint8_t> x({
        0x30,0x81,0xb1,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA2,0x80,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x81,0x95,0x30,0x81,0x92,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x04,0x81,128,'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a',0,0
          });
    
    SnmpV2cResponse y(decodeSnmpV2cResponse(x));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResponses_.size(),1);
    xju::assert_equal(y.varResponses_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_equal(dynamic_cast<StringValue const&>(*y.varResponses_[0]).val_,std::string(128,'a'));
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  
  // indefinite length s3
  try {
    std::vector<uint8_t> x({
        0x30,0x81,0xb1,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA2,0x81,0xA2,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x80,0x30,0x81,0x92,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x04,0x81,128,'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a',0,0
          });
    
    SnmpV2cResponse y(decodeSnmpV2cResponse(x));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResponses_.size(),1);
    xju::assert_equal(y.varResponses_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_equal(dynamic_cast<StringValue const&>(*y.varResponses_[0]).val_,std::string(128,'a'));
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  
  // indefinite length s4
  try {
    std::vector<uint8_t> x({
        0x30,0x81,0xb1,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA2,0x81,0xA2,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x81,0x96,0x30,0x80,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x04,0x81,128,'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a',0,0
          });
    
    SnmpV2cResponse y(decodeSnmpV2cResponse(x));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResponses_.size(),1);
    xju::assert_equal(y.varResponses_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_equal(dynamic_cast<StringValue const&>(*y.varResponses_[0]).val_,std::string(128,'a'));
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  // indefinite length s4 and s3
  try {
    std::vector<uint8_t> x({
        0x30,0x81,0xb2,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA2,0x81,0xA3,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x80,0x30,0x80,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x04,0x81,128,'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a',0,0,0,0
          });
    
    SnmpV2cResponse y(decodeSnmpV2cResponse(x));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResponses_.size(),1);
    xju::assert_equal(y.varResponses_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_equal(dynamic_cast<StringValue const&>(*y.varResponses_[0]).val_,std::string(128,'a'));
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  // multi-byte int value
  try {
    std::vector<uint8_t> x({
        0x30,0x2D,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA2,0x1F,0x02,0x02,0x01,0x00,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
          });
    
    SnmpV2cResponse y(decodeSnmpV2cResponse(x));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(0x100));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResponses_.size(),1);
    xju::assert_equal(y.varResponses_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    dynamic_cast<NullValue const&>(*y.varResponses_[0]);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
    
  // oid value
  try {
    std::vector<uint8_t> x({
        0x30,53,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA2,39,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,28,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00,0x30,0x07,0x06,0x01,0x2B,0x06,0x02,0x2B,0x07
          });
    
    SnmpV2cResponse y(decodeSnmpV2cResponse(x));
    xju::assert_equal(y.responseType_,0xA2);
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.error_,SnmpV2cResponse::ErrorStatus::NO_ERROR);
    xju::assert_equal(y.errorIndex_,SnmpV2cResponse::ErrorIndex(0));
    xju::assert_equal(y.varResponses_.size(),2);
    xju::assert_equal(y.varResponses_[0].oid(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    dynamic_cast<NullValue const&>(*y.varResponses_[0]);
    xju::assert_equal(y.varResponses_[1].oid(),Oid(".1.3"));
    xju::assert_equal(dynamic_cast<OidValue const&>(*y.varResponses_[1]).val_,Oid(".1.3.7"));
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
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
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

