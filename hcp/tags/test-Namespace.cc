// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/tags/Namespace.hh>

#include <iostream>
#include <xju/assert.hh>

namespace hcp
{
namespace tags
{

void test1() {
  Namespace x;
  std::vector<Location> const l1(
    1U,
    Location(AbsolutePath("/"),
             FileName("fred.hh"),
             LineNumber(2)));
  
  x.addSymbol(std::vector<NamespaceName>{},
              UnqualifiedSymbol("fred"),
              l1);
  
  xju::assert_equal(x.lookup(std::vector<NamespaceName>{},
                             UnqualifiedSymbol("fred")),
                    l1);
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

