// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/decodeSnmpV3UsmSecurityParameters.hh>

#include <iostream>
#include <xju/assert.hh>
#include <vector>
#include <xju/snmp/SnmpV3UsmSecurityParameters.hh>
#include <xju/snmp/ContextEngineID.hh>
#include <xju/snmp/EngineTime.hh>
#include <xju/snmp/EngineBoots.hh>
#include <xju/UserName.hh>

namespace xju
{
namespace snmp
{

void test1() {
  std::vector<uint8_t> x({
      // msgSecurityParameters
      0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, // contextid
      0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00,
      0x02, 0x02, 0x07, 0x75, // engineboots
      0x02, 0x02, 0x60, 0xfd, // enginetime
      0x04, 0x04, 0x66, 0x72, 0x65, 0x64, //username
      0x04, 0x00,
      0x04, 0x00,
    });
  auto const y(decodeSnmpV3UsmSecurityParameters(x));
  xju::assert_equal(std::get<0>(y).engineID_,ContextEngineID(
                      std::vector<uint8_t>{0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, 0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00}));
  xju::assert_equal(std::get<0>(y).engineBoots_, EngineBoots(0x775));
  xju::assert_equal(std::get<0>(y).engineTime_, EngineTime(0x60fd));
  xju::assert_equal(std::get<0>(y).userName_, UserName("fred"));
  xju::assert_equal(std::get<1>(y), std::vector<uint8_t>{});
  xju::assert_equal(std::get<2>(y), std::vector<uint8_t>{});
}

void test2() throw()
{
  // failures
  try{
    std::vector<uint8_t> x({
        // msgSecurityParameters
        0x31, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, // contextid
        0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x07, 0x75, // engineboots
        0x02, 0x02, 0x60, 0xfd, // enginetime
        0x04, 0x04, 0x66, 0x72, 0x65, 0x64, //username
        0x04, 0x00,
        0x04, 0x00,
      });
    auto const y(decodeSnmpV3UsmSecurityParameters(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 USM Security Parameters from 39 bytes of data because\nexpected sequence type byte 0x30, got 0x31 at offset 0.");
  }
  try{
    std::vector<uint8_t> x({
        // msgSecurityParameters
        0x30, 0x26, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, // contextid
        0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x07, 0x75, // engineboots
        0x02, 0x02, 0x60, 0xfd, // enginetime
        0x04, 0x04, 0x66, 0x72, 0x65, 0x64, //username
        0x04, 0x00,
        0x04, 0x00,
      });
    auto const y(decodeSnmpV3UsmSecurityParameters(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 USM Security Parameters from 39 bytes of data having successfully decoded sequence type 0x30 and length 38 bytes at offset 0 because\nsequence length 38 does not match contained data length (37).");
  }
  try{
    std::vector<uint8_t> x({
        // msgSecurityParameters
        0x30, 0x28, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, // contextid
        0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x07, 0x75, // engineboots
        0x02, 0x02, 0x60, 0xfd, // enginetime
        0x04, 0x04, 0x66, 0x72, 0x65, 0x64, //username
        0x04, 0x00,
        0x04, 0x00,
        0xa0, 0xa1, 0xa2
      });
    auto const y(decodeSnmpV3UsmSecurityParameters(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 USM Security Parameters from 42 bytes of data having successfully decoded sequence type 0x30 and length 40 bytes at offset 0, snmp engine id (hex) [80,00,1f,88,80,e6,79,08,01,97,35,2e,5d,00,00,00,00] at offset 2, engine boots 1909 at offset 21, engine time 24829 at offset 25, user name \"fred\" at offset 29, auth params (hex) [] at offset 35, priv params (hex) [] at offset 37 because\n3 bytes extra data (hex) [a0,a1,a2] at offset 39.");
  }
  try{
    std::vector<uint8_t> x({
        // msgSecurityParameters
        0x30, 0x25, 0x02, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, // contextid
        0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x07, 0x75, // engineboots
        0x02, 0x02, 0x60, 0xfd, // enginetime
        0x04, 0x04, 0x66, 0x72, 0x65, 0x64, //username
        0x04, 0x00,
        0x04, 0x00,
      });
    auto const y(decodeSnmpV3UsmSecurityParameters(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 USM Security Parameters from 39 bytes of data having successfully decoded sequence type 0x30 and length 37 bytes at offset 0 because\nfailed to decode string at offset 2 because\ntype is 0x02 not 0x04.");
  }
  try{
    std::vector<uint8_t> x({
        // msgSecurityParameters
        0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, // contextid
        0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00,
        0x04, 0x02, 0x07, 0x75, // engineboots
        0x02, 0x02, 0x60, 0xfd, // enginetime
        0x04, 0x04, 0x66, 0x72, 0x65, 0x64, //username
        0x04, 0x00,
        0x04, 0x00,
      });
    auto const y(decodeSnmpV3UsmSecurityParameters(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 USM Security Parameters from 39 bytes of data having successfully decoded sequence type 0x30 and length 37 bytes at offset 0, snmp engine id (hex) [80,00,1f,88,80,e6,79,08,01,97,35,2e,5d,00,00,00,00] at offset 2 because\nfailed to decode integer at offset 21 because\ntype is 0x04 not 0x02.");
  }
  try{
    std::vector<uint8_t> x({
        // msgSecurityParameters
        0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, // contextid
        0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x07, 0x75, // engineboots
        0x03, 0x02, 0x60, 0xfd, // enginetime
        0x04, 0x04, 0x66, 0x72, 0x65, 0x64, //username
        0x04, 0x00,
        0x04, 0x00,
      });
    auto const y(decodeSnmpV3UsmSecurityParameters(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 USM Security Parameters from 39 bytes of data having successfully decoded sequence type 0x30 and length 37 bytes at offset 0, snmp engine id (hex) [80,00,1f,88,80,e6,79,08,01,97,35,2e,5d,00,00,00,00] at offset 2, engine boots 1909 at offset 21 because\nfailed to decode integer at offset 25 because\ntype is 0x03 not 0x02.");
  }
  try{
    std::vector<uint8_t> x({
        // msgSecurityParameters
        0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, // contextid
        0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x07, 0x75, // engineboots
        0x02, 0x02, 0x60, 0xfd, // enginetime
        0x02, 0x04, 0x66, 0x72, 0x65, 0x64, //username
        0x04, 0x00,
        0x04, 0x00,
      });
    auto const y(decodeSnmpV3UsmSecurityParameters(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 USM Security Parameters from 39 bytes of data having successfully decoded sequence type 0x30 and length 37 bytes at offset 0, snmp engine id (hex) [80,00,1f,88,80,e6,79,08,01,97,35,2e,5d,00,00,00,00] at offset 2, engine boots 1909 at offset 21, engine time 24829 at offset 25 because\nfailed to decode string at offset 29 because\ntype is 0x02 not 0x04.");
  }
  try{
    std::vector<uint8_t> x({
        // msgSecurityParameters
        0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, // contextid
        0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x07, 0x75, // engineboots
        0x02, 0x02, 0x60, 0xfd, // enginetime
        0x04, 0x04, 0x66, 0x72, 0x65, 0x64, //username
        0x02, 0x00,
        0x04, 0x00,
      });
    auto const y(decodeSnmpV3UsmSecurityParameters(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 USM Security Parameters from 39 bytes of data having successfully decoded sequence type 0x30 and length 37 bytes at offset 0, snmp engine id (hex) [80,00,1f,88,80,e6,79,08,01,97,35,2e,5d,00,00,00,00] at offset 2, engine boots 1909 at offset 21, engine time 24829 at offset 25, user name \"fred\" at offset 29 because\nfailed to decode string at offset 35 because\ntype is 0x02 not 0x04.");
  }
  
  try{
    std::vector<uint8_t> x({
        // msgSecurityParameters
        0x30, 0x25, 0x04, 0x11, 0x80, 0x00, 0x1f, 0x88, 0x80, 0xe6, // contextid
        0x79, 0x08, 0x01, 0x97, 0x35, 0x2e, 0x5d, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x07, 0x75, // engineboots
        0x02, 0x02, 0x60, 0xfd, // enginetime
        0x04, 0x04, 0x66, 0x72, 0x65, 0x64, //username
        0x04, 0x00,
        0x02, 0x00,
      });
    auto const y(decodeSnmpV3UsmSecurityParameters(x));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to decode snmp v3 USM Security Parameters from 39 bytes of data having successfully decoded sequence type 0x30 and length 37 bytes at offset 0, snmp engine id (hex) [80,00,1f,88,80,e6,79,08,01,97,35,2e,5d,00,00,00,00] at offset 2, engine boots 1909 at offset 21, engine time 24829 at offset 25, user name \"fred\" at offset 29, auth params (hex) [] at offset 35 because\nfailed to decode string at offset 37 because\ntype is 0x02 not 0x04.");
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
