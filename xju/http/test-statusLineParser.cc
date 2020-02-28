// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/http/statusLineParser.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace http
{

void test1() {
  {
    std::string const s("HTTP/1.1 401 Not Found\r\n");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         statusLineParser()));
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<StatusLineItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<StatusLineItem>(r).get(),
                      StatusLine(HTTPVersion(HTTPVersion::Major(1),
                                             HTTPVersion::Minor(1)),
                                 StatusCode(401),
                                 ReasonPhrase("Not Found")));
  }
  try
  {
    std::string const s("HTTP/1.1 40 Not Found\r\n");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         statusLineParser()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse RFC7230 (HTTP) status-line at line 1 column 1 because\nfailed to parse HTTP status-code at line 1 column 10 because\nfailed to parse one of chars '0'..'9' at line 1 column 12 because\nline 1 column 12: ' ' is not one of chars '0'..'9'.");
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

