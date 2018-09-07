// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <iostream>
#include <xju/assert.hh>

#include <xju/Exception.hh>
#include <iostream>
#include <string>
#include <cxy/ORB.hh>
#include "xju/socket.hh"
#include "xju/unistd.hh"
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <xju/format.hh>

void test1() {
  for(uint16_t port=9012; port != 10012; ++port) {
    int const socket(xju::syscall(xju::socket,XJU_TRACED)(
                       AF_INET,SOCK_STREAM,0));
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=::htons(port);
    addr.sin_addr.s_addr=INADDR_ANY;
    try {
      xju::syscall(xju::bind,XJU_TRACED)(
        socket,(struct sockaddr*)&addr,sizeof(addr));
      xju::syscall(xju::listen,XJU_TRACED)(socket,1);
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      try {
        cxy::ORB<cxy::Exception> orb(orbEndPoint);
        xju::assert_never_reached(); // should never get here
      }
      catch(cxy::PortInUse const& e) {
        std::ostringstream expect;
        expect << "EndPoint \""<<orbEndPoint<<"\" requests a port that is in use (or endPoint is malformed or invalid).";
        xju::assert_equal(readableRepr(e),expect.str());
        xju::syscall(xju::close,XJU_TRACED)(socket);
        return;
      }
    }
    catch(xju::SyscallFailed const& e) {
      xju::assert_equal(e._errno,EADDRINUSE);
    }
  }
  xju::assert_never_reached(); // surely a free port in there somewhere
}

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

