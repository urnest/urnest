// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/MMap.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/file/write.hh>
#include <xju/file/read.hh>
#include <xju/path.hh>
#include <xju/io/FileWriter.hh>
#include <xju/io/FileReader.hh>

namespace xju
{

void test1() {
  auto const fred(xju::path::split("fred.txt"));
  xju::file::write(fred, "jock ", 5, xju::file::Mode(0666));
  {
    MMap x(fred, 0, 5);
    xju::assert_equal(x.addr<char>()[0], 'j');
    char* p(x.addr<char>());
    x.addr<char>()[4]='.';
    x.addr<char>()[4]='y';
    char c(p[4]);

    xju::assert_equal(c, 'y');
    x.sync();
    xju::assert_equal(xju::file::read(fred),"jocky");
    {
      xju::io::FileWriter y(fred);
      y.write("ing",3); //append
    }
    // after extend, to access new region, must re-map
    x = std::move(MMap(fred,0,8));
    xju::assert_equal(x.addr<char>()[7], 'g');
  }
}

void test2() {
  // like test1 but demonstrates holding file open always
  // and remapping after extending file
  auto const fred(xju::path::split("fred.txt"));
  xju::io::FileWriter y(fred, xju::file::Mode(0666));
  y.truncate(0);
  y.write("jock ", 5);
  y.sync();

  MMap x(fred, 0, 5);
  xju::assert_equal(x.addr<char>()[0], 'j');
  x.addr<char>()[4]='y';
  xju::assert_equal(x.addr<char>()[4], 'y');
  x.sync();
  xju::assert_equal(xju::file::read(fred),"jocky");

  // extend file, no need to write all data, but must write last
  // byte at least
  y.seekTo(9);
  y.write(".",1);
  y.sync();
  xju::assert_equal(y.size(),10U);
  
  // after extend, to access new region, must re-map
  x = std::move(MMap(fred,0,10));
  x.addr<char>()[5]='i';
  x.addr<char>()[6]='n';
  x.addr<char>()[7]='g';
  x.addr<char>()[8]='.';
  x.addr<char>()[9]='.';
  x.sync();
  xju::assert_equal(xju::file::read(fred),"jockying..");

}

void test3(){
  // mmap bigger than file (3 pages), then grow file to three pages
  // check we can write to 2nd page (which is a hole in the file)
  // via mmap and it puts the data into file (filling hole)
  auto const fred(xju::path::split("fred.txt"));
  xju::io::FileWriter y(fred, xju::file::Mode(0666));
  y.truncate(0);

  MMap x(fred, 0, 3*sysconf(_SC_PAGE_SIZE));
  y.seekTo(2*_SC_PAGE_SIZE+3);
  y.write(".",1);
  y.sync();
  xju::assert_equal(x.addr<char>()[2*_SC_PAGE_SIZE+3],'.');
  x.addr<char>()[_SC_PAGE_SIZE]=',';
  x.sync();
  {
    xju::io::FileReader r(fred);
    r.seekTo(_SC_PAGE_SIZE);
    char c;
    r.read(&c, 1);
    xju::assert_equal(c, ',');
  }
}

}

using namespace xju;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  test3(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

