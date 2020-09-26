// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/net/ostream.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/MemOBuf.hh>

namespace xju
{
namespace net
{

void test1() {
  xju::MemOBuf buf(5,16);
  {
    xju::net::ostream s(buf);
    s.put8(0x23)
      .put16(0x2324)
      .put32(0x32333435)
      .put64(0x6061626364656667);
  }
  xju::assert_equal(std::vector<uint8_t>(
                      buf.data().first,
                      buf.data().second),
                    std::vector<uint8_t>{
    0x23,
      0x23,0x24,
      0x32,0x33,0x34,0x35,
      0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67});
  try{
    xju::net::ostream s(buf);
    s.put16(0x0000);
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"no space.");
  }
}

void test2() {
  xju::MemOBuf buf(5,15);
  std::vector<uint8_t> const y{
    0x23,
    0x23,0x24,
    0x32,0x33,0x34,0x35,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67};
  {
    xju::net::ostream s(buf);
    s.put(y.begin(),y.end());
  }
  xju::assert_equal(std::vector<uint8_t>(
                      buf.data().first,
                      buf.data().second),
                    y);
}

}
}

using namespace xju::net;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

