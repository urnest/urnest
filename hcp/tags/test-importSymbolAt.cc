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
#include <memory>
#include <xju/file/read.hh>

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
  //Lookup::
  virtual ::hcp::tags::FoundIn lookupSymbol(
    ::hcp::tags::Lookup::NamespaceNames const& fromScope,
    ::hcp::tags::Lookup::NamespaceNames const& symbolScope,
    ::hcp::tags::UnqualifiedSymbol const& symbol) /*throw(xju::Exception)*/
    {
      return f_(fromScope,symbolScope,symbol);
    }
  
  virtual ::hcp::tags::Lookup::ScopedNames lookupCompletions(
    ::hcp::tags::Lookup::NamespaceNames const& fromScope,
    ::hcp::tags::Lookup::NamespaceNames const& symbolScope,
    ::hcp::tags::UnqualifiedSymbol const& symbol) /*throw(xju::Exception)*/
    {
      return {};
    }
  
  F f_;
};
template<class F>
std::shared_ptr<hcp::tags::Lookup> l(F f)
{
  return std::shared_ptr<hcp::tags::Lookup>(new L<F>(f));
}

  
void test1(std::string const& x,
           std::string const& y,
           std::string const& z,
           std::string const& a) {

  std::vector<xju::path::AbsolutePath> const hpath{
    xju::path::split("/d2/.").first,
    xju::path::split("/d3/.").first};
  
  xju::assert_equal(
    importSymbolAt(
      x,
      16U,
      *l([](::hcp::tags::Lookup::NamespaceNames const& fromScope,
            ::hcp::tags::Lookup::NamespaceNames const& symbolScope,
            ::hcp::tags::UnqualifiedSymbol const& symbol){
           xju::assert_equal(fromScope,::hcp::tags::Lookup::NamespaceNames{hcp::tags::NamespaceName("A")});
           xju::assert_equal(symbolScope,::hcp::tags::Lookup::NamespaceNames{});
           xju::assert_equal(symbol,::hcp::tags::UnqualifiedSymbol("B"));
           return hcp::tags::FoundIn(
             ::hcp::tags::Locations{
             ::hcp::tags::Location(xju::path::split("/d2/.").first,
                                   xju::path::FileName("B.hh"),
                                   hcp::tags::LineNumber(3))
               },
             ::hcp::tags::Headers());
         }),
      hpath,
      {},
      false,false).first,
    std::string(y));
}

void test2(std::string const& x,
           std::string const& y,
           std::string const& z,
           std::string const& a) {

  std::vector<xju::path::AbsolutePath> const hpath{
    xju::path::split("/d2/.").first,
    xju::path::split("/d3/.").first};
  
  xju::assert_equal(
    importSymbolAt(
      y,
      33U,
      *l([](::hcp::tags::Lookup::NamespaceNames const& fromScope,
            ::hcp::tags::Lookup::NamespaceNames const& symbolScope,
            ::hcp::tags::UnqualifiedSymbol const& symbol){
           xju::assert_equal(fromScope,::hcp::tags::Lookup::NamespaceNames{hcp::tags::NamespaceName("A")});
           xju::assert_equal(symbolScope,::hcp::tags::Lookup::NamespaceNames{});
           xju::assert_equal(symbol,::hcp::tags::UnqualifiedSymbol("B"));
           return hcp::tags::FoundIn(
             ::hcp::tags::Locations{
             ::hcp::tags::Location(xju::path::split("/d2/.").first,
                                   xju::path::FileName("B.hh"),
                                   hcp::tags::LineNumber(3))
               },
             ::hcp::tags::Headers());
         }),
      hpath,
      {},
      false,false).first,
    std::string(y));
}
void test3(std::string const& x,
           std::string const& y,
           std::string const& z,
           std::string const& a) {

  std::vector<xju::path::AbsolutePath> const hpath{
    xju::path::split("/d2/.").first,
    xju::path::split("/d3/.").first};
  

  xju::assert_equal(
    importSymbolAt(
      y,
      37U,
      *l([](::hcp::tags::Lookup::NamespaceNames const& fromScope,
            ::hcp::tags::Lookup::NamespaceNames const& symbolScope,
            ::hcp::tags::UnqualifiedSymbol const& symbol){
           xju::assert_equal(fromScope,::hcp::tags::Lookup::NamespaceNames{hcp::tags::NamespaceName("A")});
           xju::assert_equal(symbolScope,::hcp::tags::Lookup::NamespaceNames{});
           xju::assert_equal(symbol,::hcp::tags::UnqualifiedSymbol("F"));
           return hcp::tags::FoundIn(
             ::hcp::tags::Locations{
             ::hcp::tags::Location(xju::path::split("/d1/.").first,
                                   xju::path::FileName("F.hh"),
                                   hcp::tags::LineNumber(7))
               },
             ::hcp::tags::Headers());
         }),
      hpath,
      {},
      false,false).first,
    std::string(z));
}
void test4(std::string const& x,
           std::string const& y,
           std::string const& z,
           std::string const& a) {

  std::vector<xju::path::AbsolutePath> const hpath{
    xju::path::split("/d2/.").first,
    xju::path::split("/d3/.").first};
  

  xju::assert_equal(
    importSymbolAt(
      z,
      75U,
      *l([](::hcp::tags::Lookup::NamespaceNames const& fromScope,
            ::hcp::tags::Lookup::NamespaceNames const& symbolScope,
            ::hcp::tags::UnqualifiedSymbol const& symbol){
           xju::assert_equal(fromScope,::hcp::tags::Lookup::NamespaceNames{hcp::tags::NamespaceName("A")});
           xju::assert_equal(symbolScope,::hcp::tags::Lookup::NamespaceNames{hcp::tags::NamespaceName("Q")});
           xju::assert_equal(symbol,::hcp::tags::UnqualifiedSymbol("Z"));
           return hcp::tags::FoundIn(
             ::hcp::tags::Locations{
             ::hcp::tags::Location(xju::path::split("/d2/Q/.").first,
                                   xju::path::FileName("Z.hcp"),
                                   hcp::tags::LineNumber(7))
               },
             ::hcp::tags::Headers());
         }),
      hpath,
      {std::make_pair(xju::path::RelativePath(
                        std::vector<DirName>{xju::path::DirName("d1")}),
                      xju::path::Extension(".hpp")),
       std::make_pair(xju::path::RelativePath(
                        std::vector<DirName>{xju::path::DirName("Q")}),
                      xju::path::Extension(".hh")),
       std::make_pair(xju::path::RelativePath(
                        std::vector<DirName>{}),
                      xju::path::Extension(".h"))},
      false,false).first,
    std::string(a));
  
}

void test5(std::string const& x,
           std::string const& y) {

  std::vector<xju::path::AbsolutePath> const hpath{
    xju::path::split("/d2/.").first,
    xju::path::split("/d3/.").first};
  
  xju::assert_equal(
    importSymbolAt(
      x,
      32U,
      *l([](::hcp::tags::Lookup::NamespaceNames const& fromScope,
            ::hcp::tags::Lookup::NamespaceNames const& symbolScope,
            ::hcp::tags::UnqualifiedSymbol const& symbol){
           xju::assert_equal(fromScope,::hcp::tags::Lookup::NamespaceNames{hcp::tags::NamespaceName("A")});
           xju::assert_equal(symbolScope,::hcp::tags::Lookup::NamespaceNames{});
           xju::assert_equal(symbol,::hcp::tags::UnqualifiedSymbol("B"));
           return hcp::tags::FoundIn(
             ::hcp::tags::Locations{::hcp::tags::Location(
                                         xju::path::split("/d2/.").first,
                                         xju::path::FileName("B.hcp"),
                                         hcp::tags::LineNumber(3))
                                         },
             ::hcp::tags::Headers());
         }),
      hpath,
      {std::make_pair(xju::path::RelativePath(
                        std::vector<DirName>{xju::path::DirName("d1")}),
                      xju::path::Extension(".hpp")),
       std::make_pair(xju::path::RelativePath(
                        std::vector<DirName>{xju::path::DirName("d2")}),
                      xju::path::Extension(".h")),
       std::make_pair(xju::path::RelativePath(
                        std::vector<DirName>{}),
                      xju::path::Extension(".hh"))},
      false,false).first,
    std::string(y));
}

void test6(std::string const& x,
           std::string const& y) {

  std::vector<xju::path::AbsolutePath> const hpath{
    xju::path::split("/d2/.").first,
    xju::path::split("/d3/.").first};
  
  xju::assert_equal(
    importSymbolAt(
      x,
      35U,
      *l([](::hcp::tags::Lookup::NamespaceNames const& fromScope,
            ::hcp::tags::Lookup::NamespaceNames const& symbolScope,
            ::hcp::tags::UnqualifiedSymbol const& symbol){
           xju::assert_equal(fromScope,::hcp::tags::Lookup::NamespaceNames{hcp::tags::NamespaceName("A")});
           xju::assert_equal(symbolScope,::hcp::tags::Lookup::NamespaceNames{hcp::tags::NamespaceName("std")});
           xju::assert_equal(symbol,::hcp::tags::UnqualifiedSymbol("ostream"));
           return hcp::tags::FoundIn(
             ::hcp::tags::Locations{},
             ::hcp::tags::Headers{::hcp::tags::Header("iostream")});
         }),
      hpath,
      {},
      false,false).first,
    std::string(y));
}


}
}

using namespace hcp::tags;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(xju::file::read(xju::path::split(argv[1])),
        xju::file::read(xju::path::split(argv[2])),
        xju::file::read(xju::path::split(argv[3])),
        xju::file::read(xju::path::split(argv[4]))), ++n;
  test2(xju::file::read(xju::path::split(argv[1])),
        xju::file::read(xju::path::split(argv[2])),
        xju::file::read(xju::path::split(argv[3])),
        xju::file::read(xju::path::split(argv[4]))), ++n;
  test3(xju::file::read(xju::path::split(argv[1])),
        xju::file::read(xju::path::split(argv[2])),
        xju::file::read(xju::path::split(argv[3])),
        xju::file::read(xju::path::split(argv[4]))), ++n;
  test4(xju::file::read(xju::path::split(argv[1])),
        xju::file::read(xju::path::split(argv[2])),
        xju::file::read(xju::path::split(argv[3])),
        xju::file::read(xju::path::split(argv[4]))), ++n;
  test5(xju::file::read(xju::path::split(argv[5])),
        xju::file::read(xju::path::split(argv[6]))), ++n;
  test6(xju::file::read(xju::path::split(argv[7])),
        xju::file::read(xju::path::split(argv[8]))), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

