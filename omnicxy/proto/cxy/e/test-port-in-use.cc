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
#include "E.hh"

void test1() {
  // assume sshd running (port 22)
  std::string const orbEndPoint="giop:tcp::22";
  try {
    cxy::ORB<E> orb2(orbEndPoint);
    xju::assert_abort();
  }
  catch(E& e) {
    xju::assert_equal(readableRepr(e), "Failed to get omniORB insPOA at endpoint giop:tcp::22 because\nN5CORBA10INITIALIZEE minor 1096024077.");
  }
}

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

