// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/tags/augmentRootNamespace.hh>

#include <iostream>
#include <xju/assert.hh>

namespace hcp
{
namespace tags
{

void test1(std::string const& test1) {
  Namespace rootNamespace;
  augmentRootNamespace(rootNamespace,xju::path::split(test1),true);
  {
    auto const foundIn(rootNamespace.lookup(
                         std::vector<NamespaceName>(),
                         std::vector<NamespaceName>(),
                         UnqualifiedSymbol("x")));
    auto const locations(foundIn.locations_);
    xju::assert_equal(locations.size(),1U);
    auto const l(locations[0]);
    xju::assert_equal(l.file, FileName("test-1.hcp"));
    xju::assert_equal(l.line, LineNumber(2));
    xju::assert_equal(foundIn.headers_.size(),0U);
  }
  {
    auto const foundIn(rootNamespace.lookup(
                         std::vector<NamespaceName>(),
                         std::vector<NamespaceName>(),
                         UnqualifiedSymbol("y")));
    auto const locations(foundIn.locations_);
    xju::assert_equal(locations.size(),1U);
    auto const l(locations[0]);
    xju::assert_equal(l.file, FileName("test-1.hcp"));
    xju::assert_equal(l.line, LineNumber(3));
    xju::assert_equal(foundIn.headers_.size(),0U);
  }
  
}

void test2(std::string const& test4) {
  Namespace rootNamespace;
  augmentRootNamespace(rootNamespace,xju::path::split(test4),true);
  {
    auto const foundIn(rootNamespace.lookup(
                         std::vector<NamespaceName>(),
                         
                         {NamespaceName("n2")},
                         UnqualifiedSymbol("C")));
    auto const locations(foundIn.locations_);
    xju::assert_equal(locations.size(),1U);
    auto const l(locations[0]);
    xju::assert_equal(l.file, FileName("test-4.h"));
    xju::assert_equal(l.line, LineNumber(3));
    xju::assert_equal(foundIn.headers_.size(),0U);
  }
}

void test3(std::string const& test5) {
  Namespace rootNamespace;
  augmentRootNamespace(rootNamespace,xju::path::split(test5),true);
  {
    auto const foundIn(rootNamespace.lookup(
                         std::vector<NamespaceName>(),
                         
                         {NamespaceName("n2")},
                         UnqualifiedSymbol("C")));
    auto const locations(foundIn.locations_);
    xju::assert_equal(locations.size(),0U);
    xju::assert_equal(foundIn.headers_.size(),1U);
    auto const h(foundIn.headers_[1]);
    xju::assert_equal(h, Header("<iostream>"));
  }
}

}
}

using namespace hcp::tags;

int main(int argc, char* argv[])
{
  xju::assert_equal(argc,3);
  unsigned int n(0);
  test1(argv[1]), ++n; // ../test-hcp-tags/test-1.json
  test2(argv[2]), ++n; // ../test-hcp-tags/test-4.json
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

