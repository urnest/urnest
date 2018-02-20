// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/syscall.hh>

#include <iostream>
#include <xju/assert.hh>
#include <stdlib.h>
#include <unistd.h>

namespace xju
{

void test1() {
  xju::syscall("setenv",::setenv,XJU_TRACED)("XJU-TEST-SYSCALL","1",1);
  xju::syscall("dup",::dup,XJU_TRACED)(2);
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
