// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/tags/splitSymbol.hh>

#include <iostream>
#include <xju/assert.hh>

namespace hcp
{
namespace tags
{
typedef std::pair<std::vector<NamespaceName>,UnqualifiedSymbol> V;

void test1() {
  {
    auto const x(splitSymbol("a"));
    xju::assert_equal(x, V(std::vector<NamespaceName>(),
                           UnqualifiedSymbol("a")));
  }
  {
    auto const x(splitSymbol("x::a"));
    xju::assert_equal(x, V( {NamespaceName("x")},
                            UnqualifiedSymbol("a")));
  }
  {
    auto const x(splitSymbol("::y::x::a"));
    xju::assert_equal(x, V( {NamespaceName("y"),NamespaceName("x")},
                            UnqualifiedSymbol("a")));
  }
  
        
}

}
}

using namespace hcp::tags;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

