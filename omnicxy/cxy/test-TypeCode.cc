// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <cxy/TypeCode.hh>

#include <iostream>
#include <xju/assert.hh>

#include <omniORB4/CORBA.h>
#include <omniORB4/cdrStream.h>

namespace cxy
{

template<class T>
void testx(T const v) {
  cdrMemoryStream s;
  std::shared_ptr<TypeCode> const x(cxy::typeCodeOf(v));
  x->marshal(s);
  cdr<T>::marshal(v,s);

  std::shared_ptr<TypeCode> y(
    cdr< std::shared_ptr<TypeCode> >::unmarshalFrom(s));
  xju::assert_equal(*x,*y);
  
  cdrMemoryStream t;
  y->copyValue(s,t);
  
  xju::assert_equal(cdr<T>::unmarshalFrom(t),v);
}

void test1()
{
  testx((int16_t)27);
  //testx((int32_t)27);
  testx(std::vector<int16_t>{3});
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

