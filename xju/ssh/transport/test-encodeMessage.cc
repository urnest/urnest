// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/encodeMessage.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/MemOBuf.hh>

namespace xju
{
namespace ssh
{
namespace transport
{

void test1() {

  {
    xju::MemOBuf b(256);
    {
      xju::net::ostream to(b);
      encodeMessage({0,1,2,3,4,5,6,7},
                    to,
                    7,
                    0.0);
    }
    std::vector<uint8_t> const x(b.data().first,b.data().second);
    xju::assert_equal(x.size(),16);
    xju::assert_equal(std::vector<uint8_t>(x.begin(),x.begin()+13),
                      {0x0,0x0,0x0,12,
                       3,
                       0,1,2,3,4,5,6,7});
  }
  {
    xju::MemOBuf b(256);
    {
      xju::net::ostream to(b);
      encodeMessage({0,1,2,3,4,5,6,7,8,9,10},
                    to,
                    7,
                    0.0);
    }
    std::vector<uint8_t> const x(b.data().first,b.data().second);
    xju::assert_equal(x.size(),16);
    xju::assert_equal(std::vector<uint8_t>(x.begin(),x.begin()+16),
                      {0x0,0x0,0x0,12,
                       0,
                       0,1,2,3,4,5,6,7,8,9,10});
  }
  {
    xju::MemOBuf b(256);
    {
      xju::net::ostream to(b);
      encodeMessage({0,1,2,3,4,5,6,7,8,9,10},
                    to,
                    7,
                    0.5);
    }
    std::vector<uint8_t> const x(b.data().first,b.data().second);
    xju::assert_equal(x.size(),136);
    xju::assert_equal(std::vector<uint8_t>(x.begin(),x.begin()+16),
                      {0x0,0x0,0x0,132,
                       120,
                       0,1,2,3,4,5,6,7,8,9,10});
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

