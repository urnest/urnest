// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/http/parseHeaders.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace http
{

void test1() {
  {
    std::string const s("\r\n");
    std::istringstream ss(s);
    auto const x(parseHeaders(ss,s.size()));
    xju::assert_equal(x,decltype(x)());
  }
  {
    std::string const s("Host-Name: big.com.au\r\n\r\n");
    std::istringstream ss(s);
    auto const x(parseHeaders(ss,s.size()));
    xju::assert_equal(x,Headers({
          Header(FieldName("Host-Name"),
                 FieldValue({FieldContent("big.com.au")}))}));
  }
  {
    std::string const s("Host-Name: big.com.au\r\nContent-Type: text/plain\r\n\r\n");
    std::istringstream ss(s);
    auto const x(parseHeaders(ss,s.size()));
    xju::assert_equal(x,Headers({
          Header(FieldName("Host-Name"),
                 FieldValue({FieldContent("big.com.au")})),
          Header(FieldName("Content-Type"),
                 FieldValue({FieldContent("text/plain")}))}));
  }
  {
    std::string const s("Host-Name: big.com.au\r\nBogus: fold\r\n ed\r\nContent-Type: text/plain\r\n\r\n");
    std::istringstream ss(s);
    auto const x(parseHeaders(ss,s.size()));
    xju::assert_equal(x,Headers({
          Header(FieldName("Host-Name"),
                 FieldValue({FieldContent("big.com.au")})),
          Header(FieldName("Bogus"),
                 FieldValue({FieldContent("fold"),FieldContent("ed")})),
          Header(FieldName("Content-Type"),
                 FieldValue({FieldContent("text/plain")}))}));
  }
  try
  {
    std::string const s("Host-Name: big.com.au\r\nContent-Type=text/plain\r\n\r\n");
    std::istringstream ss(s);
    auto const x(parseHeaders(ss,s.size()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse HTTP headers of max 50 chars from stream because\nfailed to parse \":\" at line 2 column 13 because\nline 2 column 13: expected ':' but found '='.");
  }
}

}
}

using namespace xju::http;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

