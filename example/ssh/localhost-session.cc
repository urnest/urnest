// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include "load_id_rsa_pub.hh"
#include <iostream>
#include <xju/path.hh>
#include <xju/ip/TCPSocket.hh>
#include <xju/ip/v4/Address.hh>
#include <xju/ip/Port.hh>
#include <xju/ssh/transport/Ident.hh>
#include <xju/ssh/transport/identParser.hh>
#include <xju/ssh/transport/kexers/DhGroup1SHA1Client.hh>
#include <xju/ssh/transport/Algorithms.hh>
#include <xju/ssh/transport/hkas/RSAClient.hh>
#include <xju/ssh/transport/ClientAlgorithms.hh>
#include <xju/ssh/transport/ciphers/AES256ctr.hh>
#include <functional>
#include <xju/ssh/transport/macs/HMACSHA1.hh>
#include <xju/format.hh>
#include <xju/steadyNow.hh>
#include <chrono>
#include <xju/ssh/transport/KexAlgorithmName.hh>
#include <xju/ssh/transport/Session.hh>
#include <xju/stringToUInt.hh>
#include <xju/crypt/hashers/SHA1.hh>
#include <xju/ssh/transport/kexers/DhGroup14Client.hh>
#include <xju/crypt/hashers/SHA256.hh>

int main(int argc, char* argv[]){
  if (argc < 2){
    std::cerr << "usage: " << argv[0] << " <localhost-rsa-public-key-file> [port]\n";
    return 1;
  }
  xju::ip::Port const port(argc>2?xju::stringToUInt(argv[2]):22U);
  
  try{
    xju::path::AbsFile hostRSAPublicKeyFile(xju::path::split(argv[1]));
    auto const hostPublicKey(load_id_rsa_pub(hostRSAPublicKeyFile));
    
    xju::ip::TCPSocket socket(
      {xju::ip::v4::Address("127.0.0.1"),port},
      xju::steadyNow()+std::chrono::seconds(10));
    
    xju::ssh::transport::Ident const ourIdent(
      xju::ssh::transport::SSHVersion("2.0"),
      xju::ssh::transport::SoftwareVersion("xju::ssh::1"),
      xju::Optional<std::string>());

    xju::crypt::hashers::SHA1 sha1;
    xju::crypt::hashers::SHA256 sha256;
    
    xju::ssh::transport::kexers::DHGroup1SHA1Client kexer1;
    xju::ssh::transport::kexers::DHGroup14Client dh_group14_sha1(sha1);
    xju::ssh::transport::kexers::DHGroup14Client dh_group14_sha256(sha256);
    xju::ssh::transport::Algorithms::Kexers kexers;
    kexers.push_back({xju::ssh::transport::KexAlgorithmName("diffie-hellman-group1-sha1"),std::ref(kexer1)});
    kexers.push_back({xju::ssh::transport::KexAlgorithmName("diffie-hellman-group14-sha1"),std::ref(dh_group14_sha1)});
    kexers.push_back({xju::ssh::transport::KexAlgorithmName("diffie-hellman-group14-sha256"),std::ref(dh_group14_sha256)});
    
    xju::ssh::transport::hkas::RSAClient hka1({hostPublicKey});
    
    xju::ssh::transport::ClientAlgorithms::ClientHostKeyAlgorithms hostKeyers;
    hostKeyers.push_back(
      {xju::ssh::transport::HostKeyAlgorithmName("ssh-rsa"),std::ref(hka1)});
    
    xju::ssh::transport::ciphers::AES256ctr cipher1; //rfc4344
    xju::ssh::transport::Algorithms::CipherAlgorithms ciphers;
    ciphers.push_back({xju::ssh::transport::CipherName("aes256-ctr"),cipher1});
    
    xju::ssh::transport::macs::HMACSHA1 mac1;
    xju::ssh::transport::Algorithms::PacketAuthenticationAlgorithms packetAuthenticators;
    packetAuthenticators.push_back(
      {xju::ssh::transport::PacketAuthenticatorName("hmac-sha1"),
       std::ref(mac1)});
    
    xju::ssh::transport::ClientAlgorithms algorithms(
      kexers,
      hostKeyers,
      ciphers,
      packetAuthenticators);
    
    xju::ssh::transport::Session session(
      socket,
      socket,
      1024,
      1024,
      ourIdent,
      algorithms,
      xju::steadyNow()+std::chrono::seconds(10),
      200000U);
      
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << xju::format::join(argv,argv+argc,std::string(" "));
    e.addContext(s.str(),XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 1;
  }
}
