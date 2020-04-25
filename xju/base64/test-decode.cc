// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/base64/decode.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/Exception.hh>

namespace xju
{
namespace base64
{

void test1() {
  {
    std::string const x("");
    xju::assert_equal(decode(x.begin(),x.end()),std::vector<uint8_t>({}));
  }
          
   // BASE64("f") = "Zg=="
  {
    std::string const x("Zg==");
    xju::assert_equal(decode(x.begin(),x.end()),std::vector<uint8_t>({'f'}));
  }

   // BASE64("fo") = "Zm8="
  {
    std::string const x("Zm8=");
    xju::assert_equal(decode(x.begin(),x.end()),std::vector<uint8_t>({'f','o'}));
  }

   // BASE64("foo") = "Zm9v"
  {
    std::string const x("Zm9v");
    xju::assert_equal(decode(x.begin(),x.end()),std::vector<uint8_t>({'f','o','o'}));
  }

   // BASE64("foob") = "Zm9vYg=="
  {
    std::string const x("Zm9vYg==");
    xju::assert_equal(decode(x.begin(),x.end()),std::vector<uint8_t>({'f','o','o','b'}));
  }

   // BASE64("fooba") = "Zm9vYmE="
  {
    std::string const x("Zm9vYmE=");
    xju::assert_equal(decode(x.begin(),x.end()),std::vector<uint8_t>({'f','o','o','b','a'}));
  }

   // BASE64("foobar") = "Zm9vYmFy"
  {
    std::string const x("Zm9vYmFy");
    xju::assert_equal(decode(x.begin(),x.end()),std::vector<uint8_t>({'f','o','o','b','a','r'}));
  }

  {
    std::string const x("Zm9vYmFy ");
    xju::assert_equal(decode(x.begin(),x.end()),std::vector<uint8_t>({'f','o','o','b','a','r'}));
  }
  {
    std::string const x(" Zm9v\n YmFy ");
    xju::assert_equal(decode(x.begin(),x.end()),std::vector<uint8_t>({'f','o','o','b','a','r'}));
  }

  try{
    std::string const x("Zm~vYg==");
    auto const y(decode(x.begin(),x.end()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to decode rfc4648-base64-encoded string because\nfailed to decode at offset 2 because\n'~' is not a valid base64 char A-Za-z0-9+/.");
  }
  try{
    std::string const x("Zm9vYmF");
    auto const y(decode(x.begin(),x.end()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to decode rfc4648-base64-encoded string because\nfailed to decode at offset 7 because\nfailed to read padding character #1 of 1 because\nend of base64 string.");
  }
  try{
    std::string const x("Zm9vYmE= ");
    auto const y(decode(x.begin(),x.end()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to decode rfc4648-base64-encoded string because\nfailed to decode at offset 8 because\n1 extra characters after base64 padding.");
  }
}

}
}

using namespace xju::base64;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}
