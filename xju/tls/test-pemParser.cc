// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/tls/pemParser.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/base64/decode.hh>
#include <xju/Exception.hh>

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
      xju::assert_equal(y.getType(),PEMType("EC PRIVATE KEY"));
      xju::assert_equal(y.getComments(),"");
      xju::assert_equal(y.getPayload(),xju::base64::decode(
                          "MIGkAgEBBDBG2oN5DamWWRHyC/HfK4NF3E292ahylA9Pdq7TePL26OZjlFc+4T8NYn1yT8vmP06gBwYFK4EEACKhZANiAAS0u6wKpPXur0ZQ6IkzhRX8W7HNYayeodYnpOSlbLXBNJ1see195nAAoktlWKUZ8l5wHgKmWXhk52AuAyCX7WD0MCsbrtJ8BtI8jYopyHnOnq0OSdDrMujZRB19rapg+dA="));
    }
    catch(xju::Exception const& e){
      std::cerr << readableRepr(e) << std::endl;
      xju::assert_never_reached();
    }
  }
  {
    std::string const pk=R"--(my private key
for testing
openssl ecparam -genkey -name secp384r1 | openssl ec -out ec.key
-----BEGIN EC PRIVATE KEY-----
MIGkAgEBBDBG2oN5DamWWRHyC/HfK4NF3E292ahylA9Pdq7TePL26OZjlFc+4T8N
Yn1yT8vmP06gBwYFK4EEACKhZANiAAS0u6wKpPXur0ZQ6IkzhRX8W7HNYayeodYn
pOSlbLXBNJ1see195nAAoktlWKUZ8l5wHgKmWXhk52AuAyCX7WD0MCsbrtJ8BtI8
jYopyHnOnq0OSdDrMujZRB19rapg+dA=
-----END EC PRIVATE KEY-----)--";
    try{
      auto const x(hcp_parser::parseString(pk.begin(),pk.end(),pemParser()));
      auto const y(hcp_ast::findOnlyChildOfType<PEMItem>(x));
      xju::assert_equal(y.getType(),PEMType("EC PRIVATE KEY"));
      xju::assert_equal(y.getComments(),"my private key\nfor testing\nopenssl ecparam -genkey -name secp384r1 | openssl ec -out ec.key\n");
      xju::assert_equal(y.getPayload(),xju::base64::decode(
                          "MIGkAgEBBDBG2oN5DamWWRHyC/HfK4NF3E292ahylA9Pdq7TePL26OZjlFc+4T8NYn1yT8vmP06gBwYFK4EEACKhZANiAAS0u6wKpPXur0ZQ6IkzhRX8W7HNYayeodYnpOSlbLXBNJ1see195nAAoktlWKUZ8l5wHgKmWXhk52AuAyCX7WD0MCsbrtJ8BtI8jYopyHnOnq0OSdDrMujZRB19rapg+dA="));
    }
    catch(xju::Exception const& e){
      std::cerr << readableRepr(e) << std::endl;
      xju::assert_never_reached();
    }
  }
  {
    std::string const pk=R"--(-----BEGIN EC PRIVATE KEY-----
MIGkAgEBBDBG2oN5DamWWRHyC/HfK4NF3E292ahylA9Pdq7TePL26OZjlFc+4T8N
Yn1yT8vm\tP06gBwYFK4EEACKhZANiAAS0u6wKpPXur0ZQ6IkzhRX8W7HNYayeodYn
pOSlbLXBNJ1see195nAAoktlWKUZ8l5wHgKmWXhk52AuAyCX7WD0MCsbrtJ8BtI8
jYopyHnOnq0OSdDrMujZRB19rapg+dA=
-----END EC PRIVATE KEY-----)--";
    try{
      auto const x(hcp_parser::parseString(pk.begin(),pk.end(),pemParser()));
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      xju::assert_equal(readableRepr(e),"Failed to parse PEM (RFC7468) Item at line 1 column 1 because\nfailed to parse base64 payload at line 2 column 1 because\nfailed to parse one of chars \"a-zA-Z0-9+/\" at line 3 column 9 because\nline 3 column 9: '\\' is not one of chars \"a-zA-Z0-9+/\".");
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

