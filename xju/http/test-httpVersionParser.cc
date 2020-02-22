// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/http/httpVersionParser.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace http
{

void test1() {
  {
    const std::string s("HTTP/1.0");
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         httpVersionParser())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<HTTPVersionItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<HTTPVersionItem>(r).get(),
                      HTTPVersion(HTTPVersion::Major(1),
                                  HTTPVersion::Minor(0)));
  }
  try
  {
    const std::string s("HTTP/a");
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         httpVersionParser())};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse http-version at line 1 column 1 because\nfailed to parse major at line 1 column 6 because\nfailed to parse one of chars '0'..'9' at line 1 column 6 because\nline 1 column 6: 'a' is not one of chars '0'..'9'.");
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

