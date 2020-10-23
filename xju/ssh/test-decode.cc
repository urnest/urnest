// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/decode.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/MemIBuf.hh>

namespace xju
{
namespace ssh
{

void test1() {
  xju::MemIBuf b({
    5,
    0,0,0,4,'f','r','e','d',
    0,0,0,3,'s','a','l',
    0,0,0,8,'a','l','l',',','n','o','n','e'});
  xju::net::istream s(b);
  xju::assert_equal(decode<uint8_t>(s),5);
  xju::assert_equal(decode<std::string>(s),"fred");
  typedef xju::Tagged<std::string,uint8_t> S;
  xju::assert_equal(decode<S>(s),S("sal"));
  xju::assert_equal(decode<std::vector<S> >(s),
                    { S("all"),S("none")});
}

}
}

using namespace xju::ssh;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

