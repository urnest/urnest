// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/decodeSnmpV3Message.hh>

#include <iostream>
#include <xju/assert.hh>
#include "xju/snmp/NullValue.hh"
#include "xju/snmp/StringValue.hh"
#include "xju/snmp/OidValue.hh"
#include "xju/next.hh"

namespace xju
{
namespace snmp
{
void test1() throw()
{
  // decodeSnmpV3Message
  std::vector<uint8_t> x({
      // ip header
      // 0x45, 0x00, 0x00, 0x9b, 0xce, 0x47, 0x40, 0x00, 0x40, 0x11, 0x6e, 0x08, 0x7f, 0x00, 0x00, 0x01,
      // 0x7f, 0x00, 0x00, 0x01,
      // udp header
      // 0xed, 0x7e, 0x00, 0xa1, 0x00, 0x87, 0xfe, 0x9a,
      // payload = SnmpV3Message
      0x30, 0x7d,
      // msgVersion 3
      0x02, 0x01, 0x03,
      // msgGlobalData
      0x30, 0x11,
      // msgID
      0x02, 0x04, 0x77, 0x2d, 0x37, 0x16,
      // msgMaxSize
      0x02, 0x03, 0x00, 0xff, 0xe3,
      // msgFlags
      0x04, 0x01, 0x04,
      // msgSecurityModel
      0x02, 0x01, 0x03,
      // msgSecurityParameters
      0x04, 0x27, 0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6,
      0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x07, 0x75, 0x02,
      0x02, 0x60, 0xfd, 0x04, 0x04, 0x66, 0x72, 0x65, 0x64, 0x04, 0x00, 0x04, 0x00,
      // msgData = ScopedPDU (unencrypted in this case)
      0x30, 0x3c,
      // contextEngineID
      0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = fred
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = Get PDU
      0xa0, 0x21, 0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06, 0x0d, 0x2b, 0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00, 0x05, 0x00
    });
  std::pair<SnmpV3Message,size_t> y(decodeSnmpV3Message(x));
  xju::assert_equal(y.first.id_,SnmpV3Message::ID(0x772d3716));
  xju::assert_equal(y.first.maxSize_,0xffe3);
  xju::assert_equal(y.first.flags_,SnmpV3Message::REPORTABLE);
  xju::assert_equal(y.first.securityModel_, SnmpV3Message::SecurityModel(3));
  xju::assert_equal(y.first.securityParameters_, SnmpV3SecParams(std::vector<uint8_t>{
      0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6,
        0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x07, 0x75, 0x02,
        0x02, 0x60, 0xfd, 0x04, 0x04, 0x66, 0x72, 0x65, 0x64, 0x04, 0x00, 0x04, 0x00}));
  xju::assert_equal(y.first.scopedPduData_, SnmpV3ScopedPduData(std::vector<uint8_t>{
      0x30, 0x3c,
      // contextEngineID
      0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00,
      // contextName = fred
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = Get PDU
      0xa0, 0x21, 0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06, 0x0d, 0x2b, 0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00, 0x05, 0x00
        }));
  xju::assert_equal(y.second, 26U);
}

void test2() throw()
{
  // decodeSnmpV3Message failures

  try {
    std::vector<uint8_t> x({
      0x31, 0x7d,
      // msgVersion 3
      0x02, 0x01, 0x03,
      // msgGlobalData
      0x30, 0x11,
      // msgID
      0x02, 0x04, 0x77, 0x2d, 0x37, 0x16,
      // msgMaxSize
      0x02, 0x03, 0x00, 0xff, 0xe3,
      // msgFlags
      0x04, 0x01, 0x04,
      // msgSecurityModel
      0x02, 0x01, 0x03,
      // msgSecurityParameters
      0x04, 0x27, 0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6,
      0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x07, 0x75, 0x02,
      0x02, 0x60, 0xfd, 0x04, 0x04, 0x66, 0x72, 0x65, 0x64, 0x04, 0x00, 0x04, 0x00,
      // msgData = ScopedPDU (unencrypted in this case)
      0x30, 0x3c,
      // contextEngineID
      0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = fred
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = Get PDU
      0xa0, 0x21, 0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06, 0x0d, 0x2b, 0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00, 0x05, 0x00});
    
    auto const y(decodeSnmpV3Message(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 message from 127 bytes of data because\nexpected sequence type byte 0x30, got 0x31 at offset 0.");
  }
  try {
    std::vector<uint8_t> x({
      0x30, 0x7d,
      // msgVersion 3
      0x02, 0x01, 0x02,
      // msgGlobalData
      0x30, 0x11,
      // msgID
      0x02, 0x04, 0x77, 0x2d, 0x37, 0x16,
      // msgMaxSize
      0x02, 0x03, 0x00, 0xff, 0xe3,
      // msgFlags
      0x04, 0x01, 0x04,
      // msgSecurityModel
      0x02, 0x01, 0x03,
      // msgSecurityParameters
      0x04, 0x27, 0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6,
      0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x07, 0x75, 0x02,
      0x02, 0x60, 0xfd, 0x04, 0x04, 0x66, 0x72, 0x65, 0x64, 0x04, 0x00, 0x04, 0x00,
      // msgData = ScopedPDU (unencrypted in this case)
      0x30, 0x3c,
      // contextEngineID
      0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = fred
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = Get PDU
      0xa0, 0x21, 0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06, 0x0d, 0x2b, 0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00, 0x05, 0x00});
    
    auto const y(decodeSnmpV3Message(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 message from 127 bytes of data having successfully decoded sequence type 0x30 and length 125 bytes because\nfailed to decode at offset 2 because\nexpected snmp version 0x03 but got 0x02.");
  }
  try {
    std::vector<uint8_t> x({
      0x30, 0x7d,
      // msgVersion 3
      0x02, 0x01, 0x03,
      // msgGlobalData
      0x30, 0x12,
      // msgID
      0x02, 0x05, 0x01, 0x77, 0x2d, 0x37, 0x16,
      // msgMaxSize
      0x02, 0x03, 0x00, 0xff, 0xe3,
      // msgFlags
      0x04, 0x01, 0x04,
      // msgSecurityModel
      0x02, 0x01, 0x03,
      // msgSecurityParameters
      0x04, 0x27, 0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6,
      0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x07, 0x75, 0x02,
      0x02, 0x60, 0xfd, 0x04, 0x04, 0x66, 0x72, 0x65, 0x64, 0x04, 0x00, 0x04, 0x00,
      // msgData = ScopedPDU (unencrypted in this case)
      0x30, 0x3c,
      // contextEngineID
      0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = fred
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = Get PDU
      0xa0, 0x21, 0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06, 0x0d, 0x2b, 0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00, 0x05, 0x00});
    
    auto const y(decodeSnmpV3Message(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 message from 128 bytes of data having successfully decoded sequence type 0x30 and length 125 bytes, snmp version 3 at offset 2, header data sequence type 0x30 and length 18 bytes at offset 5 because\nexpected message id < 2147483648, got 6294419222.");
  }
  try {
    std::vector<uint8_t> x({
      0x30, 0x7b,
      // msgVersion 3
      0x02, 0x01, 0x03,
      // msgGlobalData
      0x30, 0x11,
      // msgID
      0x02, 0x04, 0x77, 0x2d, 0x37, 0x16,
      // msgMaxSize
      0x02, 0x01, 0x22,
      // msgFlags
      0x04, 0x01, 0x04,
      // msgSecurityModel
      0x02, 0x01, 0x03,
      // msgSecurityParameters
      0x04, 0x27, 0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6,
      0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x07, 0x75, 0x02,
      0x02, 0x60, 0xfd, 0x04, 0x04, 0x66, 0x72, 0x65, 0x64, 0x04, 0x00, 0x04, 0x00,
      // msgData = ScopedPDU (unencrypted in this case)
      0x30, 0x3c,
      // contextEngineID
      0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = fred
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = Get PDU
      0xa0, 0x21, 0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06, 0x0d, 0x2b, 0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00, 0x05, 0x00});
    
    auto const y(decodeSnmpV3Message(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 message from 125 bytes of data having successfully decoded sequence type 0x30 and length 123 bytes, snmp version 3 at offset 2, header data sequence type 0x30 and length 17 bytes at offset 5, request id 1999451926 at offset 7 because\nexpected max size >= 484, got 34.");
  }
  try {
    std::vector<uint8_t> x({
      0x30, 0x7d,
      // msgVersion 3
      0x02, 0x01, 0x03,
      // msgGlobalData
      0x30, 0x11,
      // msgID
      0x02, 0x04, 0x77, 0x2d, 0x37, 0x16,
      // msgMaxSize
      0x02, 0x03, 0x00, 0xff, 0xe3,
      // msgFlags
      0x02, 0x01, 0x04,
      // msgSecurityModel
      0x02, 0x01, 0x03,
      // msgSecurityParameters
      0x04, 0x27, 0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6,
      0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x07, 0x75, 0x02,
      0x02, 0x60, 0xfd, 0x04, 0x04, 0x66, 0x72, 0x65, 0x64, 0x04, 0x00, 0x04, 0x00,
      // msgData = ScopedPDU (unencrypted in this case)
      0x30, 0x3c,
      // contextEngineID
      0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = fred
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = Get PDU
      0xa0, 0x21, 0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06, 0x0d, 0x2b, 0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00, 0x05, 0x00});
    
    auto const y(decodeSnmpV3Message(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 message from 127 bytes of data having successfully decoded sequence type 0x30 and length 125 bytes, snmp version 3 at offset 2, header data sequence type 0x30 and length 17 bytes at offset 5, request id 1999451926 at offset 7, max size 65507 at offset 13 because\nfailed to decode string at offset 18 because\ntype is 0x02 not 0x04.");
  }
  try {
    std::vector<uint8_t> x({
      0x30, 0x7d,
      // msgVersion 3
      0x02, 0x01, 0x03,
      // msgGlobalData
      0x30, 0x11,
      // msgID
      0x02, 0x04, 0x77, 0x2d, 0x37, 0x16,
      // msgMaxSize
      0x02, 0x03, 0x00, 0xff, 0xe3,
      // msgFlags
      0x04, 0x01, 0x04,
      // msgSecurityModel
      0x02, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00,
      // msgSecurityParameters
      0x04, 0x23, 0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6,
      0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x07, 0x75, 0x02,
      0x02, 0x60, 0xfd, 0x04, 0x04, 0x66, 0x72, 0x65, 0x64,
      // msgData = ScopedPDU (unencrypted in this case)
      0x30, 0x3c,
      // contextEngineID
      0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = fred
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = Get PDU
      0xa0, 0x21, 0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06, 0x0d, 0x2b, 0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00, 0x05, 0x00});
    
    auto const y(decodeSnmpV3Message(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 message from 127 bytes of data having successfully decoded sequence type 0x30 and length 125 bytes, snmp version 3 at offset 2, header data sequence type 0x30 and length 17 bytes at offset 5, request id 1999451926 at offset 7, max size 65507 at offset 13, message flags (hex) 04 at offset 18 because\nexpected security model < 2147483648, got 4294967296.");
  }
  try {
    std::vector<uint8_t> x({
      0x30, 0x7d,
      // msgVersion 3
      0x02, 0x01, 0x03,
      // msgGlobalData
      0x30, 0x11,
      // msgID
      0x02, 0x04, 0x77, 0x2d, 0x37, 0x16,
      // msgMaxSize
      0x02, 0x03, 0x00, 0xff, 0xe3,
      // msgFlags
      0x04, 0x01, 0x04,
      // msgSecurityModel
      0x02, 0x01, 0x03,
      // msgSecurityParameters
      0x02, 0x27, 0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6,
      0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x07, 0x75, 0x02,
      0x02, 0x60, 0xfd, 0x04, 0x04, 0x66, 0x72, 0x65, 0x64, 0x04, 0x00, 0x04, 0x00,
      // msgData = ScopedPDU (unencrypted in this case)
      0x30, 0x3c,
      // contextEngineID
      0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = fred
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = Get PDU
      0xa0, 0x21, 0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06, 0x0d, 0x2b, 0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00, 0x05, 0x00});
    
    auto const y(decodeSnmpV3Message(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 message from 127 bytes of data having successfully decoded sequence type 0x30 and length 125 bytes, snmp version 3 at offset 2, header data sequence type 0x30 and length 17 bytes at offset 5, request id 1999451926 at offset 7, max size 65507 at offset 13, message flags (hex) 04 at offset 18, security model 3 at offset 21 because\nfailed to decode string at offset 24 because\ntype is 0x02 not 0x04.");
  }
  try {
    std::vector<uint8_t> x({
      0x30, 0x7d,
      // msgVersion 3
      0x02, 0x01, 0x03,
      // msgGlobalData
      0x30, 0x11,
      // msgID
      0x02, 0x04, 0x77, 0x2d, 0x37, 0x16,
      // msgMaxSize
      0x02, 0x03, 0x00, 0xff, 0xe3,
      // msgFlags
      0x04, 0x01, 0x07,
      // msgSecurityModel
      0x02, 0x01, 0x03,
      // msgSecurityParameters
      0x04, 0x27, 0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6,
      0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x07, 0x75, 0x02,
      0x02, 0x60, 0xfd, 0x04, 0x04, 0x66, 0x72, 0x65, 0x64, 0x04, 0x00, 0x04, 0x00,
      // msgData = ScopedPDU (unencrypted in this case)
      0x30, 0x3c,
      // contextEngineID
      0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = fred
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = Get PDU
      0xa0, 0x21, 0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06, 0x0d, 0x2b, 0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00, 0x05, 0x00});
    
    auto const y(decodeSnmpV3Message(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 message from 127 bytes of data having successfully decoded sequence type 0x30 and length 125 bytes, snmp version 3 at offset 2, header data sequence type 0x30 and length 17 bytes at offset 5, request id 1999451926 at offset 7, max size 65507 at offset 13, message flags (hex) 07 at offset 18, security model 3 at offset 21, security parameters (hex) 30,25,04,11,80,00,1f,88,80,e6,79,08,01,97,35,2e,5d,00,00,00,00,02,02,07,75,02,02,60,fd,04,04,66,72,65,64,04,00,04,00 at offset 24 because\nexpected encrypted ScopedPDU to have type 0x04, got 0x30.");
  }
  try {
    std::vector<uint8_t> x({
      0x30, 0x7d,
      // msgVersion 3
      0x02, 0x01, 0x03,
      // msgGlobalData
      0x30, 0x11,
      // msgID
      0x02, 0x04, 0x77, 0x2d, 0x37, 0x16,
      // msgMaxSize
      0x02, 0x03, 0x00, 0xff, 0xe3,
      // msgFlags
      0x04, 0x01, 0x04,
      // msgSecurityModel
      0x02, 0x01, 0x03,
      // msgSecurityParameters
      0x04, 0x27, 0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6,
      0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x07, 0x75, 0x02,
      0x02, 0x60, 0xfd, 0x04, 0x04, 0x66, 0x72, 0x65, 0x64, 0x04, 0x00, 0x04, 0x00,
      // msgData = ScopedPDU
      0x04, 0x3c,
      // contextEngineID
      0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = fred
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = Get PDU
      0xa0, 0x21, 0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06, 0x0d, 0x2b, 0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00, 0x05, 0x00});
    
    auto const y(decodeSnmpV3Message(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 message from 127 bytes of data having successfully decoded sequence type 0x30 and length 125 bytes, snmp version 3 at offset 2, header data sequence type 0x30 and length 17 bytes at offset 5, request id 1999451926 at offset 7, max size 65507 at offset 13, message flags (hex) 04 at offset 18, security model 3 at offset 21, security parameters (hex) 30,25,04,11,80,00,1f,88,80,e6,79,08,01,97,35,2e,5d,00,00,00,00,02,02,07,75,02,02,60,fd,04,04,66,72,65,64,04,00,04,00 at offset 24 because\nexpected unencrypted ScopedPDU to have type 0x30, got 0x04.");
  }
  try {
    std::vector<uint8_t> x({
      0x30, 0x7d,
      // msgVersion 3
      0x02, 0x01, 0x03,
      // msgGlobalData
      0x30, 0x11,
      // msgID
      0x02, 0x04, 0x77, 0x2d, 0x37, 0x16,
      // msgMaxSize
      0x02, 0x03, 0x00, 0xff, 0xe3,
      // msgFlags
      0x04, 0x01, 0x04,
      // msgSecurityModel
      0x02, 0x01, 0x03,
      // msgSecurityParameters
      0x04, 0x27, 0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6,
      0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x07, 0x75, 0x02,
      0x02, 0x60, 0xfd, 0x04, 0x04, 0x66, 0x72, 0x65, 0x64, 0x04, 0x00, 0x04, 0x00,
      // msgData = ScopedPDU (unencrypted in this case)
      0x30, 0x3d,
      // contextEngineID
      0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = fred
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = Get PDU
      0xa0, 0x21, 0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      0x02, 0x01, 0x00, 0x02, 0x01, 0x00, 0x30, 0x13, 0x30, 0x11, 0x06, 0x0d, 0x2b, 0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00, 0x05, 0x00});
    
    auto const y(decodeSnmpV3Message(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 message from 127 bytes of data having successfully decoded sequence type 0x30 and length 125 bytes, snmp version 3 at offset 2, header data sequence type 0x30 and length 17 bytes at offset 5, request id 1999451926 at offset 7, max size 65507 at offset 13, message flags (hex) 04 at offset 18, security model 3 at offset 21, security parameters (hex) 30,25,04,11,80,00,1f,88,80,e6,79,08,01,97,35,2e,5d,00,00,00,00,02,02,07,75,02,02,60,fd,04,04,66,72,65,64,04,00,04,00 at offset 24, scoped pdu type 0x30 and length 61 bytes at offset 65 because\nexpected 61 bytes of scoped pdu data but have 60.");
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

