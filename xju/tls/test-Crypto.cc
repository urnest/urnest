// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/tls/Session.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace tls
{

void test1() {
  //REVISIT
  //connect timeout

  //connect loopback
  //send nx quick brown foxes piecemeal with random chunks and random
  //delay, verify read correctly (read piecemeal too with random delay)

  //close server first detect at client
}
void test2() {
  //REVISIT
  //connect verify peer cert - good
  //connect verify peer cert - bad

  //close client first detect at server
}

}
}

using namespace xju::tls;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

