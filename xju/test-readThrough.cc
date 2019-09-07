// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/readThrough.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

void test1() {

  {
    std::istringstream s{"\r\n"};
    xju::assert_equal(readThrough(s,"\r\n",2),"\r\n");
  }
  {
    std::istringstream s{"fred\r\n"};
    xju::assert_equal(readThrough(s,"\r\n",6),"fred\r\n");
  }
  {
    std::istringstream s{"fred\r\njock"};
    xju::assert_equal(readThrough(s,"\r\n",6),"fred\r\n");
  }
  {
    std::istringstream s{"fred\r\njock"};
    xju::assert_equal(readThrough(s,"\r\n",7),"fred\r\n");
  }
  {
    std::istringstream s{"f\re\n\r\njock"};
    xju::assert_equal(readThrough(s,"\r\n",7),"f\re\n\r\n");
  }
  {
    std::istringstream s{"f\re\r\r\njock"};
    xju::assert_equal(readThrough(s,"\r\n",7),"f\re\r\r\n");
  }
  
}

void test2() {

  {
    std::istringstream s{"\r\n"};
    try{
      auto const x{readThrough(s,"\r\n",1)};
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      xju::assert_equal(readableRepr(e),"Failed to read stream until \"\\r\\n\" reading at most 1 chars because\n1 chars read without seeing \"\\r\\n\" amongst \"\\r\".");
    }
  }
  {
    std::istringstream s{"fred\r"};
    try{
      auto const x{readThrough(s,"\r\n",6)};
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      xju::assert_equal(readableRepr(e),"Failed to read stream until \"\\r\\n\" reading at most 6 chars because\nend of file before \"\\r\\n\" amongst \"fred\\r\".");
    }
  }
  {
    std::istringstream s{"fred\r\n"};
    try{
      auto const x{readThrough(s,"\r\n",5)};
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      xju::assert_equal(readableRepr(e),"Failed to read stream until \"\\r\\n\" reading at most 5 chars because\n5 chars read without seeing \"\\r\\n\" amongst \"fred\\r\".");
    }
  }
}

}

using namespace xju;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

