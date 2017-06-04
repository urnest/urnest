// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/AutoFd.hh>

#include <iostream>
#include <xju/assert.hh>
#include "xju/syscall.hh"
#include <fcntl.h>
#include "xju/unistd.hh"
#include <utility>

namespace xju
{
void test1() {
  AutoFd devnull(::open("/dev/null",O_WRONLY));
  xju::syscall(xju::write,XJU_TRACED)(devnull.fd(),"x",1U);

  AutoFd devnull2(std::move(devnull));
  xju::syscall(xju::write,XJU_TRACED)(devnull2.fd(),"x",1U);
  try {
    xju::syscall(xju::write,XJU_TRACED)(devnull.fd(),"x",1U);
    xju::assert_never_reached();
  }
  catch(xju::SyscallFailed const&) {
  }

  devnull=std::move(devnull2);
  xju::syscall(xju::write,XJU_TRACED)(devnull.fd(),"x",1U);

  try {
    xju::syscall(xju::write,XJU_TRACED)(devnull2.fd(),"x",1U);
    xju::assert_never_reached();
  }
  catch(xju::SyscallFailed const&) {
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

