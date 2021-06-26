// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/crypt/bit_iterator.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace crypt
{

void test1() {

  // empty container;
  {
    std::vector<uint8_t> x;
    xju::assert_equal(bit_begin(x), bit_end(x));
    std::vector<uint8_t> const& y(x);
    xju::assert_equal(bit_const_begin(x), bit_const_end(x));
  }

  std::vector<uint8_t> const v{0xf3, 0x21};
  auto x(bit_begin(v));
  xju::assert_less(x, bit_end(v));
  xju::assert_equal(x.containingByte(),(uint8_t)0x21);
  xju::assert_equal(*x, true);
  xju::assert_equal(*x++, true);
  xju::assert_less(bit_begin(v),x);
  xju::assert_equal(*x++, false);
  xju::assert_equal(x-bit_begin(v), 2);
  xju::assert_equal(*--x, false);
  xju::assert_equal(*--x, true);
  xju::assert_equal(*++x, false);
  xju::assert_equal(x-bit_begin(v), 1);
  ++x;
  ++x;
  ++x;
  xju::assert_equal(x.get(),false);
  ++x;
  xju::assert_equal(x.get(),true);
  ++x;
  ++x;
  xju::assert_equal(x-bit_begin(v), 7);
  xju::assert_equal(x.containingByte(),(uint8_t)0x21);

  x++;
  xju::assert_equal(x-bit_begin(v), 8);
  xju::assert_equal(x.containingByte(),(uint8_t)0xf3);
  xju::assert_equal(x.get(),true);
  xju::assert_equal(*++x,true);
  xju::assert_equal(*++x,false);
  xju::assert_equal(x-bit_begin(v), 10);
  ++x;
  xju::assert_equal(*x,false);
  ++x;
  xju::assert_equal(*x,true);
  ++x;
  xju::assert_equal(*x,true);
  ++x;
  xju::assert_equal(*x,true);
  ++x;
  xju::assert_equal(x.containingByte(),(uint8_t)0xf3);
  xju::assert_equal(x-bit_begin(v), 15);
  xju::assert_equal(*x,true);
  ++x;
  xju::assert_equal(x, bit_end(v));
  xju::assert_equal(x-bit_begin(v), 16);
}

void test2() {

  std::vector<uint8_t> v{0xf3, 0x21};
  auto x(bit_begin(v));
  ++x;
  xju::assert_equal(*x, false);
  xju::assert_equal(x.containingByte(),(uint8_t)0x21);
  x.containingByte()=0x42;
  xju::assert_equal(x.containingByte(),(uint8_t)0x42);
  xju::assert_equal(*x, true);
  x.set(false);
  xju::assert_equal(*x, false);
  xju::assert_equal(x.containingByte(),(uint8_t)0x40);
  
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

