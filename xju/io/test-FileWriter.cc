// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/io/FileWriter.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/file/rm.hh>
#include <xju/file/read.hh>
#include <xju/file/touch.hh>

namespace xju
{
namespace io
{

void test1() {
  auto const fileName(xju::path::split("test.txt"));
  xju::file::touch(fileName,xju::file::Mode(0777));
  xju::file::rm(fileName);
  try{
    FileWriter x(fileName);
  }
  catch(xju::SyscallFailed const& e){
    xju::assert_equal(e._errno,ENOENT);
  }
  {
    FileWriter x(fileName,xju::file::Mode(0777),FileWriter::Exclusive());
    x.write("fred",4);
    x.write("jock",4);
  }
  xju::assert_equal(xju::file::read(fileName),
                    std::string("fredjock"));
  try{
    FileWriter x(fileName,xju::file::Mode(0777),FileWriter::Exclusive());
    xju::assert_never_reached();
  }
  catch(xju::SyscallFailed const& e){
    xju::assert_equal(e._errno,EEXIST);
  }
  {
    FileWriter x(fileName);
    x.write("sally",5);
    xju::assert_equal(x.seekBy(-4),9);
    x.write("i",1);
  }
  xju::assert_equal(xju::file::read(fileName),
                    std::string("fredjocksilly"));
  {
    xju::file::rm(fileName);
    FileWriter x(fileName,xju::file::Mode(0777));
    x.write("fred",4);
    x.write("jock",4);
  }
  xju::assert_equal(xju::file::read(fileName),
                    std::string("fredjock"));
  {
    FileWriter x(fileName);
    x.write("sally",5);
    xju::assert_equal(&x.seekTo(1),&x);
    x.write("e",1);
  }
  xju::assert_equal(xju::file::read(fileName),
                    std::string("feedjocksally"));
  {
    FileWriter x(fileName);
    x.truncate(4);
    x.write("sally",5);

    try{
      x.seekTo(-1);
      xju::assert_never_reached();
    }
    catch(xju::SyscallFailed const& e){
      xju::assert_equal(e._errno,EINVAL);
    }
    
  }
  xju::assert_equal(xju::file::read(fileName),
                    std::string("feedsally"));
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

