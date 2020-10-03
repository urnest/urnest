// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/writeIdentificationString.hh>

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
  // writeIdentificationString
  {
    xju::MemOBuf buffer(256);
    {
      xju::net::ostream s(buffer);
      transport::writeIdentificationString(
        s,
        transport::SoftwareVersion("fred's-ssh-2.6"),
        std::string("slower than all the rest"));
    }
    xju::assert_equal(std::string(buffer.data().first,buffer.data().second),
                      "SSH-2.0-fred's-ssh-2.6 slower than all the rest\r\n");
  }
  {
    xju::MemOBuf buffer(256);
    {
      xju::net::ostream s(buffer);
      transport::writeIdentificationString(
        s,
        transport::SoftwareVersion("fred's-ssh-2.6"),
        xju::Optional<std::string>());
    }
    xju::assert_equal(std::string(buffer.data().first,buffer.data().second),
                      "SSH-2.0-fred's-ssh-2.6\r\n");
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

