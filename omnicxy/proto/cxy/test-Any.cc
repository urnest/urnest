// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <iostream>
#include <xju/assert.hh>
#include <cinttypes>
#include <vector>
#include <omnicxy/proto/p13.any.hh>
#include <omnicxy/proto/p12.any.hh>
#include <cxy/Any.hh>
#include <cxy/MemCdrStream.hh>

void test1() {
  {
    std::shared_ptr<p13::U1 const> x(
      new p13::U1::V<1>(5));
    ::cxy::Any<> const a(x);
    xju::assert_equal(a.isA<std::shared_ptr<p13::U1 const> >(),true);

    auto const y(a.get<std::shared_ptr<p13::U1  const> >());
    xju::assert_equal(*x, *y);

    ::cxy::MemCdrStream s;
    ::cxy::cdr< ::cxy::Any<> >::marshal(a,*s);
    ::cxy::Any<> b(cxy::cdr< ::cxy::Any<> >::unmarshalFrom(*s));
    auto const z(b.get<std::shared_ptr<p13::U1  const> >());
    xju::assert_equal(*x, *z);
  }
}

void test2() {
  {
    std::shared_ptr<p12::U1 const> x(
      new p12::U1::A(5));
    ::cxy::Any<> const a(x);
    xju::assert_equal(a.isA<std::shared_ptr<p12::U1 const> >(),true);

    auto const y(a.get<std::shared_ptr<p12::U1  const> >());
    xju::assert_equal(*x, *y);

    ::cxy::MemCdrStream s;
    ::cxy::cdr< ::cxy::Any<> >::marshal(a,*s);
    ::cxy::Any<> b(cxy::cdr< ::cxy::Any<> >::unmarshalFrom(*s));
    auto const z(b.get<std::shared_ptr<p12::U1  const> >());
    xju::assert_equal(*x, *z);
  }
}

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}
