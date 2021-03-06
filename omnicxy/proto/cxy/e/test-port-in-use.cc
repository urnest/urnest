// Copyright (c) 2014 Trevor Taylor
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
#include <xju/format.hh>
#include <cxy/ORB.hh>
#include <omnicxy/proto/cxy/e/E.hh>
#include <sstream>
#include <omniORB4/CORBA.h>

std::ostringstream messages;
void log(char const* message)
{
  messages << std::string(message) << std::endl;
}

void test1() {
  // assume sshd running (port 22)
  std::string const orbEndPoint="giop:tcp::22";
  try {
    omniORB::setLogFunction(log);
    cxy::ORB<E> orb2(orbEndPoint);
    std::cerr << messages.str() << std::endl;
    xju::assert_abort();
  }
  catch(E& e) {
    std::string const m(messages.str());
    xju::assert_equal(readableRepr(e), "endPoint \"giop:tcp::22\" requests a port that is in use (or endPoint is malformed or invalid).");
  }
}

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

