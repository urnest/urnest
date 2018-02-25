// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ip/TCPService.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/format.hh>
#include <xju/now.hh>
#include <xju/Thread.hh>
#include <xju/ip/TCPSocket.hh>
#include <xju/ip/v4/getHostAddresses.hh>
#include <xju/getHostName.hh>

namespace xju
{
namespace ip
{

void test1() {
  TCPService s(TCPService::Backlog(1),true);
  auto const deadline(xju::now()+std::chrono::seconds(5));
  xju::Thread t([&](){
      TCPSocket c(
        {xju::ip::v4::getHostAddresses(xju::getHostName())[0],s.port()},
        deadline);
      char a;
      c.read(&a,sizeof(a),deadline);
      ++a;
      c.write(&a,sizeof(a),deadline);
    });
  TCPSocket server(s,deadline);
  char b('x');
  server.write(&b,sizeof(b),deadline);
  server.read(&b,sizeof(b),deadline);
  xju::assert_equal(b,'y');
}

void test2() {
  TCPService s(TCPService::Backlog(1),true);
  try{
    TCPService s2(s.port(),
                  TCPService::Backlog(1),
                  false);
    xju::assert_never_reached();
  }
  catch(xju::ip::PortInUse const& e){
    xju::assert_equal(readableRepr(e),"Failed to create TCP listener socket listening on local port "+xju::format::str(s.port())+", not allowing local port re-use,  closing socket on exec because\nbind system call failed, errno = 98.");

  }
}

}
}

using namespace xju::ip;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

