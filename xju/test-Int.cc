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
#include "xju/Int.hh"

#include "xju/assert.hh"
#include <iostream>

class Tag
{
};

typedef xju::Int<Tag, int64_t> T;

std::ostream& operator<<(std::ostream& s, T const& x) throw()
{
   return s << x.value();
}

void test1()
{
   T x(0);
   xju::assert_equal(T(0), T(0));
   xju::assert_equal(T(0)+T(1), T(1));
   xju::assert_equal(T(1)-T(1), T(0));
   xju::assert_not_equal(T(0), T(1));
   xju::assert_(T(0), std::less<T>(), T(1));
   xju::assert_(T(0), std::less_equal<T>(), T(1));
   xju::assert_(T(1), std::less_equal<T>(), T(1));
   xju::assert_(T(1), std::greater<T>(), T(0));
   xju::assert_(T(0), std::greater_equal<T>(), T(0));
   xju::assert_(T(1), std::greater_equal<T>(), T(0));
   xju::assert_equal(++x,T(1));
   xju::assert_equal(x,T(1));
   xju::assert_equal(x++,T(1));
   xju::assert_equal(x,T(2));
   xju::assert_equal(--x,T(1));
   xju::assert_equal(x,T(1));
   xju::assert_equal(x--,T(1));
   xju::assert_equal(x,T(0));
   
}
void test2()
{
  T x(std::numeric_limits<int64_t>::max());
  try {
    x=x+T(1);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"9223372036854775807 + 1 would overflow.");
  }
  try {
    x=x-T(-1);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"9223372036854775807 + 1 would overflow.");
  }
}

void test3()
{
  T x(std::numeric_limits<int64_t>::min());
  try {
    x=x-T(1);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"-9223372036854775808 - 1 would underflow.");
  }
  try {
    x=x+T(-1);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"-9223372036854775808 - 1 would underflow.");
  }
}

int main(int argc, char* argv[])
{
   unsigned int n(0);
   test1(); ++n;
   test2(); ++n;
   test3(); ++n;
   std::cout << "PASS " << n << " steps" << std::endl;
   return 0;
}
