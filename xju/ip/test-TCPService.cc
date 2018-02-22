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

namespace xju
{
namespace ip
{

void test1() {
  TCPService s(TCPService::Backlog(1),true);
  try{
    TCPService s2(s.port(),
                  TCPService::Backlog(1),
                  false);
    xju::assert_never_reached();
  }
  catch(xju::ip::PortInUse const& e){
    xju::assert_equal(readableRepr(e),"");
  }
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

