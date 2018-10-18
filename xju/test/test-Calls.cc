// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/test/Calls.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace test
{

class A
{
public:
  void f1() {}
  void f2(int c) {}
  int f3() {return 0;}
  int f4(std::string x) {return 0;}
};

void test1() {
  Calls c;

  A a;
  {
    c.enqueue(a,&A::f1);
    auto call{c.awaitCall(a,&A::f1,xju::steadyNow())};
    call->return_();
    call->awaitReturn(xju::steadyNow());
  }
}

}
}

using namespace xju::test;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

