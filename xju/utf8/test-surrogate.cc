// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/utf8/surrogate.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace utf8
{
namespace surrogate
{

void test1() {
  xju::assert_equal(isSurrogateHigh(0xd7ff),false);
  xju::assert_equal(isSurrogateHigh(0xd800),true);
  xju::assert_equal(isSurrogateHigh(0xdbff),true);
  xju::assert_equal(isSurrogateHigh(0xdc00),false);
}

void test2() {
  xju::assert_equal(decodeSurrogatePair(0xd801,0xdc37),0x10437);
  xju::assert_equal(decodeSurrogatePair(0xd852,0xdf62),0x24b62);
}

void test3() {
  try{
    decodeSurrogatePair(0xD7ff,0xDc00);
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to decode 0xd7ff and 0xdc00 assuming they are a UTF16 surrogate pair (high,low) because
0xd7ff is not in range 0xD800..0xDBFF.
)--");
  }
  try{
    decodeSurrogatePair(0xD800,0xDb00);
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to decode 0xd800 and 0xdb00 assuming they are a UTF16 surrogate pair (high,low) because
0xdb00 is not in range 0xDC00..0xDFFF.
)--");
  }
}

}
}
}

using namespace xju::utf8::surrogate;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  test3(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

