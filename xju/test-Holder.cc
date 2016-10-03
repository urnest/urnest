// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/Holder.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

int constructs(0);
int destructs(0);

class X
{
public:
  X() throw()
  {
    ++constructs;
  }
  ~X() throw()
  {
    ++destructs;
  }
};
  
void test1() {
  {
    Holder<X> h;
    xju::assert_equal(constructs,0);
    xju::assert_equal(destructs,0);
    
    X* const x(new(h) X);
    xju::assert_equal(constructs,1);
    xju::assert_equal(destructs,0);
    
    x->~X();
    xju::assert_equal(constructs,1);
    xju::assert_equal(destructs,1);
  }
  xju::assert_equal(constructs,1);
  xju::assert_equal(destructs,1);
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

