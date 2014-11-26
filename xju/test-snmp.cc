// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "snmp.hh"

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

void test1() throw() {
  Oid x(".1.3.6.1.4.364");
  xju::assert_equal(x.toString(), ".1.3.6.1.4.364");
  std::vector<uint32_t> b;
  b.push_back(1);
  b.push_back(3);
  b.push_back(6);
  b.push_back(1);
  b.push_back(4);
  b.push_back(364);
  xju::assert_equal(x.components(),b);
  Oid y(b);
  xju::assert_equal(y.toString(), x.toString());
  xju::assert_equal(x,y);
  std::vector<uint32_t> c(b);
  c.push_back(3);
  Oid a(c);
  xju::assert_not_equal(x, a);
  xju::assert_less(x, a);

  Oid a2(x+Oid(".3"));
  xju::assert_equal(a2, a);
}

void test2()
{
  try {
    Oid x("");
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e), "Failed to parse \"\" assumed to be an SNMP OID in dotted notation, eg .1.3.6.1.4.364 because\n\"\" does not start with '.'.");
  }
  try {
    Oid x("34.53");
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e), "Failed to parse \"34.53\" assumed to be an SNMP OID in dotted notation, eg .1.3.6.1.4.364 because\n\"34.53\" does not start with '.'.");
  }
  try {
    Oid x(".34.");
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e), "Failed to parse \".34.\" assumed to be an SNMP OID in dotted notation, eg .1.3.6.1.4.364 because\nfailed to parse component \"\" (at offset 4) because\nfailed to convert \"\" to a base-10 unsigned integer because\n\"\" is null.");
  }
  try {
    Oid x(".ac");
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e), "Failed to parse \".ac\" assumed to be an SNMP OID in dotted notation, eg .1.3.6.1.4.364 because\nfailed to parse component \"ac\" (at offset 1) because\nfailed to convert \"ac\" to a base-10 unsigned integer because\ncharacter 1 ('a') of \"ac\" unexpected.");
  }
  
}

}

using namespace xju;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

