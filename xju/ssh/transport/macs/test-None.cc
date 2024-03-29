// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/macs/None.hh>

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
  None n;
  xju::assert_equal(n.keySize_,BitCount(0));
  auto const c(n.macCalculator(xju::crypt::MacKey({})));
  xju::assert_equal(c->macSize(),0U);
  auto const mac(c->calculateMac({}));
  xju::assert_equal(mac,xju::crypt::Mac({}));

  auto const v(n.macVerifier(xju::crypt::MacKey({})));
  xju::assert_equal(c->macSize(),0U);
  v->verifyMac(mac,{});
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

