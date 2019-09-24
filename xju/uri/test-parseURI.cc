// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/uri/parseURI.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace uri
{

void test1() {

  {
    const std::string s{"a8-._~%20!$&'()*+,;="};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),segment())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<SegmentItem>(r)),
      s);
  }
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

