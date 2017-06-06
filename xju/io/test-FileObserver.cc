// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/io/FileObserver.hh>

namespace xju
{
namespace io
{
void test0() //header check
{
  FileObserver o({});
}

}
}

#include <iostream>
#include <xju/assert.hh>
#include <xju/file/mkdir.hh>
#include <xju/file/touch.hh>
#include <xju/file/write.hh>
#include <xju/file/rename.hh>
#include <xju/io/select.hh>
#include <xju/now.hh>

namespace xju
{
namespace io
{
    
void test1() {
  auto const f1(xju::path::split("d1/f1"));
  auto const f2(xju::path::split("d2/f2"));
  auto const f3(xju::path::split("d1/f3"));

  try {
    FileObserver o( {f1,f2} );
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
  }
      
  xju::file::mkdir(xju::path::dirname(f1),0777);
  xju::file::mkdir(xju::path::dirname(f2),0777);
  
  FileObserver o( {f1,f2} );
  xju::assert_equal(xju::io::select({&o},xju::now()).size(),0U);
  xju::assert_equal(o.read(xju::now()).size(),0U);
  
  // create watched file
  xju::file::touch(f1,0777);
  xju::assert_equal(xju::io::select({&o},xju::now()).size(),1U);

  {
    auto const r(o.read(xju::now()));
    xju::assert_equal(r.size(),1U);
    xju::assert_equal(*r.begin(),f1);
  }
  xju::assert_equal(xju::io::select({&o},xju::now()).size(),0U);
                       
  // touch existing file - note no change
  xju::file::touch(f1,0777);
  xju::assert_equal(xju::io::select({&o},xju::now()).size(),0U);
                       
  // create non-watched file
  xju::file::write(f3,"fred",0777);
  xju::assert_equal(o.read(xju::now()).size(),0U);
  xju::assert_equal(xju::io::select({&o},xju::now()).size(),0U);
  
  // create watched file
  xju::file::touch(f2,0777);
  xju::assert_equal(xju::io::select({&o},xju::now()).size(),1U);

  {
    auto const r(o.read(xju::now()));
    xju::assert_equal(r.size(),1U);
    xju::assert_equal(*r.begin(),f2);
  }
  xju::assert_equal(xju::io::select({&o},xju::now()).size(),0U);
                       
  // modify watched file via rename
  xju::file::rename(f3,f2);
  xju::assert_equal(xju::io::select({&o},xju::now()).size(),1U);

  {
    auto const r(o.read(xju::now()));
    xju::assert_equal(r.size(),1U);
    xju::assert_equal(*r.begin(),f2);
  }
  xju::assert_equal(xju::io::select({&o},xju::now()).size(),0U);

  // delete watched file via rename
  xju::file::rename(f2,f3);
  xju::assert_equal(xju::io::select({&o},xju::now()).size(),1U);

  {
    auto const r(o.read(xju::now()));
    xju::assert_equal(r.size(),1U);
    xju::assert_equal(*r.begin(),f2);
  }
  xju::assert_equal(xju::io::select({&o},xju::now()).size(),0U);

  // two mods
  xju::file::write(f3,"pete",0777);
  xju::file::rename(f3,f1);
  xju::file::write(f3,"jock",0777);
  xju::file::rename(f3,f2);
  xju::assert_equal(xju::io::select({&o},xju::now()).size(),1U);

  {
    auto const r(o.read(xju::now()));
    xju::assert_equal(r.size(),2U);
    xju::assert_equal(r,std::set<std::pair<xju::path::AbsolutePath,xju::path::FileName>>({f1,f2}));
  }
  
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

