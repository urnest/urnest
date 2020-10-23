// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/decodePacket.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/MemIBuf.hh>
#include <xju/Exception.hh>

namespace xju
{
namespace ssh
{
namespace transport
{

void test1() {
  {
    xju::MemIBuf b({0x0,0x0,0x0,12,
                    0,
                    0,1,2,3,4,5,6,7,8,9,10});
    xju::net::istream from(b);
    auto const y(decodePacket(from,11));
    xju::assert_equal(y.first,
                      {0,1,2,3,4,5,6,7,8,9,10});
    xju::assert_equal(y.second,Padding{});
  }
  {
    xju::MemIBuf b({0x0,0x0,0x0,12,
                    3,
                    0,1,2,3,4,5,6,7,0xa,0xb,0xc});
    xju::net::istream from(b);
    auto const y(decodePacket(from,8));
    xju::assert_equal(y.first,
                      {0,1,2,3,4,5,6,7});
    xju::assert_equal(y.second,Padding{0xa,0xb,0xc});
  }
  {
    xju::MemIBuf b({0x0,0x0,0x0,12,
                    0,
                    0,1,2,3,4,5,6,7,8,9,10,
                    0x0,0x0,0x0,12,
                    3,
                    0,1,2,3,4,5,6,7,0xa,0xb,0xc});
    xju::net::istream from(b);
    {
      auto const y(decodePacket(from,11));
      xju::assert_equal(y.first,
                        {0,1,2,3,4,5,6,7,8,9,10});
      xju::assert_equal(y.second,Padding{});
    }
    {
      auto const y(decodePacket(from,8));
      xju::assert_equal(y.first,
                        {0,1,2,3,4,5,6,7});
      xju::assert_equal(y.second,Padding{0xa,0xb,0xc});
    }
  }
  {
    xju::MemIBuf b({0x0,0x0,0x0,12,
                    11,
                    0,1,2,3,4,5,6,7,0xa,0xb,0xc});
    xju::net::istream from(b);
    auto const y(decodePacket(from,0));
    xju::assert_equal(y.first,
                      {});
    xju::assert_equal(y.second,Padding{0,1,2,3,4,5,6,7,0xa,0xb,0xc});
  }
  {
    xju::MemIBuf b({0x0,0x0,0x0,12,
                    12,
                    0,1,2,3,4,5,6,7,0xa,0xb,0xc});
    xju::net::istream from(b);
    try{
      auto const y(decodePacket(from,1024));
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      xju::assert_equal(readableRepr(e),"Failed to decode SSH 1024-max-bytes packet excluding MAC from stream because\nSSH padding length 12 overruns packet with length 12.");
    }
  }
  {
    xju::MemIBuf b({0x0,0x0,0x0,0,
                    12,
                    0,1,2,3,4,5,6,7,0xa,0xb,0xc});
    xju::net::istream from(b);
    try{
      auto const y(decodePacket(from,1024));
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      xju::assert_equal(readableRepr(e),"Failed to decode SSH 1024-max-bytes packet excluding MAC from stream because\nSSH packet length 0 is not valid.");
    }
  }
}

}
}
}

using namespace xju::ssh::transport;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

