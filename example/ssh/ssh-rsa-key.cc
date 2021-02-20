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
#include <xju/path.hh>
#include <xju/file/read.hh>
#include <hcp/parser.hh>
#include <xju/ssh/openSSHPrivateKeyFileParser.hh>
#include <xju/MemIBuf.hh>
#include <xju/net/istream.hh>
#include <xju/ssh/decode.hh>
#include <sstream>
#include <string>
#include <xju/crypt/rsa/SigVerifier.hh>

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
