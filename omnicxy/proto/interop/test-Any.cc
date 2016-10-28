// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <omniORB4/CORBA.h>

#include <cxy/Any.hh>

#include <iostream>
#include <xju/assert.hh>
#include <cinttypes>

#include <omnicxy/proto/cxx/p20.hh>
#include <omnicxy/proto/p20.any.hh>

namespace cxy
{

void test1() {
  CORBA::Short x(-23);
  CORBA::Any a;
  a <<= x;
  
  cdrMemoryStream s;
  a >>= s;
  
  ::cxy::Any<> b(::cxy::cdr<Any<> >::unmarshalFrom(s));
  
  xju::assert_equal(b.isA<int16_t>(),true);
  int16_t const y(b.get<int16_t>());
  xju::assert_equal(x, y);

  {
    ::cxy::Any<> const b2(y);
    cdrMemoryStream s2;
    ::cxy::cdr< Any<> >::marshal(b2,s2);
    CORBA::Any a2;
    a2 <<= s2;
    CORBA::Short x2;
    a2 >>= x2;
    xju::assert_equal(x2,x);
  }
}

}

using namespace cxy;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}
