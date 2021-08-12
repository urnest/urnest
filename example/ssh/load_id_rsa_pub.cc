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

#include <xju/file/read.hh> //impl
#include <hcp/parser.hh> //impl
#include <xju/ssh/misc/openSSHPublicKeyFileParser.hh> //impl
#include <xju/format.hh> //impl
#include <xju/Exception.hh> //impl
#include <xju/MemIBuf.hh> //impl
#include <xju/net/istream.hh> //impl
#include <xju/ssh/decode.hh> //impl
#include <sstream> //impl

xju::crypt::rsa::PublicKey load_id_rsa_pub(xju::path::AbsFile const& path)
{
  try{
    auto const x(xju::file::read(path));
    auto const r(hcp_parser::parseString(
                   x.begin(),x.end(),
                   xju::ssh::misc::openSSHPublicKeyFileParser()));
    auto const i(
      hcp_ast::findOnlyChildOfType<xju::ssh::misc::open_ssh_public_key_file_parser::Item>(r));
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
