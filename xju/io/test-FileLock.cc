// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/io/FileLock.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/path.hh>
#include <xju/file/Mode.hh>
#include <xju/file/touch.hh>
#include <xju/file/rm.hh>
#include <xju/file/write.hh>
#include <xju/io/FileReader.hh>
#include <xju/io/FileWriter.hh>
#include <xju/startsWith.hh>

namespace xju
{
namespace io
{

void test1() {
  auto const fileName(xju::path::split("test.txt"));
  xju::file::touch(fileName,xju::file::Mode(0777));
  xju::file::rm(fileName);

  xju::file::write(
    fileName,
    "fred",4,
    xju::file::Mode(0777));

  FileReader x(fileName);
  {
    FileLock l(x);
    FileWriter y(fileName);
    try{
      FileLock l(y);
      xju::assert_never_reached();
    }
    catch(FileLock::Unavailable const& e){
      auto const r(readableRepr(e));
      xju::assert_equal(xju::startsWith(r,std::string("Failed to lock ")),true);
    }
  }
  FileLock l(x);
}

}
}

using namespace xju::io;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

