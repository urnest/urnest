// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/file/ls.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/path.hh>
#include <xju/file/mkdir.hh>
#include <xju/file/touch.hh>

namespace xju
{
namespace file
{

void test1() {
  auto const d(xju::path::splitdir("d"));
  xju::file::mkdir(d,xju::file::Mode(0777));
  xju::assert_equal(ls(d),std::set<xju::path::AbsFile>({}));
  xju::file::touch({d,xju::path::FileName("c")},xju::file::Mode(0777));
  xju::file::touch({d,xju::path::FileName("b")},xju::file::Mode(0777));
  xju::file::touch({d,xju::path::FileName("a")},xju::file::Mode(0777));
  xju::assert_equal(ls(d),std::set<xju::path::AbsFile>({
        {d,xju::path::FileName("a")},
        {d,xju::path::FileName("b")},
        {d,xju::path::FileName("c")}}));
  
}

}
}

using namespace xju::file;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

