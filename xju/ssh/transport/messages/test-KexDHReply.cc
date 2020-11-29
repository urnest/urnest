// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/messages/KexDHReply.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/MemOBuf.hh>
#include <xju/MemIBuf.hh>
#include <xju/net/ostream.hh>
#include <xju/net/istream.hh>

namespace xju
{
namespace ssh
{
namespace transport
{
namespace messages
{

void test1() {
  xju::assert_equal(KexDHReply::type_,MSG::KEXDH_REPLY);
  
  KexDHReply const x({0x23,0x24},
                     xju::mpi::I(33),
                     {0x25,0x26,0x27});
  xju::assert_equal(x.hostKeyAndCertificates_,std::vector<uint8_t>{0x23,0x24});
  xju::assert_equal(x.f_,xju::mpi::I(33));
  xju::assert_equal(x.hostSignature_,std::vector<uint8_t>{0x25,0x26,0x27});
  
  xju::MemOBuf b(1024,1024);
  {
    xju::net::ostream s(b);
    encode(s,x);
  }
  xju::MemIBuf bi(b.data().first,b.data().second);
  {
    xju::net::istream s(bi);
    xju::assert_equal((MSG)s.get8(),MSG::KEXDH_REPLY);
    xju::assert_equal(decode<KexDHReply>(s),x);
  }
}

}
}
}
}

using namespace xju::ssh::transport::messages;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

