// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/io/FileReader.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/now.hh>
#include <xju/file/write.hh>
#include <xju/startsWith.hh>
#include <xju/file/touch.hh>
#include <xju/file/rm.hh>

namespace xju
{
namespace io
{

void test1() {
  auto const fileName(xju::path::split("test.txt"));
  xju::file::touch(fileName,xju::file::Mode(0777));
  xju::file::rm(fileName);
  try{
    FileReader x(fileName);
  }
  catch(xju::SyscallFailed const& e){
    xju::assert_equal(e._errno,ENOENT);
  }
  
  xju::file::write(
    fileName,
    "fred",4,
    xju::file::Mode(0777));

  FileReader x(fileName);
  std::string data(4U,' ');
  data.resize(x.read(&data[0],2));
  xju::assert_equal(data,"fr");

  data.resize(4U);
  data.resize(x.read(&data[0],3));
  xju::assert_equal(data,"ed");

  try{
    data.resize(1U);
    data.resize(x.read(&data[0],1));
    xju::assert_never_reached();
  }
  catch(xju::io::EndOfFile const& e){
    std::string const r(readableRepr(e));
    xju::assert_equal(xju::startsWith(r,std::string("Failed to read up to 1 bytes at offset 4 of file")),true);
  }

  x.seekBy(-1);
  data.resize(4U);
  data.resize(x.read(&data[0],1));
  xju::assert_equal(data,"d");
  
  x.seekTo(1);
  data.resize(4U);
  data.resize(x.read(&data[0],2));
  xju::assert_equal(data,"re");
  
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

