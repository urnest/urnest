// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/snmp/decodeSnmpV2cGetBulkRequest.hh"

#include <iostream>
#include <xju/assert.hh>
#include "xju/snmp/NullValue.hh"
#include "xju/snmp/StringValue.hh"
#include "xju/snmp/OidValue.hh"
#include "xju/next.hh"
#include <xju/snmp/SnmpV2cGetBulkRequest.hh>

namespace xju
{
namespace snmp
{
void test1() throw()
{
  // decodeSnmpV2cGetBulkRequest
  std::vector<uint8_t> x({
      0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65, //private
      0xA5,0x1E,
      0x02,0x01,0x01, //requestid
      0x02,0x01,0x01, //repeaters
      0x02,0x01,0x00, //max-repetitions
      0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
        });
  SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
  xju::assert_equal(y.community_,Community("private"));
  xju::assert_equal(y.id_,RequestId(1));
  xju::assert_equal(y.getNext_.size(),1);
  xju::assert_equal(*y.getNext_.begin(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
  xju::assert_equal(y.n_,0U);
  xju::assert_equal(y.getNextN_.size(),0U);
}
void test2() throw()
{
  // decodeSnmpV2cGetBulkRequest variations
  
  // definite long length eg long string
  try {
    std::vector<uint8_t> x({
        0x30,0x81,0xb0,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,
        0xA5,0x81,0xA1,
        0x02,0x01,0x01,
        0x02,0x01,0x00,
        0x02,0x01,0x04,
        0x30,0x81,0x95,0x30,0x81,0x92,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x04,0x81,128,'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a'
          });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.getNext_.size(),0);
    xju::assert_equal(y.n_,4);
    xju::assert_equal(*y.getNextN_.begin(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  
  // indefinite length s1
  try {
    std::vector<uint8_t> x({
        0x30,0x80,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,
        0xA5,0x81,0xA1,
        0x02,0x01,0x01,
        0x02,0x01,0x01,
        0x02,0x01,0x00,
        0x30,0x81,0x95,0x30,0x81,0x92,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x04,0x81,128,'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a',0,0
          });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.getNext_.size(),1);
    xju::assert_equal(*y.getNext_.begin(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_equal(y.n_, 0U);
    xju::assert_equal(y.getNextN_.size(), 0U);
    
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  
  // indefinite length s2
  try {
    std::vector<uint8_t> x({
        0x30,0x81,0xb1,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,
        0xA5,0x80,
        0x02,0x01,0x01,
        0x02,0x01,0x01,
        0x02,0x01,0x00,
        0x30,0x81,0x95,0x30,0x81,0x92,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x04,0x81,128,'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a',0,0
          });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.getNext_.size(),1);
    xju::assert_equal(*y.getNext_.begin(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_equal(y.n_,0U);
    xju::assert_equal(y.getNextN_.size(),0U);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  
  // indefinite length s3
  try {
    std::vector<uint8_t> x({
        0x30,0x81,0xb1,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,
        0xA5,0x81,0xA2,
        0x02,0x01,0x01,
        0x02,0x01,0x01,
        0x02,0x01,0x00,
        0x30,0x80,0x30,0x81,0x92,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x04,0x81,128,'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a',0,0
          });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.getNext_.size(),1);
    xju::assert_equal(*y.getNext_.begin(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_equal(y.n_,0U);
    xju::assert_equal(y.getNextN_.size(),0U);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
  
  // indefinite length s4
  try {
    std::vector<uint8_t> x({
        0x30,0x81,0xb1,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,
        0xA5,0x81,0xA2,
        0x02,0x01,0x01,
        0x02,0x01,0x01,
        0x02,0x01,0x00,
        0x30,0x81,0x96,0x30,0x80,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x04,0x81,128,'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a',0,0
          });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.getNext_.size(),1);
    xju::assert_equal(*y.getNext_.begin(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_equal(y.n_,0U);
    xju::assert_equal(y.getNextN_.size(),0U);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  // indefinite length s4 and s3
  try {
    std::vector<uint8_t> x({
        0x30,0x81,0xb2,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,
        0xA5,0x81,0xA3,
        0x02,0x01,0x01,
        0x02,0x01,0x01,
        0x02,0x01,0x00,
        0x30,0x80,0x30,0x80,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x04,0x81,128,'a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a','a',0,0,0,0
          });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.getNext_.size(),1);
    xju::assert_equal(*y.getNext_.begin(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_equal(y.n_,0U);
    xju::assert_equal(y.getNextN_.size(),0U);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  // multi-byte int value
  try {
    std::vector<uint8_t> x({
        0x30,0x2D,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,
        0xA5,0x1F,
        0x02,0x02,0x01,0x00,
        0x02,0x01,0x01,
        0x02,0x01,0x00,
        0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
          });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(0x100));
    xju::assert_equal(y.getNext_.size(),1);
    xju::assert_equal(*y.getNext_.begin(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_equal(y.n_,0U);
    xju::assert_equal(y.getNextN_.size(),0U);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
    
  // oid value
  try {
    std::vector<uint8_t> x({
        0x30,53,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,
        0xA5,39,
        0x02,0x01,0x01,
        0x02,0x01,0x02,
        0x02,0x01,0x00,
        0x30,28,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00,0x30,0x07,0x06,0x01,0x2B,0x06,0x02,0x2B,0x07
          });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
    xju::assert_equal(y.community_,Community("private"));
    xju::assert_equal(y.id_,RequestId(1));
    xju::assert_equal(y.getNext_.size(),2);
    xju::assert_equal(*y.getNext_.begin(),Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"));
    xju::assert_equal(*xju::next(y.getNext_.begin()),Oid(".1.3"));
    xju::assert_equal(y.n_,0U);
    xju::assert_equal(y.getNextN_.size(),0U);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }
    
}

void test3() throw()
{
  // decodeSnmpV2cGetBulkRequest failures

  try {
    std::vector<uint8_t> x({
        0x31,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,
        0xA5,0x1E,
        0x02,0x01,0x01,
        0x02,0x01,0x00,
        0x02,0x01,0x00,
        0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
          });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v2c get bulk request from 46 bytes of data because\nexpected sequence type byte 0x30, got 0x31 at offset 0.");
  }

  try {
    std::vector<uint8_t> x({
        0x30,0x2c,0x03,0x01,0x00,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA5,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
        });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v2c get bulk request from 46 bytes of data having successfully decoded sequence type 0x30 and length 44 bytes because\nfailed to decode integer at offset 2 because\ntype is 0x03 not 0x02.");
  }

  try {
    std::vector<uint8_t> x({
        0x30,0x2c,0x02,0x01,0x00,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA5,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
        });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
  }
  catch(SnmpVersionMismatch const& e) {
    xju::assert_equal(e.cause().first,"expected snmp version 1 (SNMP V2) but got 0 (SNMP V1)");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e),readableRepr(e));
  }

  try {
    std::vector<uint8_t> x({
        0x30,0x2c,0x02,0x01,0x01,0x02,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA5,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
        });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v2c get bulk request from 46 bytes of data having successfully decoded sequence type 0x30 and length 44 bytes, snmp version 2c at offset 2 because\nfailed to decode string at offset 5 because\ntype is 0x02 not 0x04.");
  }

  try {
    std::vector<uint8_t> x({
        0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA5,0x1E,0xa2,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
        });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v2c get bulk request from 46 bytes of data having successfully decoded sequence type 0x30 and length 44 bytes, snmp version 2c at offset 2, community \"private\" at offset 5, 2nd sequence type 0xa5 and length 30 bytes at offset 14 because\nfailed to decode integer at offset 16 because\ntype is 0xa2 not 0x02.");
  }

  try {
    std::vector<uint8_t> x({
        0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA5,0x1E,0x02,0x01,0x01,0x01,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
        });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v2c get bulk request from 46 bytes of data having successfully decoded sequence type 0x30 and length 44 bytes, snmp version 2c at offset 2, community \"private\" at offset 5, 2nd sequence type 0xa5 and length 30 bytes at offset 14, request id 1 at offset 16 because\nfailed to decode integer at offset 19 because\ntype is 0x01 not 0x02.");
  }

  try {
    std::vector<uint8_t> x({
        0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA5,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x09,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
        });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v2c get bulk request from 46 bytes of data having successfully decoded sequence type 0x30 and length 44 bytes, snmp version 2c at offset 2, community \"private\" at offset 5, 2nd sequence type 0xa5 and length 30 bytes at offset 14, request id 1 at offset 16, number of non repeaters 0x0000000000000000 at offset 19 because\nfailed to decode integer at offset 22 because\ncan only handle 8-byte integers, not 9.");
  }

  try {
    std::vector<uint8_t> x({
        0x30,0x30,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA5,0x22,0x02,0x01,0x01,0x02,0x05,0x01,0x00,0x00,0x00,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
        });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v2c get bulk request from 50 bytes of data having successfully decoded sequence type 0x30 and length 48 bytes, snmp version 2c at offset 2, community \"private\" at offset 5, 2nd sequence type 0xa5 and length 34 bytes at offset 14, request id 1 at offset 16, number of non repeaters 0x0000000100000000 at offset 19, max repetitions 0 at offset 26, 3rd sequence type 0x30 and length 19 bytes at offset 29 because\nerror status 4294967296 exceeds maximimum supported (2147483647).");
  }

  try {
    std::vector<uint8_t> x({
        0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA5,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x01,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
        });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v2c get bulk request from 46 bytes of data having successfully decoded sequence type 0x30 and length 44 bytes, snmp version 2c at offset 2, community \"private\" at offset 5, 2nd sequence type 0xa5 and length 30 bytes at offset 14, request id 1 at offset 16, number of non repeaters 0x0000000000000000 at offset 19 because\nfailed to decode integer at offset 22 because\ntype is 0x01 not 0x02.");
  }

  try {
    std::vector<uint8_t> x({
        0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA5,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x03,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
        });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v2c get bulk request from 46 bytes of data having successfully decoded sequence type 0x30 and length 44 bytes, snmp version 2c at offset 2, community \"private\" at offset 5, 2nd sequence type 0xa5 and length 30 bytes at offset 14, request id 1 at offset 16, number of non repeaters 0x0000000000000000 at offset 19, max repetitions 0 at offset 22, 3rd sequence type 0x30 and length 19 bytes at offset 25 because\nfailed to decode param oid and value sequence at offset offset 27 because\nfailed to decode oid at offset 29 because\ntype is 0x03 not 0x06.");
  }

  try {
    std::vector<uint8_t> x({
        0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA5,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x30,0x00
        });
    
    SnmpV2cGetBulkRequest y(decodeSnmpV2cGetBulkRequest(x));
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v2c get bulk request from 46 bytes of data having successfully decoded sequence type 0x30 and length 44 bytes, snmp version 2c at offset 2, community \"private\" at offset 5, 2nd sequence type 0xa5 and length 30 bytes at offset 14, request id 1 at offset 16, number of non repeaters 0x0000000000000000 at offset 19, max repetitions 0 at offset 22, 3rd sequence type 0x30 and length 19 bytes at offset 25 because\nfailed to decode param oid and value sequence at offset offset 27 because\nfailed to decode one int/string/oid/null etc value at offset 44 because\ndecoding of type 0x30 is not implemented.");
  }
  try {
    std::vector<uint8_t> x({
        0x30,0x2c,0x02,0x01,0x01,0x04,0x07,0x70,0x72,0x69,0x76,0x61,0x74,0x65,0xA0,0x1E,0x02,0x01,0x01,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x13,0x30,0x11,0x06,0x0D,0x2B,0x06,0x01,0x04,0x01,0x94,0x78,0x01,0x02,0x07,0x03,0x02,0x00,0x05,0x00
          });
  }
  catch(RequestTypeMismatch const& e) {
    xju::assert_equal(e.cause().first,"expected request of type 0xa1 but got request of type 0xa0");
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
  test3(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

