// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/decodeSnmpV3ScopedPDU.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/snmp/NullValue.hh>
#include <xju/snmp/StringValue.hh>
#include <xju/snmp/OidValue.hh>
#include <xju/next.hh>

namespace xju
{
namespace snmp
{
void test1() throw()
{
  // decodeSnmpV3ScopedPDU
  SnmpV3ScopedPduData const x(std::vector<uint8_t>{
      0x30, 0x3c,
      // contextEngineID
      0x04, 0x11,
      0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = jock
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = V2 Get PDU = content of SnmpV2cGetRequest starting with request id (see RFC3416 PDU)
      0xa0, 0x21,
      // request-id
      0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      // error-status
      0x02, 0x01, 0x00,
      // error-index
      0x02, 0x01, 0x00,
      // variable-bindings
      0x30, 0x13,
      // .1.3.6.1.4.1.2680.1.2.7.3.2.0
      0x30, 0x11, 0x06, 0x0d, 0x2b,
      0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00,
      // type+length
      0x05, 0x00
  });
  SnmpV3ScopedPDU y(decodeSnmpV3ScopedPDU(x));
  xju::assert_equal(y.contextEngineID_, ContextEngineID(std::vector<uint8_t>{
        0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
          0x00, 0x00,}));
  xju::assert_equal(y.contextName_, ContextName("jock"));
  xju::assert_equal(y.pdu_,
                    PDU(RequestId(0x690f79b6),
                        0x00,
                        0x00,
                        // variable-bindings
                        {SnmpVar(Oid(".1.3.6.1.4.1.2680.1.2.7.3.2.0"),
                                 std::shared_ptr<Value const>(new NullValue()))},
                        0xa0));
}
    
void test2()
{
  // decodeSnmpV3ScopedPDU failures
  try {
    SnmpV3ScopedPduData const x(std::vector<uint8_t>{
      0x31, 0x3c,
      // contextEngineID
      0x04, 0x11,
      0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = jock
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = V2 Get PDU = content of SnmpV2cGetRequest starting with request id (see RFC3416 PDU)
      0xa0, 0x21,
      // request-id
      0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      // error-status
      0x02, 0x01, 0x00,
      // error-index
      0x02, 0x01, 0x00,
      // variable-bindings
      0x30, 0x13,
      // .1.3.6.1.4.1.2680.1.2.7.3.2.0
      0x30, 0x11, 0x06, 0x0d, 0x2b,
      0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00,
      // type+length
      0x05, 0x00});
    
    SnmpV3ScopedPDU y(decodeSnmpV3ScopedPDU(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 scoped pdu from 62 bytes of data because\nexpected sequence type byte 0x30, got 0x31 at offset 0.");
  }
  try {
    SnmpV3ScopedPduData const x(std::vector<uint8_t>{
      0x30, 0x3d,
      // contextEngineID
      0x04, 0x11,
      0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = jock
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = V2 Get PDU = content of SnmpV2cGetRequest starting with request id (see RFC3416 PDU)
      0xa0, 0x21,
      // request-id
      0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      // error-status
      0x02, 0x01, 0x00,
      // error-index
      0x02, 0x01, 0x00,
      // variable-bindings
      0x30, 0x13,
      // .1.3.6.1.4.1.2680.1.2.7.3.2.0
      0x30, 0x11, 0x06, 0x0d, 0x2b,
      0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00,
      // type+length
      0x05, 0x00});
    
    SnmpV3ScopedPDU y(decodeSnmpV3ScopedPDU(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 scoped pdu from 62 bytes of data having successfully decoded sequence type 0x30 and length 61 bytes because\nsequence length 61 is greater than contained data length (60).");
  }
  try {
    SnmpV3ScopedPduData const x(std::vector<uint8_t>{
      0x30, 0x3c,
      // contextEngineID
      0x02, 0x11,
      0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = jock
      0x04, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = V2 Get PDU = content of SnmpV2cGetRequest starting with request id (see RFC3416 PDU)
      0xa0, 0x21,
      // request-id
      0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      // error-status
      0x02, 0x01, 0x00,
      // error-index
      0x02, 0x01, 0x00,
      // variable-bindings
      0x30, 0x13,
      // .1.3.6.1.4.1.2680.1.2.7.3.2.0
      0x30, 0x11, 0x06, 0x0d, 0x2b,
      0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00,
      // type+length
      0x05, 0x00});
    
    SnmpV3ScopedPDU y(decodeSnmpV3ScopedPDU(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 scoped pdu from 62 bytes of data having successfully decoded sequence type 0x30 and length 60 bytes because\nfailed to decode string at offset 2 because\ntype is 0x02 not 0x04.");
  }
  try {
    SnmpV3ScopedPduData const x(std::vector<uint8_t>{
      0x30, 0x3c,
      // contextEngineID
      0x04, 0x11,
      0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00,
      0x00, 0x00,
      // contextName = jock
      0x02, 0x04, 0x6a, 0x6f, 0x63, 0x6b,
      // data = V2 Get PDU = content of SnmpV2cGetRequest starting with request id (see RFC3416 PDU)
      0xa0, 0x21,
      // request-id
      0x02, 0x04, 0x69, 0x0f, 0x79, 0xb6,
      // error-status
      0x02, 0x01, 0x00,
      // error-index
      0x02, 0x01, 0x00,
      // variable-bindings
      0x30, 0x13,
      // .1.3.6.1.4.1.2680.1.2.7.3.2.0
      0x30, 0x11, 0x06, 0x0d, 0x2b,
      0x06, 0x01, 0x04,
      0x01, 0x94, 0x78, 0x01, 0x02, 0x07, 0x03, 0x02, 0x00,
      // type+length
      0x05, 0x00});
    
    SnmpV3ScopedPDU y(decodeSnmpV3ScopedPDU(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 scoped pdu from 62 bytes of data having successfully decoded sequence type 0x30 and length 60 bytes, snmp engine id at offset 2 because\nfailed to decode string at offset 21 because\ntype is 0x02 not 0x04.");
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

