// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <iostream>
#include <xju/assert.hh>
#include <xju/http/parseStatusLine.hh>

namespace xju
{
namespace http
{

void test1() {
  {
    std::string const x{"HTTP/1.1 401 Not Found\r\n"};
    std::istringstream s{x};
    StatusLine y{parseStatusLine(s,x.size())};
    xju::assert_equal(y,StatusLine(HTTPVersion(HTTPVersion::Major(1),
                                             HTTPVersion::Minor(1)),
                                 StatusCode(401),
                                   ReasonPhrase("Not Found")));
  }
  try
  {
    std::string const x("HTTP/1.1 40 Not Found");
    std::istringstream s(x);
    StatusLine y(parseStatusLine(s,x.size()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse HTTP status-line of max 21 chars from stream because\nfailed to read stream until \"\\r\\n\" reading at most 21 chars because\n21 chars read without seeing \"\\r\\n\" amongst \"HTTP/1.1 40 Not Found\".");
  }
  try
  {
    std::string const x("HTTP/1.1 40 Not Found\r\n");
    std::istringstream s(x);
    StatusLine y(parseStatusLine(s,x.size()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse HTTP status-line of max 23 chars from stream because\nfailed to parse RFC7230 (HTTP) status-line at line 1 column 1 because\nfailed to parse HTTP status-code at line 1 column 10 because\nfailed to parse one of chars '0'..'9' at line 1 column 12 because\nline 1 column 12: ' ' is not one of chars '0'..'9'.");
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

