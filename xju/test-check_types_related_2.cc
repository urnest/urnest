// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "check_types_related.hh"

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
class A
{
};
class B
{
};
  
void test1() {
  // note that both these statements should generate compile errors ie
  // this code is designed *not* to compile
  check_types_related<A, B>(); // must not compile
  check_types_related<B, A>(); // must not compile
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

