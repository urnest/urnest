// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/crypt/rsa/SigVerifier.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/crypt/verifySuccessful.hh>
#include <xju/crypt/hash/sha1.hh>
#include <xju/crypt/Signature.hh>

namespace xju
{
namespace crypt
{
namespace rsa
{

void test1() {

  //create key
  gcry_sexp_t key_(0);

  xju::crypt::sexp_ptr params("(genkey (rsa(nbits 4:3072)))");

  verifySuccessful(gcry_pk_genkey(&key_, params.get()),XJU_TRACED);
  sexp_ptr keyPair(key_);

  sexp_ptr privateKey(keyPair.getCons("private-key"));
  sexp_ptr publicKey_(keyPair.getCdrOf("public-key"));

  PublicKey publicKey(xju::BitCount(3072),
                      publicKey_.getI("e"),
                      publicKey_.getCdrOf("rsa").getI("n"));
  
  std::string const message("the quick brown fox");

  std::vector<uint8_t> messageHash(xju::crypt::hash::sha1(
                                     std::vector<uint8_t>(message.begin(),
                                                          message.end())));
  //sign message with private key
  xju::crypt::sexp_ptr data("(data (flags pkcs1) (hash sha1 %b))",
                            messageHash.size(),messageHash.data());
  gcry_sexp_t sig_(0);
  std::cout << "message hash:\n" << data << "\n";
  std::cout << "\nprivate key:\n" << privateKey << "\n";
  gcry_sexp_dump(privateKey.get());

  verifySuccessful(gcry_pk_testkey(privateKey.get()),XJU_TRACED);
  
  verifySuccessful(
    gcry_pk_sign(&sig_, data.get(), privateKey.get()),XJU_TRACED);
  xju::crypt::sexp_ptr sig(sig_);
  std::cout << sig << std::endl;//what is in sig so we can tell how to get it out

  std::cout << "signature:\n" << sig << "\n";

  Signature signature(sig.getCdrOf("sig-val").getDataOf("s"));
  
  SigVerifier v(publicKey);
  v.verifySignature(message.data(),message.size(),signature);

  Signature badSig(signature);
  while(badSig==signature){
    std::random_shuffle(badSig.begin(),badSig.end());
  }
  try{
    v.verifySignature(message.data(),message.size(),badSig);
  }
  catch(xju::Exception const& e){
    xju::assert_contains(readableRepr(e),std::string("Bad signature"));
  }
}

}
}
}

using namespace xju::crypt::rsa;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

