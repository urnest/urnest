// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/userauth/authenticatees/RSAPublicKeyAuthenticatee.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace ssh
{
namespace userauth
{
namespace authenticatees
{

void test1() {
  @@@;
  // good
  // sign fails
  // send message fails
  // receive fails
  // response is FAILURE, can't continue
  // response is FAILURE, can continue
  // ignore, debug + success
  // unexpected message transport::SERVICE_ACCEPT
}

}
}
}
}

using namespace xju::ssh::userauth::authenticatees;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

