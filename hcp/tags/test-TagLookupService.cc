// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/tags/TagLookupService.hh>

#include <iostream>
#include <xju/assert.hh>
#include "xju/Thread.hh"
#include <xju/file/write.hh>
#include <xju/file/rename.hh>
#include <xju/file/rm.hh>

namespace hcp
{
namespace tags
{

void test1() {
  std::pair<xju::path::AbsolutePath,xju::path::FileName> const f1(
    xju::path::split("f1.tags"));
  std::pair<xju::path::AbsolutePath,xju::path::FileName> const f2(
    xju::path::split("f2.tags"));
  
  // no files yet
  TagLookupService x({f1,f2});
  xju::Thread t([&]() { x.run(); } );

  xju::assert_equal(x.lookupSymbol(TagLookupService::NamespaceNames(),
                                   TagLookupService::NamespaceNames(),
                                   UnqualifiedSymbol("x")),
                    Lookup::Locations());
  
  // create a file
  xju::file::write(f1,
                   "{ \"x\":  { \"f\":\"/src/x.hh\",\"l\":23 } }",
                   0777);

  auto const x_hh(xju::path::split("/src/x.hh"));
  
  // lookup symbol
  xju::assert_equal(x.lookupSymbol(TagLookupService::NamespaceNames(),
                                   TagLookupService::NamespaceNames(),
                                   UnqualifiedSymbol("x")),
                    Lookup::Locations({Location(x_hh.first,x_hh.second,LineNumber(23))}));
  
  // lookup non-existent symbol
  xju::assert_equal(x.lookupSymbol(TagLookupService::NamespaceNames(),
                                   TagLookupService::NamespaceNames(),
                                   UnqualifiedSymbol("y")),
                    Lookup::Locations());
  
  // create new file
  xju::file::write(f2,
                   "{ \"y\":  { \"f\":\"/src/y.hh\",\"l\":12 } }",
                   0777);
  
  auto const y_hh(xju::path::split("/src/y.hh"));

  // redo lookup
  xju::assert_equal(x.lookupSymbol(TagLookupService::NamespaceNames(),
                                   TagLookupService::NamespaceNames(),
                                   UnqualifiedSymbol("y")),
                    Lookup::Locations({Location(y_hh.first,y_hh.second,LineNumber(12))}));

  // lookup non-existent symbol
  xju::assert_equal(x.lookupSymbol(TagLookupService::NamespaceNames(),
                                   TagLookupService::NamespaceNames(),
                                   UnqualifiedSymbol("z")),
                    Lookup::Locations());

  // update a file
  std::pair<xju::path::AbsolutePath,xju::path::FileName> const f2new(
    xju::path::split("f2.tags.new"));
  xju::file::write(f2new,
                   "{ \"y\":  { \"f\":\"/src/y.hh\",\"l\":12 },"
                   "  \"z\":  { \"f\":\"/src/z.hh\",\"l\":99 } }",
                   0777);

  xju::file::rename(f2new,f2);
  
  auto const z_hh(xju::path::split("/src/z.hh"));
  // redo lookup
  xju::assert_equal(x.lookupSymbol(TagLookupService::NamespaceNames(),
                                   TagLookupService::NamespaceNames(),
                                   UnqualifiedSymbol("z")),
                    Lookup::Locations({Location(z_hh.first,z_hh.second,LineNumber(99))}));

  // remove file
  xju::file::rm(f2);
  
  // check lookup fails
  xju::assert_equal(x.lookupSymbol(TagLookupService::NamespaceNames(),
                                   TagLookupService::NamespaceNames(),
                                   UnqualifiedSymbol("z")),
                    Lookup::Locations());
  
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

