// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/SnmpV1Trap.hh>

#include <iostream>
#include <xju/assert.hh>
#include "xju/format.hh"
#include "xju/snmp/StringValue.hh"
#include "xju/snmp/IntValue.hh"

namespace xju
{
namespace snmp
{

void test1() {
  SnmpV1Trap const x(
    Community("pub"),
    Oid(".1.3.6.1.4.1.33.2.1"),
    xju::ip::v4::Address(0x01020304),
    SnmpV1Trap::GenericType(SnmpV1Trap::GenericType::LINKDOWN),
    SnmpV1Trap::SpecificType(0),
    std::chrono::milliseconds(20),
    std::map<Oid, std::shared_ptr<Value const> >{
      std::pair<Oid const,std::shared_ptr<Value const> >(
        Oid(".1.3.6.1.4.1.33.2.1.5"),
        std::shared_ptr<Value const>(new StringValue("fred"))),
      std::pair<Oid const,std::shared_ptr<Value const> >(
        Oid(".1.3.6.1.4.1.33.2.1.6"),
        std::shared_ptr<Value const>(new IntValue(89)))});
  xju::assert_equal(xju::format::str(x),"community \"pub\", trap type .1.3.6.1.4.1.33.2.1, origin 1.2.3.4, generic type 2, specific type 0, up time 2, vars: .1.3.6.1.4.1.33.2.1.5=\"fred\", .1.3.6.1.4.1.33.2.1.6=89");

  SnmpV1Trap const y(
    Community("pub"),
    Oid(".1.3.6.1.4.1.33.2.1"),
    xju::ip::v4::Address(0x01020304),
    SnmpV1Trap::GenericType(SnmpV1Trap::GenericType::LINKDOWN),
    SnmpV1Trap::SpecificType(0),
    std::chrono::milliseconds(20),
    std::map<Oid, std::shared_ptr<Value const> >{
      std::pair<Oid const,std::shared_ptr<Value const> >(
        Oid(".1.3.6.1.4.1.33.2.1.5"),
        std::shared_ptr<Value const>(new StringValue("fred"))),
      std::pair<Oid const,std::shared_ptr<Value const> >(
        Oid(".1.3.6.1.4.1.33.2.1.6"),
        std::shared_ptr<Value const>(new IntValue(90)))});

  xju::assert_less(x,y);
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

