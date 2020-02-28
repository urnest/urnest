// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/http/StatusLine.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/format.hh>

namespace xju
{
namespace http
{

void test1() {
  StatusLine x(HTTPVersion(HTTPVersion::Major(1),
                           HTTPVersion::Minor(0)),StatusCode(200),ReasonPhrase("OK"));
  xju::assert_equal(xju::format::str(x),std::string("HTTP/1.0 200 OK"));
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

