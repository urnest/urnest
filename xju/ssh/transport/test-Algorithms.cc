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
    std::pair<std::string,Ident> const& peerPreambleAndIdent,
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

  // encryption capable host key needed but server does not have one
  {
    HostKeyAlgorithmName HK1("HK1");
    HK hk1(true,true);
    HostKeyAlgorithmName HK2("HK2");
    HK hk2(false,true);
    {
      auto const x(Algorithms(
                     { {K2,std::ref(k2)},
                       {K1,std::ref(k1)} },
                     { {HK1,std::ref(hk1)},
                       {HK2,std::ref(hk2)} },
                     {},
                     {}).chooseKexer(
                       messages::KexInit(
                         cookie,
                         {K1,K2},
                         {},{},{},{},{},{},{},{},{},
                         false),
                       messages::KexInit(
                         cookie,
                         {K2,K1},
                         {HK2},{},{},{},{},{},{},{},{},
                         true)));
      xju::assert_equal(&x.first.get(),&k2);
      xju::assert_equal(x.second,true);
    }
  }
  // signature capable host key needed
  {
    HostKeyAlgorithmName HK1("HK1");
    HK hk1(true,true);
    HostKeyAlgorithmName HK2("HK2");
    HK hk2(true,false);
    {
      auto const x(Algorithms(
                     { {K2,std::ref(k2)},
                       {K1,std::ref(k1)} },
                     { {HK1,std::ref(hk1)},
                       {HK2,std::ref(hk2)} },
                     {},
                     {}).chooseKexer(
                       messages::KexInit(
                         cookie,
                         {K1,K2},
                         {},{},{},{},{},{},{},{},{},
                         false),
                       messages::KexInit(
                         cookie,
                         {K2,K1},
                         {HK2},{},{},{},{},{},{},{},{},
                         true)));
      xju::assert_equal(&x.first.get(),&k2);
      xju::assert_equal(x.second,true);
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
      auto const x(Algorithms(
                     { {K2,std::ref(k2)},
                       {K1,std::ref(k1)} },
                     { {HK1,std::ref(hk1)},
                       {HK2,std::ref(hk2)} },
                     {},
                     {}).chooseKexer(
                       messages::KexInit(
                         cookie,
                         {K1},
                         {},{},{},{},{},{},{},{},{},
                         false),
                       messages::KexInit(
                         cookie,
                         {K2,K1},
                         {HK2},{},{},{},{},{},{},{},{},
                         true)));
      xju::assert_never_reached();
    }
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to choose key exchange method given our KexInit message kex algorithms K1; host key algorithms ; client-to-server ciphers ; server-to-client ciphers ; client-to-server mac algorithms ; server-to-client mac algorithms ; client-to-server compressors ; server-to-client compressors ; client-to-server languages ; server-to-client languages  and peer KexInit message kex algorithms K2, K1; host key algorithms HK2; client-to-server ciphers ; server-to-client ciphers ; client-to-server mac algorithms ; server-to-client mac algorithms ; client-to-server compressors ; server-to-client compressors ; client-to-server languages ; server-to-client languages  because\nclient and server have no common key-exchange and/or required host key algorithm.");
  }
}

// chooseHostKeyAlgorithm
void test2(){
  HostKeyAlgorithmName HK1("HK1");
  HK hk1(true,true);
  HostKeyAlgorithmName HK2("HK2");
  HK hk2(false,false);
  HostKeyAlgorithmName HK3("HK3");
  HK hk3(true,true);

  xju::Array<uint8_t,16> cookie;
  {
    auto const& x(Algorithms(
                   { },
                   { {HK1,std::ref(hk1)},
                     {HK2,std::ref(hk2)},
                     {HK3,std::ref(hk3)} },
                   {},
                   {}).chooseHostKeyAlgorithm(
                     messages::KexInit(
                       cookie,
                       {},
                       {HK1,HK2,HK3},{},{},{},{},{},{},{},{},
                       false),
                     messages::KexInit(
                       cookie,
                       {},
                       {HK2,HK3},{},{},{},{},{},{},{},{},
                       true),
                     true,
                     true));
    xju::assert_equal(&x,&hk3);
  }
  {
    auto const& x(Algorithms(
                   { },
                   { {HK1,std::ref(hk1)},
                     {HK2,std::ref(hk2)},
                     {HK3,std::ref(hk3)} },
                   {},
                   {}).chooseHostKeyAlgorithm(
                     messages::KexInit(
                       cookie,
                       {},
                       {HK1,HK2,HK3},{},{},{},{},{},{},{},{},
                       false),
                     messages::KexInit(
                       cookie,
                       {},
                       {HK2,HK3},{},{},{},{},{},{},{},{},
                       true),
                     false,
                     true));
    xju::assert_equal(&x,&hk3);
  }
  {
    auto const& x(Algorithms(
                   { },
                   { {HK1,std::ref(hk1)},
                     {HK2,std::ref(hk2)},
                     {HK3,std::ref(hk3)} },
                   {},
                   {}).chooseHostKeyAlgorithm(
                     messages::KexInit(
                       cookie,
                       {},
                       {HK1,HK2,HK3},{},{},{},{},{},{},{},{},
                       false),
                     messages::KexInit(
                       cookie,
                       {},
                       {HK2,HK3},{},{},{},{},{},{},{},{},
                       true),
                     true,
                     false));
    xju::assert_equal(&x,&hk3);
  }
  {
    auto const& x(Algorithms(
                   { },
                   { {HK1,std::ref(hk1)},
                     {HK2,std::ref(hk2)},
                     {HK3,std::ref(hk3)} },
                   {},
                   {}).chooseHostKeyAlgorithm(
                     messages::KexInit(
                       cookie,
                       {},
                       {HK1,HK2,HK3},{},{},{},{},{},{},{},{},
                       false),
                     messages::KexInit(
                       cookie,
                       {},
                       {HK2,HK3},{},{},{},{},{},{},{},{},
                       true),
                     false,
                     false));
    xju::assert_equal(&x,&hk2);
  }
  try
  {
    auto const& x(Algorithms(
                   { },
                   { {HK1,std::ref(hk1)},
                     {HK2,std::ref(hk2)},
                     {HK3,std::ref(hk3)} },
                   {},
                   {}).chooseHostKeyAlgorithm(
                     messages::KexInit(
                       cookie,
                       {},
                       {HK1,HK2,HK3},{},{},{},{},{},{},{},{},
                       false),
                     messages::KexInit(
                       cookie,
                       {},
                       {HK2},{},{},{},{},{},{},{},{},
                       true),
                     true,
                     false));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to choose host key algorithm given our KexInit message kex algorithms ; host key algorithms HK1, HK2, HK3; client-to-server ciphers ; server-to-client ciphers ; client-to-server mac algorithms ; server-to-client mac algorithms ; client-to-server compressors ; server-to-client compressors ; client-to-server languages ; server-to-client languages , peer KexInit message kex algorithms ; host key algorithms HK2; client-to-server ciphers ; server-to-client ciphers ; client-to-server mac algorithms ; server-to-client mac algorithms ; client-to-server compressors ; server-to-client compressors ; client-to-server languages ; server-to-client languages  noting encryption capable algorithm is required and signature capable algorithm is not required because\nclient and server have no common host key algorithm that meets the encryption and signature requirements.");
  }
}

class C : public CipherAlgorithm
{
  std::unique_ptr<Encrypter> encrypter(SessionKey const& k) override{
    xju::assert_never_reached();
  }
  std::unique_ptr<Decrypter> decrypter(SessionKey const& k) override{
    xju::assert_never_reached();
  }
};

void test3(){
  CipherName C1("C1");
  CipherName C2("C2");
  C c1;
  C c2;
  CipherName C3("C3");
  CipherName C4("C4");

  xju::Array<uint8_t,16> cookie;
  {
    auto const& x(Algorithms(
                   { },
                   { },
                   { {C1,std::ref(c1)},
                     {C2,std::ref(c2)} },
                   {}).chooseEncryptionCipher(
                     messages::KexInit(
                       cookie,
                       {},
                       {},
                       {C1,C2},{C3,C4},{},{},{},{},{},{},
                       false),
                     messages::KexInit(
                       cookie,
                       {},
                       {},
                       {C2,C1},{C3,C4},{},{},{},{},{},{},
                       true)));
    xju::assert_equal(&x,&c1);
  }
  {
    auto const& x(Algorithms(
                   { },
                   { },
                   { {C1,std::ref(c1)},
                     {C2,std::ref(c2)} },
                   {}).chooseEncryptionCipher(
                     messages::KexInit(
                       cookie,
                       {},
                       {},
                       {C1,C2},{C3,C4},{},{},{},{},{},{},
                       false),
                     messages::KexInit(
                       cookie,
                       {},
                       {},
                       {C2,C3},{C3,C4},{},{},{},{},{},{},
                       true)));
    xju::assert_equal(&x,&c2);
  }
  try
  {
    auto const& x(Algorithms(
                   { },
                   { },
                   { {C1,std::ref(c1)},
                     {C2,std::ref(c2)} },
                   {}).chooseEncryptionCipher(
                     messages::KexInit(
                       cookie,
                       {},
                       {},
                       {C1,C2},{C3,C4},{},{},{},{},{},{},
                       false),
                     messages::KexInit(
                       cookie,
                       {},
                       {},
                       {C4,C3},{C3,C4},{},{},{},{},{},{},
                       true)));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to choose our encryption cipher given our KexInit message kex algorithms ; host key algorithms ; client-to-server ciphers C1, C2; server-to-client ciphers C3, C4; client-to-server mac algorithms ; server-to-client mac algorithms ; client-to-server compressors ; server-to-client compressors ; client-to-server languages ; server-to-client languages , peer KexInit message kex algorithms ; host key algorithms ; client-to-server ciphers C4, C3; server-to-client ciphers C3, C4; client-to-server mac algorithms ; server-to-client mac algorithms ; client-to-server compressors ; server-to-client compressors ; client-to-server languages ; server-to-client languages  because\nwe have no us-to-peer cipher in common.");
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
  test2(), ++n;
  test3(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

