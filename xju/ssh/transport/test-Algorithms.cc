// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/Algorithms.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace ssh
{
namespace transport
{

namespace
{
class K : public Kexer
{
public:
  explicit K(bool const requiresEncryptionCapableHostKey,
             bool const requiresSignatureCapableHostKey):
      x_(requiresEncryptionCapableHostKey),
      y_(requiresSignatureCapableHostKey)
  {
  }
  virtual SessionKey exchangeKeys(
    I::RekeyLock& i,
    O::RekeyLock& o,
    Ident const& ourIdent,
    std::pair<std::string,Ident> const& peerPreambleAndIdent) override
  {
    xju::assert_never_reached();
  }

  virtual bool requiresEncryptionCapableHostKey() const noexcept override
  {
    return x_;
  }
  virtual bool requiresSignatureCapableHostKey() const noexcept override
  {
    return y_;
  }

private:
  bool x_;
  bool y_;
};
class HK : public HostKeyAlgorithm
{
public:
  HK(bool isEncryptionCapable, bool isSignatureCapable) noexcept:
      x_(isEncryptionCapable),
      y_(isSignatureCapable)
  {
  }
  virtual bool isEncryptionCapable() const noexcept override
  {
    return x_;
  }
  virtual bool isSignatureCapable() const noexcept override
  {
    return y_;
  }
private:
  bool x_;
  bool y_;
};

}

void test1() {
  KexAlgorithmName K1("K1");
  K k1(true,true);
  xju::Array<uint8_t,16> cookie;
  // client and server guess correctly
  {
    auto const x(Algorithms(
                   { {K1,std::ref(k1)} },
                   {},
                   {},
                   {}).chooseKexer(
                   messages::KexInit(
                     cookie,
                     {K1},
                     {},{},{},{},{},{},{},{},{},
                     false),
                   messages::KexInit(
                     cookie,
                     {K1},
                     {},{},{},{},{},{},{},{},{},
                     true)));
    xju::assert_equal(&x.first.get(),&k1);
    xju::assert_equal(x.second,false);
  }
  KexAlgorithmName K2("K2");
  K k2(false,false);
  // mis-guess but no encryption or signature host key algorithms needed
  {
    auto const x(Algorithms(
                   { {K1,std::ref(k1)},
                     {K2,std::ref(k2)}},
                   {},
                   {},
                   {}).chooseKexer(
                   messages::KexInit(
                     cookie,
                     {K1,K2},
                     {},{},{},{},{},{},{},{},{},
                     false),
                   messages::KexInit(
                     cookie,
                     {K2},
                     {},{},{},{},{},{},{},{},{},
                     true)));
    xju::assert_equal(&x.first.get(),&k2);
    xju::assert_equal(x.second,true);
  }
  {
    auto const x(Algorithms(
                   { {K1,std::ref(k1)},
                     {K2,std::ref(k2)}},
                   {},
                   {},
                   {}).chooseKexer(
                   messages::KexInit(
                     cookie,
                     {K1,K2},
                     {},{},{},{},{},{},{},{},{},
                     false),
                   messages::KexInit(
                     cookie,
                     {K2},
                     {},{},{},{},{},{},{},{},{},
                     false)));
    xju::assert_equal(&x.first.get(),&k2);
    xju::assert_equal(x.second,false);
  }
  {
    auto const x(Algorithms(
                   { {K2,std::ref(k2)}},
                   {},
                   {},
                   {}).chooseKexer(
                   messages::KexInit(
                     cookie,
                     {K2},
                     {},{},{},{},{},{},{},{},{},
                     false),
                   messages::KexInit(
                     cookie,
                     {K1,K2},
                     {},{},{},{},{},{},{},{},{},
                     true)));
    xju::assert_equal(&x.first.get(),&k2);
    xju::assert_equal(x.second,true);
  }
  @@@;
  // encrption capable host key needed
  // signature capable host key needed

  // no common ground
}

}
}
}

using namespace xju::ssh::transport;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

