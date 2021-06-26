// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/crypt/I.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace crypt
{

void test1() {
  I const x(0x1000);
  xju::assert_equal(x.value(),std::vector<uint8_t>({0x10,0x00}));
  xju::assert_equal(I(std::vector<uint8_t>({0x10,0x00})),x);
  xju::assert_less(x,I(0x1001));
  xju::assert_less_equal(x,I(0x1001));
  xju::assert_less_equal(x,x);
  xju::assert_greater(x,I(0x09ff));
  xju::assert_greater_equal(x,I(0x09ff));
  xju::assert_greater_equal(x,x);
  xju::assert_equal(I(x),x);
  {
    I x(255);
    xju::assert_equal(x.value(),std::vector<uint8_t>({0,255}));
  }
  {
    I x(0UL);
    xju::assert_equal(x.value(),std::vector<uint8_t>());
  }
  {
    I x(1);
    xju::assert_equal(neg(x).value(),std::vector<uint8_t>({0xff}));
  }
  {
    I y(1);
    xju::assert_not_equal(y,x);
    y=x;
    xju::assert_equal(y,x);

    I z(std::move(y));
    xju::assert_equal(z,x);

    I b(22);
    z=std::move(b);
    xju::assert_not_equal(z,x);
    xju::assert_equal(z,I(22));
  }

  {
    I y(1);
    I z(neg(y));
    xju::assert_equal(z.value(),std::vector<uint8_t>({0xff}));
    I const a(abs(z));
    xju::assert_equal(a,y);
  }
  {
    I a(22);
    I b(33);
    I const c(33);
    xju::assert_equal(b,c);
    xju::assert_not_equal(a,c);
    std::swap(a,b);
    xju::assert_equal(a,c);
    xju::assert_not_equal(b,c);
  }
  {
    I const a(22);
    I const b(33);
    xju::assert_equal(a+11,b);
    xju::assert_equal(b-11,a);
    xju::assert_equal(a/2,I(11));
    xju::assert_equal(a*2,I(44));
    xju::assert_equal(a%2,I(0UL));
    xju::assert_equal(a%21,I(1));
    xju::assert_equal(mul_2exp(a,2),I(88));
    xju::assert_equal(div(a,I(9)),std::pair<I,I>(2,4));
    xju::assert_equal(div(a,I(9)),std::pair<I,I>(2,4));

    xju::assert_equal(div(neg(a),I(9)),std::pair<I,I>(neg(I(2)),neg(I(4))));
    xju::assert_equal(div(a,neg(I(9))),std::pair<I,I>(neg(I(2)),I(4)));
  }
  xju::assert_equal(powm(I(2),I(4),I(1)),I(0UL));
  xju::assert_equal(powm(I(2),I(4),I(16)),I(0UL));
  xju::assert_equal(powm(I(2),I(4),I(7)),I(2));

  xju::assert_equal(gcd(I(27),I(24)),I(3));
}

void test2()
{
  I const x(std::vector<uint8_t>({0x1f,0xff}));
  I b(randl(x));
  xju::assert_less(b,x);
}

}
}

using namespace xju::crypt;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

