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
namespace mac
{

void test1() {
  None n;
  xju::MemOBuf b(256);
  {
    xju::net::ostream s(b);
    n.computeMac(SequenceNumber(12),
                 {},
                 Padding{},
                 s);
  }
  xju::assert_equal(b.data().first,b.data().second);
}

void test2() {
  None n;
  xju::MemIBuf b({});
  {
    xju::net::istream s(b);
    n.authenticatePacket(SequenceNumber(12),
                         {},
                         Padding{},
                         s);
  }
}

}
}
}
}

using namespace xju::ssh::transport::mac;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

