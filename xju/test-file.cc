// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/file.hh>

#include <xju/assert.hh>
#include <iostream>

namespace xju
{

void test1() {
  auto const f(xju::path::split("x/y"));
  auto const d(xju::path::dirname(f));
  
  xju::file::mkdir(d,0777);
  xju::file::touch(f,0777);
  xju::assert_equal(xju::file::read(f),"");
  xju::file::write(f,"fred",0777);
  auto const f2(xju::path::split("z"));
  xju::file::rename(f,f2);
  xju::assert_equal(xju::file::read(f2),"fred");
}

}

using namespace xju;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

