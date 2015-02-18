//     -*- mode: c++ ; c-file-style: "xju" ; -*-
//
// Copyright (c) 2008
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/Float.hh"

#include "xju/assert.hh"
#include <iostream>

class Tag
{
};

typedef xju::Float<Tag, double> T;

std::ostream& operator<<(std::ostream& s, T const& x) throw()
{
   return s << x.value();
}

void test1()
{
   T x(0);
   xju::assert_equal(T(0), T(0));
   xju::assert_equal(T(0)+T(1.2), T(1.2));
   xju::assert_equal(T(1.2)-T(1.2), T(0));
   xju::assert_not_equal(T(0), T(1.2));
   xju::assert_(T(0), std::less<T>(), T(1.2));
   xju::assert_(T(0), std::less_equal<T>(), T(1.2));
   xju::assert_(T(1), std::less_equal<T>(), T(1.2));
   xju::assert_(T(1.2), std::greater<T>(), T(0));
   xju::assert_(T(0), std::greater_equal<T>(), T(0));
   xju::assert_(T(1.2), std::greater_equal<T>(), T(0));
   
}
void test2()
{
  T x(std::numeric_limits<double>::max());
  try {
    x=x+T(0.1);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"1.79769e+308 + 0.1 would overflow.");
  }
  try {
    x=x-T(-0.1);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"1.79769e+308 + 0.1 would overflow.");
  }
}

void test3()
{
  T x(std::numeric_limits<double>::min());
  try {
    x=x-T(0.1);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"2.22507e-308 - 0.1 would underflow.");
  }
  try {
    x=x+T(-0.1);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"2.22507e-308 - 0.1 would underflow.");
  }
}

void test4()
{
  T x(std::numeric_limits<double>::min());
  xju::assert_equal(isnan(x),false);
}

int main(int argc, char* argv[])
{
   unsigned int n(0);
   test1(); ++n;
   test2(); ++n;
   test3(); ++n;
   test4(); ++n;
   std::cout << "PASS " << n << " steps" << std::endl;
   return 0;
}
