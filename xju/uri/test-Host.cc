// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/uri/Host.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/format.hh>
#include <xju/HostName.hh>

namespace xju
{
namespace uri
{

void test1() {
  Host const h(xju::HostName("my(big:wide!) host;name.com"));
  xju::assert_equal(xju::format::str(h), "my(big%3awide!)%20host;name.com");
}

}
}

using namespace xju::uri;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

