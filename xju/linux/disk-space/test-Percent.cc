// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/linux/disk-space/Percent.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace linux
{
namespace disk_space
{

void test1() {

  {
    Percent x(0);
    Percent y(50);
    Percent z(100);
    try{
      --x;
      xju::assert_never_reached();
    }
    catch(xju::Exception& e){
      xju::assert_equal(readableRepr(e),"--0% would underflow.");
    }
    try{
      ++z;
      xju::assert_never_reached();
    }
    catch(xju::Exception& e){
      xju::assert_equal(readableRepr(e),"++100% would overflow.");
    }
    try{
      Percent a{101};
      xju::assert_never_reached();
    }
    catch(xju::Exception& e){
      xju::assert_equal(readableRepr(e),"101% is invalid, must be <= 100%.");
    }
  }
}

}
}
}

using namespace xju::linux::disk_space;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

