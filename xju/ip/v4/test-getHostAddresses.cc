// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ip/v4/getHostAddresses.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace ip
{
namespace v4
{

void test1() {
  xju::assert_equal(getHostAddresses(xju::HostName("localhost")),
                    {Address(0x7f000001)});
}

}
}
}

using namespace xju::ip::v4;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

