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
#include <xju/unistd.hh>

namespace xju
{

void test1(std::string self) {
  {
    auto const x(doCmd({"/bin/echo", "fred"},
                       std::chrono::steady_clock::now()+
                       std::chrono::seconds(1)));
    xju::assert_equal(x.first,"fred\n");
    xju::assert_equal(x.second,"");
  }
  {
    auto const x(doCmd({self, "w"},
                       std::chrono::steady_clock::now()+
                       std::chrono::seconds(1)));
    xju::assert_equal(x.first,"");
    xju::assert_equal(x.second,"warning\n");
  }
  try{
    auto const x(doCmd({self,"e"},
                       std::chrono::steady_clock::now()+
                       std::chrono::seconds(1)));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(e.cause().first,"command failed with status 256, stdout \"\" and stderr \"error\\n\"");
  }
  
  try{
    auto const x(doCmd({"/bin/sleep","2"},
                       std::chrono::steady_clock::now()+
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
  if (argc>1 && argv[1]==std::string("w")){
    std::string const x("warning\n");
    xju::syscall(xju::write,XJU_TRACED)(2,x.c_str(),x.size());
    return 0;
  }
  if (argc>1 && argv[1]==std::string("e")){
    std::string const x("error\n");
    xju::syscall(xju::write,XJU_TRACED)(2,x.c_str(),x.size());
    return 1;
  }
  unsigned int n(0);
  test1(argv[0]), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}
