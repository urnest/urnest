// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/tls/PrivateKey.hh>

#include <iostream>
#include <xju/assert.hh>
#include <hcp/parser.hh>
#include <xju/tls/pemParser.hh>

namespace xju
{
namespace tls
{

void test1() {

  {
    std::string const pk=R"--(-----BEGIN EC PRIVATE KEY-----
MIGkAgEBBDBG2oN5DamWWRHyC/HfK4NF3E292ahylA9Pdq7TePL26OZjlFc+4T8N
Yn1yT8vmP06gBwYFK4EEACKhZANiAAS0u6wKpPXur0ZQ6IkzhRX8W7HNYayeodYn
pOSlbLXBNJ1see195nAAoktlWKUZ8l5wHgKmWXhk52AuAyCX7WD0MCsbrtJ8BtI8
jYopyHnOnq0OSdDrMujZRB19rapg+dA=
-----END EC PRIVATE KEY-----)--";
    try{
      auto const x(hcp_parser::parseString(pk.begin(),pk.end(),pemParser()));
      auto const y(hcp_ast::findOnlyChildOfType<PEMItem>(x));
      PrivateKey pk(y.getPayload());
    }
    catch(xju::Exception& e){
      std::cerr << readableRepr(e) << std::endl;
      xju::assert_never_reached();
    }
  }
  {
    std::string const pk=R"--(-----BEGIN EC PRIVATE KEY-----
MIGkAgEBBDBG2oN5DamWWRHyC/HfK4NF3E292ahylA9Pdq7TePL26OZjlFc+4T8N
Yn1yT8vmP06gBwYFK4EEACKhZANiAAS0u6wKpPXur0ZQ6IkzhRX8W7HNYayeodYn
pOSlbLXBNJ1see195nAAoktlWKUZ8l5wHgKmWXhk52AuAyCX7WD0MCsbrtJ8BtI8
-----END EC PRIVATE KEY-----)--";
    try{
      auto const x(hcp_parser::parseString(pk.begin(),pk.end(),pemParser()));
      auto const y(hcp_ast::findOnlyChildOfType<PEMItem>(x));
      try{
        PrivateKey pk(y.getPayload());
        xju::assert_never_reached();
      }
      catch(xju::Exception& e){
        
        xju::assert_startswith(readableRepr(e),std::string("Failed to decode private key from DER 0x30 0x81 0xa4 0x02 0x01 0x01 0x04 0x30 0x46 0xda 0x83 0x79 0x0d 0xa9 0x96 0x59 0x11 0xf2 0x0b 0xf1 0xdf 0x2b 0x83 0x45 0xdc 0x4d 0xbd 0xd9 0xa8 0x72 0x94 0x0f 0x4f 0x76 0xae 0xd3 0x78 0xf2 0xf6 0xe8 0xe6 0x63 0x94 0x57 0x3e 0xe1 0x3f 0x0d 0x62 0x7d 0x72 0x4f 0xcb 0xe6 0x3f 0x4e 0xa0 0x07 0x06 0x05 0x2b 0x81 0x04 0x00 0x22 0xa1 0x64 0x03 0x62 0x00 0x04 0xb4 0xbb 0xac 0x0a 0xa4 0xf5 0xee 0xaf 0x46 0x50 0xe8 0x89 0x33 0x85 0x15 0xfc 0x5b 0xb1 0xcd 0x61 0xac 0x9e 0xa1 0xd6 0x27 0xa4 0xe4 0xa5 0x6c 0xb5 0xc1 0x34 0x9d 0x6c 0x79 0xed 0x7d 0xe6 0x70 0x00 0xa2 0x4b 0x65 0x58 0xa5 0x19 0xf2 0x5e 0x70 0x1e 0x02 0xa6 0x59 0x78 0x64 0xe7 0x60 0x2e 0x03 0x20 0x97 0xed 0x60 0xf4 0x30 0x2b 0x1b 0xae 0xd2 0x7c 0x06 0xd2 0x3c because\n"));
      }
    }
    catch(xju::Exception& e){
      std::cerr << readableRepr(e) << std::endl;
      xju::assert_never_reached();
    }
  }
}

}
}

using namespace xju::tls;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

