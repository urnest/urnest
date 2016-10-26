// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/Subprocess.hh>

#include <iostream>
#include <xju/assert.hh>
#include <thread>

namespace xju
{

void test1() {
  int exitStatus(0);
  {
    Subprocess p(exitStatus,
                 [](){ return 47; },
                 [](pid_t) {});
  }
  xju::assert_not_equal(WIFEXITED(exitStatus),0);
  xju::assert_equal(WEXITSTATUS(exitStatus),47);
}

void test2() {
  int exitStatus(0);
  {
    Subprocess p(exitStatus,
                 [](){ 
                   while(true) {
                     std::this_thread::sleep_for(
                       std::chrono::seconds(10));
                   }
                   return 0;
                 });
  }
  xju::assert_not_equal(WIFSIGNALED(exitStatus),0);
  xju::assert_equal(WTERMSIG(exitStatus),9);
}
}

#include "xju/signal.hh"

namespace xju
{
void test3() {
  int exitStatus(0);
  {
    Subprocess p(exitStatus,
                 [](){ 
                   while(true) {
                     std::this_thread::sleep_for(
                       std::chrono::seconds(10));
                   }
                   return 0;
                 },
                 [](pid_t pid) {
                   xju::syscall(xju::kill,XJU_TRACED)(pid,2);
                 });
  }
  xju::assert_not_equal(WIFSIGNALED(exitStatus),0);
  xju::assert_equal(WTERMSIG(exitStatus),2);
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

