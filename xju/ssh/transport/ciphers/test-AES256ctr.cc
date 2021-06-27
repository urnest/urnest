// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/ciphers/AES256ctr.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/MemOBuf.hh>
#include <gcrypt.h>
#include <xju/net/ostream.hh>
#include <xju/net/istream.hh>
#include <xju/MemIBuf.hh>

namespace xju
{
namespace ssh
{
namespace transport
{
namespace ciphers
{

void test1() {
  AES256ctr x;
  
  std::vector<uint8_t> key(x.keySize_.value()/8,0);
  gcry_randomize(key.data(),key.size(),GCRY_STRONG_RANDOM);

  std::vector<uint8_t> iv(x.ivSize_,0);
  gcry_randomize(iv.data(),iv.size(),GCRY_STRONG_RANDOM);

  auto encrypter(x.encrypter(CipherKey(key),CipherIV(iv)));
  auto decrypter(x.decrypter(CipherKey(key),CipherIV(iv)));

  {
    std::vector<uint8_t> m;
    xju::MemOBuf encrypted(1024);
    {
      xju::net::ostream s(encrypted);
      encrypter->encryptPacket(m,s);
    }
    {
      xju::MemIBuf b(std::vector<uint8_t>(encrypted.data().first,
                                        encrypted.data().second));
      xju::net::istream s(b);
      auto const y(decrypter->decryptPacket(s,
                                            encrypted.data().second-
                                            encrypted.data().first));
      xju::assert_equal(y.first,m);
    }
  }
  {
    std::vector<uint8_t> m{1,2,3,4,5};
    xju::MemOBuf encrypted(1024);
    {
      xju::net::ostream s(encrypted);
      encrypter->encryptPacket(m,s);
    }
    {
      xju::MemIBuf b(std::vector<uint8_t>(encrypted.data().first,
                                        encrypted.data().second));
      xju::net::istream s(b);
      auto const y(decrypter->decryptPacket(s,
                                            encrypted.data().second-
                                            encrypted.data().first));
      xju::assert_equal(y.first,m);
    }
  }
  {
    auto makeM=
      [](){
        std::vector<uint8_t> m;
        xju::MemOBuf b(1024);
        {
          xju::net::ostream s(b);
          for(auto i=0; i!=32323; ++i){
            s.put32(i);
          }
        }
        std::copy(b.data().first,b.data().second,
                  std::back_inserter(m));
        return m;
      };

    std::vector<uint8_t> const m(makeM());
    xju::assert_equal(m.size(),4*32323);
    xju::MemOBuf encrypted(1024);
    {
      xju::net::ostream s(encrypted);
      encrypter->encryptPacket(m,s);
    }
    {
      xju::MemIBuf b(std::vector<uint8_t>(encrypted.data().first,
                                          encrypted.data().second));
      xju::net::istream s(b);
      auto const y(decrypter->decryptPacket(s,
                                            encrypted.data().second-
                                            encrypted.data().first));
      xju::assert_equal(y.first,m);
    }
  }
}

}
}
}
}

using namespace xju::ssh::transport::ciphers;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

