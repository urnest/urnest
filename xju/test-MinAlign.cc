// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/MinAlign.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

void test1() {
  xju::assert_equal(sizeof(MinAlign<uint8_t>::T),1U);
  xju::assert_equal(sizeof(MinAlign<uint16_t>::T),2U);
  xju::assert_equal(sizeof(MinAlign<uint32_t>::T),4U);
  xju::assert_equal(sizeof(MinAlign<uint64_t>::T),8U);
}

struct A
{
  uint32_t x;
  char y;
};
void test2() {
  xju::assert_equal(sizeof(MinAlign<A>::T),4U);
}


}

using namespace xju;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

