// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ip/UDPSocket.hh>

#include <iostream>
#include <xju/assert.hh>
#include <string>
#include "xju/ip/v4/getHostAddresses.hh"
#include <chrono>
#include <vector>
#include <utility>
#include "xju/next.hh"
#include "xju/format.hh"

namespace xju
{
namespace ip
{

void test1() {
  UDPSocket s1;
  auto const p1(s1.port());
  try {
    UDPSocket s2(p1);
    xju::assert_never_reached();
  }
  catch(xju::ip::PortInUse const& e){
    xju::assert_equal(readableRepr(e),"Failed to create UDP socket bound to port "+xju::format::str(p1)+" because\nbind system call failed, errno = "+xju::format::str(EADDRINUSE)+".");
  }
  for(xju::ip::Port p2{xju::next(p1)};p2!=p1+xju::ip::Port(1000);++p2){
    try{
      xju::ip::v4::Address localhost(
        xju::ip::v4::getHostAddresses(xju::HostName("localhost"))[0]);
      
      UDPSocket s2(p2);

      std::string const fred("fred");

      // good receive
      s1.sendTo(localhost,p2,
                fred.c_str(),
                fred.size(),
                std::chrono::steady_clock::now());
      std::vector<char> r(4,0);
      auto const rr(
        s2.receive(r.data(),r.size(),
                   std::chrono::steady_clock::now()+std::chrono::seconds(1)));
      xju::assert_equal(rr,std::make_pair(std::make_pair(localhost,p1),fred.size()));
      xju::assert_equal(std::string(r.begin(),r.end()),fred);

      // timeout
      auto const deadline(std::chrono::steady_clock::now());
      try {
        std::vector<char> r(4,0);
        s2.receive(r.data(),r.size(),deadline);
        xju::assert_never_reached();
      }
      catch(xju::DeadlineReached const& e){
        xju::assert_startswith(readableRepr(e),"Failed to receive udp datagram (up to 4 bytes) on port "+xju::format::str(p2)+" within ");
        xju::assert_endswith(readableRepr(e),std::string(" because\ndeadline reached before socket readable."));
      }

      // buffer too small
      s1.sendTo(localhost,p2,
                fred.c_str(),
                fred.size(),
                deadline);
      auto const deadline2(deadline+std::chrono::seconds(2));
      try {
        std::vector<char> r(0,0);
        s2.receive(r.data(),r.size(),deadline2);
      }
      catch(xju::Exception const& e){
        xju::assert_startswith(readableRepr(e),"Failed to receive udp datagram (up to 0 bytes) on port "+xju::format::str(p2)+" within ");
        xju::assert_endswith(readableRepr(e),std::string(" because\nbuffer too small."));
      }
     
      return;
    }
    catch(xju::ip::PortInUse&){
    }
  }
  xju::assert_never_reached();//1000 ports in use?
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

