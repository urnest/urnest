// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/chooseKexer.hh>

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
class K_ : public Kexer
{
public:
  explicit K_(bool const requiresEncryptionCapableHostKey,
             bool const requiresSignatureCapableHostKey):
      x_(requiresEncryptionCapableHostKey),
      y_(requiresSignatureCapableHostKey)
  {
  }
  virtual std::pair<K,H> exchangeKeys(
    I::RekeyLock& i,
    O::RekeyLock& o,
    Ident const& ourIdent,
    std::pair<std::string,Ident> const& peerPreambleAndIdent,
    messages::KexInit const& ourKexInit,
    messages::KexInit const& peerKexInit,
    HostKeyAlgorithm& hostKeyAlgorithm,
    bool ignoreFirstMessageReceived) override
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

// chooseKexer
void test1() {
  KexAlgorithmName K1("K1");
  K_ k1(true,true);
  // client and server guess correctly
  {
    auto const x(chooseKexer(
                    {K1},
                    {K1},
                    {},{},
                    { {K1,std::ref(k1)} },
                    {}));
    xju::assert_equal(&x.first.get(),&k1);
    xju::assert_equal(x.second,true);
  }
  KexAlgorithmName K2("K2");
  K_ k2(false,false);
  // mis-guess but no encryption or signature host key algorithms needed
  {
    auto const x(chooseKexer(
                   {K1,K2},
                   {K2},
                   {},{},
                   { {K1,std::ref(k1)},
                     {K2,std::ref(k2)} },
                   {}));
    xju::assert_equal(&x.first.get(),&k2);
    xju::assert_equal(x.second,false);
  }
  {
    auto const x(chooseKexer(
                   {K1,K2},
                   {K2},
                   {},{},
                   { {K1,std::ref(k1)},
                     {K2,std::ref(k2)}},
                   {}));
    xju::assert_equal(&x.first.get(),&k2);
    xju::assert_equal(x.second,false);
  }
  {
    auto const x(chooseKexer(
                   {K2},
                   {K1,K2},
                   {},{},
                   { {K2,std::ref(k2)}},
                   {}));
    xju::assert_equal(&x.first.get(),&k2);
    xju::assert_equal(x.second,false);
  }

  // encryption capable host key needed but server does not have one
  {
    HostKeyAlgorithmName HK1("HK1");
    HK hk1(true,true);
    HostKeyAlgorithmName HK2("HK2");
    HK hk2(false,true);
    {
      auto const x(chooseKexer(
                     {K1,K2},
                     {K2,K1},
                     {},
                     {HK2},
                     { {K2,std::ref(k2)},
                       {K1,std::ref(k1)} },
                     { {HK1,std::ref(hk1)},
                       {HK2,std::ref(hk2)} }));
      xju::assert_equal(&x.first.get(),&k2);
      xju::assert_equal(x.second,false);
    }
  }
  // signature capable host key needed
  {
    HostKeyAlgorithmName HK1("HK1");
    HK hk1(true,true);
    HostKeyAlgorithmName HK2("HK2");
    HK hk2(true,false);
    {
      auto const x(chooseKexer(
                     {K1,K2},
                     {K2,K1},
                     {},
                     {HK2},
                     { {K2,std::ref(k2)},
                       {K1,std::ref(k1)} },
                     { {HK1,std::ref(hk1)},
                       {HK2,std::ref(hk2)} }));
      xju::assert_equal(&x.first.get(),&k2);
      xju::assert_equal(x.second,false);
    }
  }

  // no common ground
  try
  {
    HostKeyAlgorithmName HK1("HK1");
    HK hk1(true,true);
    HostKeyAlgorithmName HK2("HK2");
    HK hk2(true,false);
    {
      auto const x(chooseKexer(
                     {K1},
                     {K2,K1},
                     {},
                     {HK2},
                     { {K2,std::ref(k2)},
                       {K1,std::ref(k1)} },
                     { {HK1,std::ref(hk1)},
                       {HK2,std::ref(hk2)} }));
      xju::assert_never_reached();
    }
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to choose key exchange method given client proposes key exchange algorithms K1 and host key algorithms  while server proposes key exchange algorithms K2; K1 and host key algorithms HK2 because\nclient and server have no common key-exchange and/or required host key algorithm.");
  }
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

