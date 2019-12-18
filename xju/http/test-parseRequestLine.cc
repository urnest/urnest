// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/http/parseRequestLine.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace http
{

void test1() {
  {
    std::string const x{"GET /x.html HTTP/1.1\r\n"};
    std::istringstream s{x};
    RequestLine y{parseRequestLine(s,x.size())};
    xju::assert_equal(y.m_,Method("GET"));
    xju::assert_equal(y.t_,RequestTarget(xju::path::AbsFile(xju::path::AbsolutePath({}),xju::path::FileName("x.html"))));
    xju::assert_equal(y.v_,HTTPVersion(HTTPVersion::Major(1),
                                       HTTPVersion::Minor(1)));
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

