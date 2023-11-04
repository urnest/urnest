// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/snmp/SnmpVar.hh"

#include <iostream>
#include <xju/assert.hh>
#include "xju/snmp/IntValue.hh"

namespace xju
{
namespace snmp
{

void test1() {
  SnmpVar const x(Oid(".1.3.4"),std::shared_ptr<IntValue const>(
                               new IntValue(5)));
  xju::assert_equal(x.oid(),Oid(".1.3.4"));
  xju::assert_equal(dynamic_cast<IntValue const&>(*x).val_,5);
  xju::assert_equal(x.value()->intValue(),5);
}

void test2() {
  SnmpVar const x(
    Oid(".1.3.4"),
    SnmpVar::NoSuchObject(Oid(".1.3.4"),XJU_TRACED));
  try {
    auto y(dynamic_cast<IntValue const*>(&*x));
    xju::assert_not_equal(y,y);
  }
  catch(SnmpVar::NoSuchObject const& e) {
    xju::assert_equal(readableRepr(e),"no such object .1.3.4.");
  }
  try {
    auto y(x.value()->intValue());
    xju::assert_not_equal(y,y);
  }
  catch(SnmpVar::NoSuchObject const& e) {
    xju::assert_equal(readableRepr(e),"no such object .1.3.4.");
  }
}

void test3() {
  SnmpVar const x(
    Oid(".1.3.4"),
    SnmpVar::NoSuchInstance(Oid(".1.3.4"),XJU_TRACED));
  try {
    auto y(dynamic_cast<IntValue const*>(&*x));
    xju::assert_not_equal(y,y);
  }
  catch(SnmpVar::NoSuchInstance const& e) {
    xju::assert_equal(readableRepr(e),"no such instance .1.3.4.");
  }
  try {
    auto y(x.value()->intValue());
    xju::assert_not_equal(y,y);
  }
  catch(SnmpVar::NoSuchInstance const& e) {
    xju::assert_equal(readableRepr(e),"no such instance .1.3.4.");
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

