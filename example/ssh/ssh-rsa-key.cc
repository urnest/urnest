// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <xju/crypt/rsa/PublicKey.hh>
#include <xju/path.hh>
#include <xju/file/read.hh>
#include <hcp/parser.hh>
#include <xju/ssh/openSSHPublicKeyFileParser.hh>
#include <sstream>
#include <xju/format.hh>
#include <xju/MemIBuf.hh>
#include <xju/net/istream.hh>
#include <xju/ssh/decode.hh>
#include <memory>
#include <xju/crypt/Signer.hh>
#include <xju/ssh/openSSHPrivateKeyFileParser.hh>
#include <xju/crypt/rsa/SigVerifier.hh>

xju::crypt::rsa::PublicKey load_id_rsa_pub(xju::path::AbsFile const& path)
{
  try{
    auto const x(xju::file::read(path));
    auto const r(hcp_parser::parseString(
                   x.begin(),x.end(),
                   xju::ssh::openSSHPublicKeyFileParser()));
    auto const i(
      hcp_ast::findOnlyChildOfType<xju::ssh::open_ssh_public_key_file_parser::Item>(r));
    if (std::get<0>(i.get())!=xju::ssh::KeyTypeName("ssh-rsa")){
      std::ostringstream s;
      s << "expected key type (name) \"ssh-rsa\" but got "
        << xju::format::quote(
          xju::format::cEscapeString(std::get<0>(i.get()).value()));
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    auto const encodedKey(std::get<1>(i.get()));
    xju::MemIBuf b(encodedKey.encodedValue_);
    xju::net::istream s(b);
    return xju::ssh::decode<xju::crypt::rsa::PublicKey>(s);
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "read OpenSSH Public Key file "
      << xju::path::str(path) << std::endl;
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}
std::unique_ptr<xju::crypt::Signer> load_id_rsa(xju::path::AbsFile const& path)
{
  try{
    auto const x(xju::file::read(path));
    auto const r(hcp_parser::parseString(
                   x.begin(),x.end(),
                   xju::ssh::openSSHPrivateKeyFileParser()));
    auto const i(
      hcp_ast::findOnlyChildOfType<xju::ssh::open_ssh_private_key_file_parser::Item>(r));
    xju::ssh::EncodedPrivateKey k(i.getEncodedPrivateKey());
    xju::MemIBuf b(k.value_);
    xju::net::istream s(b);
    return xju::ssh::decode<std::unique_ptr<xju::crypt::Signer>>(s);
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "read OpenSSH Private Key file "
      << xju::path::str(path) << std::endl;
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

int main(int argc, char* argv[])
{
  if (argc != 3){
    std::cerr << "usage: " << argv[0] << " <id_rsa> <id_rsa.pub>\n";
    return 1;
  }
  xju::path::AbsFile id_rsa_pub(xju::path::split(argv[2]));

  xju::crypt::rsa::SigVerifier verifier(load_id_rsa_pub(id_rsa_pub));

  xju::path::AbsFile id_rsa(xju::path::split(argv[1]));

  auto const signer(load_id_rsa(id_rsa));
  
  std::string message("fred the fox");

  auto signature(signer->sign(message.data(),message.size()));

  verifier.verifySignature(message.data(),message.size(),signature);

  return 0;
}
