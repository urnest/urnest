// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/http/requestLineParser.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace http
{

void test1() {
  {
    std::string const s("GET /x.html HTTP/1.0\r\n");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         requestLineParser()));
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestLineItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<RequestLineItem>(r).get(),
                      RequestLine(
                        Method("GET"),
                        RequestTarget(
                          xju::uri::Path({
                              xju::uri::Segment(""),
                              xju::uri::Segment("x.html")}),
                          xju::uri::Query("")),
                        HTTPVersion(HTTPVersion::Major(1),
                                    HTTPVersion::Minor(0))));
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

