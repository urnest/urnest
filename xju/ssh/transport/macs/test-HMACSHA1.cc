// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/macs/HMACSHA1.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/MemOBuf.hh>
#include <xju/MemIBuf.hh>

namespace xju
{
namespace ssh
{
namespace transport
{
namespace macs
{

void test1() {
  HMACSHA1 n;
  xju::assert_equal(n.keySize_,BitCount(20*8));
  xju::crypt::MacKey const key({0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9});
  auto const c(n.macCalculator(key));
  xju::assert_equal(c->macSize(),20U);
  auto const mac(c->calculateMac({1,2}));

  auto const v(n.macVerifier(key));
  xju::assert_equal(c->macSize(),20U);
  v->verifyMac(mac,{1,2});
}

}
}
}
}

using namespace xju::ssh::transport::macs;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

