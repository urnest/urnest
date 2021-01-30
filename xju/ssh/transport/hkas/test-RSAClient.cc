// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/hkas/RSAClient.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/crypt/rsa/PrivateKey.hh>
#include <xju/crypt/rsa/generateKey.hh>
#include <xju/BitCount.hh>
#include <xju/MemOBuf.hh>
#include <xju/net/ostream.hh>
#include <xju/ssh/encode.hh>
#include <xju/crypt/rsa/Signer.hh>
#include <xju/crypt/Signature.hh>
#include <xju/Exception.hh>
#include <hcp/parser.hh>

namespace xju
{
namespace ssh
{
namespace transport
{
namespace hkas
{

void test1() {
  xju::crypt::rsa::PrivateKey targetKey(
    xju::crypt::rsa::generateKey(xju::BitCount(2048)));

  //generate RSA key of same size as target key; make a key from one
  //by halving key size but keeping e and n (not a valid key
  //but does not matter, we want to check we do not pick it)
  std::set<xju::crypt::rsa::PublicKey> acceptableKeys;
  acceptableKeys.insert(
    xju::crypt::rsa::generateKey(xju::BitCount(2048)).publicKey());
  acceptableKeys.insert(xju::crypt::rsa::PublicKey(xju::BitCount(1024),
                                                   targetKey.e_,
                                                   targetKey.n_));
  acceptableKeys.insert(targetKey.publicKey());
  
  //encode target key per verifyBonafide
  xju::MemOBuf b(1024);
  {
    xju::net::ostream s(b);
    xju::ssh::encode(s,std::string("ssh-rsa"));
    xju::ssh::encode(s,targetKey.e_);
    xju::ssh::encode(s,targetKey.n_);
  }
  std::vector<uint8_t> const hostKeyAndCertificates(
    b.data().first,b.data().second);
  
  //sign a message with the target key (see crypt::rsa::test-SigVerifier)
  std::string message("the quick brown fox");
  xju::crypt::rsa::Signer signer(targetKey);
  xju::crypt::Signature const sig(signer.sign(message.data(),message.size()));
  
  //verifyBonafide
  RSAClient client(acceptableKeys);
  auto x(client.verifyBonafide(hostKeyAndCertificates,BitCount(sig.size()*8)));
  
  //verify above signature
  x->verifySignature(message.data(),message.size(),sig);

  std::reverse(message.begin(),message.end());
  try{
    x->verifySignature(message.data(),message.size(),sig);
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_startswith(readableRepr(e),std::string("Failed to verify that signature "));
    xju::assert_contains(readableRepr(e),std::string(" on 19 supplied data bytes was signed by owner of RSA public key "));
    xju::assert_endswith(readableRepr(e),std::string("gcrypt failed because Bad signature (error code 8)."));
  }

  {
    xju::crypt::rsa::PrivateKey unacceptableKey(
      xju::crypt::rsa::generateKey(xju::BitCount(2048)));
    xju::MemOBuf b(1024);
    {
      xju::net::ostream s(b);
      xju::ssh::encode(s,std::string("ssh-rsa"));
      xju::ssh::encode(s,unacceptableKey.e_);
      xju::ssh::encode(s,unacceptableKey.n_);
    }
    std::vector<uint8_t> const hostKeyAndCertificates(
      b.data().first,b.data().second);
    try{
      auto x(client.verifyBonafide(hostKeyAndCertificates,
                                   BitCount(sig.size()*8)));
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      auto const r(readableRepr(e));
      using namespace hcp_parser;
      hcp_parser::parseString(
        r.begin(),r.end(),
        "Failed to verify that SSH-encoded host key and certificates "+
        atLeastOne(hexDigit())+
        " represents one of connected host's known public keys "+
        listOf(("e: "+atLeastOne(hexDigit())+
                ", n ("+atLeastOne(digit())+"-bit): "+atLeastOne(hexDigit())),
               parseLiteral(", "),
               parseLiteral(" "))+
        "because\nfailed to decode and verify RSA public key having decoded key type identifier \"ssh-rsa\" as expected because\nfailed to progress having decoded parameter 'e' 0x"+atLeastOne(hexDigit())+
        " because\nfailed to progress having decoded parameter 'n' 0x"+
        atLeastOne(hexDigit())+
        " because\nRSA public key e: "+
        atLeastOne(hexDigit())+
        ", n (2048-bit): "+
        atLeastOne(hexDigit())+
        " is not a known key of connected host.");
    }
  }
}

}
}
}
}

using namespace xju::ssh::transport::hkas;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

