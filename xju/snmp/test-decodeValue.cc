// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/decodeValue.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/snmp/NullValue.hh>
#include <xju/snmp/IntValue.hh>
#include <xju/snmp/StringValue.hh>
#include <xju/snmp/OidValue.hh>
#include <xju/snmp/Counter32Value.hh>
#include <xju/snmp/Gauge32Value.hh>
#include <xju/snmp/Counter64Value.hh>
#include <xju/snmp/OpaqueValue.hh>

namespace xju
{
namespace snmp
{

void test1() {

  // NullValue encoding
  {
    NullValue const x;
    std::vector<uint8_t> y(x.encodedLength(),0U);
    xju::assert_equal(y.size(),2U);
    xju::assert_equal(x.encodeTo(y.begin()),y.end());
    xju::assert_equal(x,*decodeValue(DecodeIterator(y)).first);
  }
  // IntValue encoding
  // 0
  {
    IntValue const x(0);
    std::vector<uint8_t> y(x.encodedLength(),0U);
    xju::assert_equal(y.size(),3U);
    xju::assert_equal(x.encodeTo(y.begin()),y.end());
    xju::assert_equal(y,std::vector<uint8_t>{0x02,1,0});
    xju::assert_equal(x,*decodeValue(DecodeIterator(y)).first);
  }
  // StringValue encoding
  {
    StringValue const x(std::vector<uint8_t> {'f','r','e','d'});
    std::vector<uint8_t> y(x.encodedLength(),0U);
    xju::assert_equal(y.size(),6U);
    xju::assert_equal(x.encodeTo(y.begin()),y.end());
    xju::assert_equal(y,std::vector<uint8_t>{0x04,4,'f','r','e','d'});
    xju::assert_equal(x,*decodeValue(DecodeIterator(y)).first);
  }
  // OidValue encoding
  // .1.3
  {
    OidValue const x(Oid(".1.3"));
    std::vector<uint8_t> y(x.encodedLength(),0U);
    xju::assert_equal(y.size(),3U);
    xju::assert_equal(x.encodeTo(y.begin()),y.end());
    xju::assert_equal(y,std::vector<uint8_t>{0x06,1,40*1+3});
    xju::assert_equal(x,*decodeValue(DecodeIterator(y)).first);
  }
  // Counter32
  {
    Counter32Value const x(0x7fffffff);
    std::vector<uint8_t> y(x.encodedLength(),0U);
    xju::assert_equal(y.size(),6U);
    xju::assert_equal(x.encodeTo(y.begin()),y.end());
    xju::assert_equal(y,std::vector<uint8_t>{0x41,4,0x7f,0xff,0xff,0xff});
    xju::assert_equal(x,*decodeValue(DecodeIterator(y)).first);
  }
  // Gauge32
  {
    Gauge32Value const x(0x7fffffff);
    std::vector<uint8_t> y(x.encodedLength(),0U);
    xju::assert_equal(y.size(),6U);
    xju::assert_equal(x.encodeTo(y.begin()),y.end());
    xju::assert_equal(y,std::vector<uint8_t>{0x42,4,0x7f,0xff,0xff,0xff});
    xju::assert_equal(x,*decodeValue(DecodeIterator(y)).first);
  }
  // Counter64
  {
    Counter64Value const x(0x7fffffff);
    std::vector<uint8_t> y(x.encodedLength(),0U);
    xju::assert_equal(y.size(),6U);
    xju::assert_equal(x.encodeTo(y.begin()),y.end());
    xju::assert_equal(y,std::vector<uint8_t>{0x46,4,0x7f,0xff,0xff,0xff});
    xju::assert_equal(x,*decodeValue(DecodeIterator(y)).first);
  }
  // OpaqueValue
  {
    OpaqueValue const x({'f','r','e','d'});
    std::vector<uint8_t> y(x.encodedLength(),0U);
    xju::assert_equal(y.size(),6U);
    xju::assert_equal(x.encodeTo(y.begin()),y.end());
    xju::assert_equal(y,std::vector<uint8_t>{0x44,4,'f','r','e','d'});
    xju::assert_equal(x,*decodeValue(DecodeIterator(y)).first);
  }
}

}
}

using namespace xju::snmp;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

