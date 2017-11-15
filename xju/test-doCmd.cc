// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/doCmd.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

void test1() {
  xju::assert_equal(doCmd({"/bin/echo", "fred"},
                          std::chrono::system_clock::now()+
                          std::chrono::seconds(1)),"fred\n");
  try{
    auto const x(doCmd({"/bin/sleep","2"},
                       std::chrono::system_clock::now()+
                       std::chrono::milliseconds(10)));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(e.cause().first,"timeout");
  }
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
