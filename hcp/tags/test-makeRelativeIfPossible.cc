// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/tags/makeRelativeIfPossible.hh>

#include <iostream>
#include <xju/assert.hh>

namespace hcp
{
namespace tags
{

void test1() {
  std::vector<xju::path::AbsolutePath> const hpath{
    xju::path::split("/a/.").first,
    xju::path::split("/b/c/.").first,
    xju::path::split("/b/c/d/.").first};
  
  xju::assert_equal(makeRelativeIfPossible(xju::path::split("/x/y/z/f.h"),hpath),"/x/y/z/f.h");
  xju::assert_equal(makeRelativeIfPossible(xju::path::split("/a/p/q/f.h"),hpath),"p/q/f.h");
  xju::assert_equal(makeRelativeIfPossible(xju::path::split("/b/c/d/x/y/f.h"),hpath),"x/y/f.h");
  xju::assert_equal(makeRelativeIfPossible(xju::path::split("/b/c/x/y/f.h"),hpath),"x/y/f.h");
  xju::assert_equal(makeRelativeIfPossible(xju::path::split("/b/x/y/f.h"),hpath),"/b/x/y/f.h");
  xju::assert_equal(makeRelativeIfPossible(xju::path::split("/b/c/f.h"),hpath),"f.h");
  
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

