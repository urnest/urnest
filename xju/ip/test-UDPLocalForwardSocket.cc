// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ip/UDPLocalForwardSocket.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/ip/UDPSocket.hh>
#include <xju/steadyNow.hh>
#include <chrono>

namespace xju
{
namespace ip
{

void test1() {
  UDPSocket t;
  UDPLocalForwardSocket x;
  x.forwardToLocalPort(
    t.port(),
    "fred",4,
    xju::ip::v4::Address("127.0.0.2"),
    xju::ip::Port(22),
    xju::steadyNow()+std::chrono::seconds(1));
  std::vector<uint8_t> buffer;
  buffer.resize(5);
  auto const y{t.receive(buffer.data(),5,
                         xju::steadyNow()+std::chrono::seconds(1))};
  buffer.resize(y.second);
  xju::assert_equal(buffer,std::vector<uint8_t>{'f','r','e','d'});
  xju::assert_equal(y.first,UDPSocket::Sender(
                      xju::ip::v4::Address("127.0.0.2"),
                      xju::ip::Port(22)));
}

}
}

using namespace xju::ip;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

