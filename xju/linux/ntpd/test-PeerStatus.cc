// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/linux/ntpd/PeerStatus.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace linux
{
namespace ntpd
{

void test1() {
  xju::assert_equal(isSysPeer(PeerStatus(0x8811)),false);
  xju::assert_equal(isSysPeer(PeerStatus(0x151a)),false);
  xju::assert_equal(isSysPeer(PeerStatus(0x1414)),false);
  xju::assert_equal(isSysPeer(PeerStatus(0x142a)),false);
  xju::assert_equal(isSysPeer(PeerStatus(0x1514)),false);
  xju::assert_equal(isSysPeer(PeerStatus(0x143a)),false);
  xju::assert_equal(isSysPeer(PeerStatus(0x161a)),true);
}

}
}
}

using namespace xju::linux::ntpd;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

