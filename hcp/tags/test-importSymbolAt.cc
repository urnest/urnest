// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/tags/importSymbolAt.hh>

#include <iostream>
#include <xju/assert.hh>

namespace hcp
{
namespace tags
{

template<class F>
class L : public hcp::tags::Lookup
{
public:
  explicit L(F f) throw():
      f_(f){
  }
  virtual ::hcp::tags::Lookup::Locations lookupSymbol(
    ::hcp::tags::Lookup::NamespaceNames const& fromScope,
    ::hcp::tags::Lookup::NamespaceNames const& symbolScope,
    ::hcp::tags::UnqualifiedSymbol const& symbol) throw(xju::Exception)
    {
      return f_(fromScope,symbolScope,symbol);
    }
  
  F f_;
};
template<class F>
L l(F f)
{
  return L<F>(f);
}

  
void test1() {
  char const x[]=
    "class A : class B {};";
  char const y[]=
    "#include <B.hh>\n"
    "class A : class B { F c;};";
  char const z[]=
    "#include <B.hh>\n"
    "#include <d1/F.hh>\n"
    "class A : class B { F c;};";

  std::vector<xju::path::AbsolutePath> const hpath{
    xju::path::split("/d2/.").first,
    xju::path::split("/d3/.").first};
  
  xju::assert_equal(
    importSymbolAt(
      x,
      16U,
      false,
      l([](::hcp::tags::Lookup::NamespaceNames const& fromScope,
           ::hcp::tags::Lookup::NamespaceNames const& symbolScope,
           ::hcp::tags::UnqualifiedSymbol const& symbol){
          xju::assert_equal(fromScope,::hcp::tags::Lookup::NamespaceNames{});
          xju::assert_equal(symbolScope,::hcp::tags::Lookup::NamespaceNames{});
          xju::assert_equal(symbol,::hcp::tags::UnqualifiedSymbol("B"));
          return ::hcp::tags::Lookup::Locations{
            ::hcp::tags::Lookup::Location(xju::path::split("/d2/.").first,
                                          xju::path::FileName("B.hh"),
                                          hcp::tags::LineNumber(3))
              };
        }),
      hpath),
    y);
  
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

