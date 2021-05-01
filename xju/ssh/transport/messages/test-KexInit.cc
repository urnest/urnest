// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/messages/KexInit.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/MemOBuf.hh>
#include <xju/MemIBuf.hh>
#include <xju/ssh/transport/MSG.hh>
#include <xju/ssh/encode.hh>
#include <xju/ssh/decode.hh>

namespace xju
{
namespace ssh
{
namespace transport
{
namespace messages
{

void test1() {
  KexInit const x(
    xju::Array<uint8_t,16>{0,1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf},
    {KexAlgorithmName("kexa1")},
    {HostKeyAlgorithmName("hka1"),HostKeyAlgorithmName("hka2")},
    {CipherName("c1")},
    {CipherName("c2")},
    {PacketAuthenticatorName("a1")},
    {PacketAuthenticatorName("a2")},
    {CompressorName("z1")},
    {CompressorName("z2")},
    {LanguageName("l1")},
    {LanguageName("l2")},
    true);
  xju::assert_equal(x.cookie_,xju::Array<uint8_t,16>(
                      {0,1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf}));
  xju::assert_equal(x.kexAlgorithms_,{KexAlgorithmName("kexa1")});
  xju::assert_equal(x.serverHostKeyAlgorithmNames_,
                    {HostKeyAlgorithmName("hka1"),HostKeyAlgorithmName("hka2")});
  xju::assert_equal(x.clientOutCipherNames_,{CipherName("c1")});
  xju::assert_equal(x.serverOutCipherNames_,{CipherName("c2")});
  xju::assert_equal(x.clientOutPacketAuthenticatorNames_,{PacketAuthenticatorName("a1")});
  xju::assert_equal(x.serverOutPacketAuthenticatorNames_,{PacketAuthenticatorName("a2")});
  xju::assert_equal(x.clientOutCompressorNames_,{CompressorName("z1")});
  xju::assert_equal(x.serverOutCompressorNames_,{CompressorName("z2")});
  xju::assert_equal(x.clientOutLanguageNames_,{LanguageName("l1")});
  xju::assert_equal(x.serverOutLanguageNames_,{LanguageName("l2")});
  xju::assert_equal(x.firstKexPacketFollows_,true);

  xju::MemOBuf b(1024,1024);
  {
    xju::net::ostream s(b);
    encode(s,x);
  }
  xju::MemIBuf bi(b.data().first,b.data().second);
  {
    xju::net::istream s(bi);
    xju::assert_equal((MSG)s.get8(),MSG::KEXINIT);
    xju::assert_equal(decode<KexInit>(s),x);
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

