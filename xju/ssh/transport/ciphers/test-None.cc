// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/cipher/None.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/MemOBuf.hh>
#include <xju/net/ostream.hh>
#include <xju/ssh/transport/encodePacket.hh>
#include <xju/MemIBuf.hh>
#include <xju/net/istream.hh>

namespace xju
{
namespace ssh
{
namespace transport
{
namespace cipher
{

void test1() {
  None x;
  xju::MemOBuf b(256);
  {
    xju::net::ostream s(b);
    encodePacket({'f','r','e','d'},s,8,0.333);
    encodePacket({'j','o','n','e','s'},s,8,0.1236);
  }
  xju::MemIBuf bi(std::vector<uint8_t>(b.data().first,b.data().second));
  xju::net::istream s(bi);
  xju::assert_equal(x.decryptPacket(s).first,{'f','r','e','d'});
  xju::assert_equal(x.decryptPacket(s).first,{'j','o','n','e','s'});
}

void test2() {
  None x;
  xju::MemOBuf b(256);
  {
    xju::net::ostream s(b);
    x.encryptPacket({'f','r','e','d'},s);
    x.encryptPacket({'j','o','n','e','s'},s);
  }
  xju::MemIBuf bi(std::vector<uint8_t>(b.data().first,b.data().second));
  xju::net::istream s(bi);
  xju::assert_equal(x.decryptPacket(s).first,{'f','r','e','d'});
  xju::assert_equal(x.decryptPacket(s).first,{'j','o','n','e','s'});
}

}
}
}
}

using namespace xju::ssh::transport::cipher;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

