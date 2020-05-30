// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ByteBuffer.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

void test1() {
  ByteBuffer buffer(32);
  xju::assert_equal(buffer.size(),0U);
  auto const extent(buffer.contigSpace());
  xju::assert_equal(extent.second,32U);
  xju::assert_equal(buffer.contigData().second,0U);
  buffer.appended(10U);
  xju::assert_equal(buffer.size(),10U);
  xju::assert_equal(buffer.contigData(),std::make_pair(extent.first,10UL));
  buffer.appended(22U);
  xju::assert_equal(buffer.contigData(),std::make_pair(extent.first,32UL));
  xju::assert_equal(buffer.size(),32U);
  xju::assert_equal(buffer.contigSpace().second,0U);
  buffer.consumed(10U);
  xju::assert_equal(buffer.contigData(),std::make_pair(extent.first+10U,22UL));
  xju::assert_equal(buffer.size(),22U);
  xju::assert_equal(buffer.contigSpace().second,10U);
  buffer.appended(4U);
  xju::assert_equal(buffer.contigData(),std::make_pair(extent.first+10U,22UL));
  xju::assert_equal(buffer.size(),26U);
  xju::assert_equal(buffer.contigSpace().second,6U);
  xju::assert_equal(buffer.contigSpace().first,extent.first+4);
  buffer.consumed(21U);
  xju::assert_equal(buffer.contigData(),std::make_pair(extent.first+31U,1UL));
  xju::assert_equal(buffer.size(),5U);
  xju::assert_equal(buffer.contigSpace().second,27U);
  xju::assert_equal(buffer.contigSpace().first,extent.first+4);
  buffer.consumed(1U);
  xju::assert_equal(buffer.contigData(),std::make_pair(extent.first,4UL));
  xju::assert_equal(buffer.size(),4U);
  xju::assert_equal(buffer.contigSpace().second,28U);
  xju::assert_equal(buffer.contigSpace().first,extent.first+4);
  buffer.consumed(4U);
  xju::assert_equal(buffer.contigData(),std::make_pair(extent.first+0U,0UL));
  xju::assert_equal(buffer.size(),0U);
  xju::assert_equal(buffer.contigSpace().second,32U);
  xju::assert_equal(buffer.contigSpace().first,extent.first);
  buffer.appended(31U);
  xju::assert_equal(buffer.contigData(),std::make_pair(extent.first,31UL));
  xju::assert_equal(buffer.size(),31U);
  xju::assert_equal(buffer.contigSpace().second,1U);
  xju::assert_equal(buffer.contigSpace().first,extent.first+31UL);
  buffer.consumed(30U);
  xju::assert_equal(buffer.contigData(),std::make_pair(extent.first+30U,1UL));
  xju::assert_equal(buffer.size(),1U);
  xju::assert_equal(buffer.contigSpace().second,1U);
  xju::assert_equal(buffer.contigSpace().first,extent.first+31UL);
  buffer.appended(3U);
  xju::assert_equal(buffer.contigData(),std::make_pair(extent.first+30U,2UL));
  xju::assert_equal(buffer.size(),4UL);
  xju::assert_equal(buffer.contigSpace().second,28UL);
  xju::assert_equal(buffer.contigSpace().first,extent.first+2UL);
  buffer.consumed(3U);
  xju::assert_equal(buffer.contigData(),std::make_pair(extent.first+1U,1UL));
  xju::assert_equal(buffer.size(),1U);
  xju::assert_equal(buffer.contigSpace().second,30UL);
  xju::assert_equal(buffer.contigSpace().first,extent.first+2UL);
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

