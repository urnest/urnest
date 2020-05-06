// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ip/UDPDeliveryFailureNoticeQueue.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/HostName.hh>

namespace xju
{
namespace ip
{

void test1() {
  // verify can send and receive without errors
}
void test2(xju::HostName const& unreachableHost)
{
  // verify error sets POLLERR and does not try to collect pending
  // inbound messages; verify
}

void test3(xju::HostName const& arpFailureHost)
{
  // verify non-existent local network host i.e. arp failure
  // generates local delivery failure
}

}
}

using namespace xju::ip;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  if(argc==2){
    test2(xju::HostName(argv[1])), ++n;
  }
  if(argc==3){
    test3(xju::HostName(argv[2])), ++n;
  }
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

