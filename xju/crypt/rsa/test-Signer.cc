// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/crypt/rsa/Signer.hh>
#include <iostream>
#include <xju/assert.hh>
#include <xju/crypt/verifySuccessful.hh>
#include <xju/crypt/hash/sha1.hh>
#include <xju/crypt/Signature.hh>
#include <xju/crypt/rsa/PrivateKey.hh>
#include <xju/crypt/rsa/generateKey.hh>
#include <xju/crypt/rsa/SigVerifier.hh>

namespace xju
{
namespace crypt
{
namespace rsa
{

void test1() {

  //create key
  PrivateKey privateKey(generateKey(xju::BitCount(3072)));
  
  auto const publicKey(privateKey.publicKey());
  
  std::string const message("the quick brown fox");

  Signer signer(privateKey);
  
  Signature signature(signer.sign(message.data(),message.size()));
  
  SigVerifier v(publicKey);
  v.verifySignature(message.data(),message.size(),signature);
}

}
}
}

using namespace xju::crypt::rsa;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  try{
    test1(), ++n;
  }
  catch(xju::Exception const& e){
    std::cerr << e << "\n";
    return 1;
  }
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

