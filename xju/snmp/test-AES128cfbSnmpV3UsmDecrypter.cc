// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/AES128cfbSnmpV3UsmDecrypter.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/snmp/SnmpV3UsmPrivKey.hh>
#include <xju/snmp/AES128cfbSnmpV3UsmDecrypter.hh>
#include <xju/snmp/AES128cfbSnmpV3UsmEncrypter.hh>
#include <xju/UserName.hh>
#include <xju/snmp/EngineBoots.hh>
#include <xju/snmp/EngineTime.hh>
#include <xju/snmp/SnmpV3ScopedPduData.hh>

namespace xju
{
namespace snmp
{

void test1() {
  SnmpV3UsmPrivKey const privKey({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15});
  AES128cfbSnmpV3UsmEncrypter e(privKey);
  AES128cfbSnmpV3UsmDecrypter d(privKey);
  UserName const username("fred");
  EngineBoots const engineBoots(34);
  EngineTime const engineTime(18);

  auto const x(e.encryptScopedPduData(
                 username,
                 engineBoots,
                 engineTime,
                 SnmpV3ScopedPduData({9,8,7,6,2})));
  xju::assert_equal(
    d.decryptScopedPduData(
      username,
      engineBoots,
      engineTime,
      x.first,
      x.second),
    SnmpV3ScopedPduData({9,8,7,6,2}));
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

