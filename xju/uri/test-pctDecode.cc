// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/uri/pctDecode.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/uri/pctEncode.hh>

namespace xju
{
namespace uri
{

void test1() {

  hcp::Chars const httpQueryAllowed("a-zA-Z0-9");
  hcp::Chars const uriQueryAllowed(httpQueryAllowed+hcp::Chars("=&"));
  xju::assert_equal(pctDecode("afc!",uriQueryAllowed),"afc!");
  {
    auto httpQuery("name="+pctEncode(httpQueryAllowed,"Fred & Jock?")+"&"+
                   "age=17");
    auto uriQuery(pctEncode(uriQueryAllowed,httpQuery));
    xju::assert_equal(pctDecode(uriQueryAllowed,uriQuery),httpQuery);
  }
  xju::assert_equal(pctDecode("afc!%a",uriQueryAllowed),"afc!%a");
  xju::assert_equal(pctDecode("afc!%",uriQueryAllowed),"afc!%");
  xju::assert_equal(pctDecode("a%20 b",hcp::Chars("ab")),"a  b");
  xju::assert_equal(pctDecode("a%20 b",hcp::Chars("ab ")),"a%20 b");
}

}
}

using namespace xju::uri;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

