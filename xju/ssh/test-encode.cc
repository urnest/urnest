// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/encode.hh>

#include <iostream>
#include <xju/assert.hh>
#include <vector>
#include <xju/MemOBuf.hh>
#include <xju/LanguageName.hh>

namespace xju
{
namespace ssh
{

void test1() {
  xju::MemOBuf b(256,1024);
  {
    xju::net::ostream s(b);
    encode(s,(uint8_t)5);
    encode(s,std::string("fred"));
    encode(s,xju::Tagged<std::string,uint8_t>("sal"));
    typedef xju::Tagged<std::string,uint8_t> S;
    encode(s,std::vector<S>{S("all"),S("none")});
  }
  xju::assert_equal(std::vector<uint8_t>(b.data().first,b.data().second),
                    {5,
                     0,0,0,4,'f','r','e','d',
                     0,0,0,3,'s','a','l',
                     0,0,0,8,'a','l','l',',','n','o','n','e'});
}

void test2() {
  xju::MemOBuf b(256,1024);
  {
    xju::net::ostream s(b);
    encode(s,xju::ssh::transport::messages::Disconnect(
             27,
             "shrug",
             xju::LanguageName("SP")));
  }
  xju::assert_equal(std::vector<uint8_t>(b.data().first,b.data().second),
                    {1,
                     0,0,0,27,
                     0,0,0,5,'s','h','r','u','g',
                     0,0,0,2,'S','P'});
}

}
}

using namespace xju::ssh;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

