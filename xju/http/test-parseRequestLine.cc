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
#include <xju/http/parseRequestLine.hh>

namespace xju
{
namespace http
{

void test1() {
  {
    std::string const x{"GET /x.html HTTP/1.0\r\n"};
    std::istringstream s{x};
    RequestLine y{parseRequestLine(s,x.size())};
    xju::assert_equal(y.m_,Method("GET"));
    xju::assert_equal(y.t_,RequestTarget(
                        xju::uri::Path({
                            xju::uri::Segment(""),
                            xju::uri::Segment("x.html")}),
                        xju::uri::Query("")));
    xju::assert_equal(y.v_,HTTPVersion(HTTPVersion::Major(1),
                                       HTTPVersion::Minor(0)));
  }
  try
  {
    std::string const x("GET /x.html HTTP/1.0");
    std::istringstream s(x);
    RequestLine y(parseRequestLine(s,x.size()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse HTTP request-line of max 20 chars from stream because\nfailed to read stream until \"\\r\\n\" reading at most 20 chars because\n20 chars read without seeing \"\\r\\n\" amongst \"GET /x.html HTTP/1.0\".");
  }
  try
  {
    std::string const x("GET /x.html http/1.0\r\n");
    std::istringstream s(x);
    RequestLine y(parseRequestLine(s,x.size()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse HTTP request-line of max 22 chars from stream because\nfailed to parse RFC7230 (HTTP) request-line at line 1 column 1 because\nfailed to parse http-version at line 1 column 13 because\nfailed to parse \"HTTP/\" at line 1 column 13 because\nline 1 column 13: expected 'H' but found 'h'.");
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

