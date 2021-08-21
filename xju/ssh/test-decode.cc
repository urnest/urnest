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
#include <xju/MemOBuf.hh>
#include <xju/net/ostream.hh>
#include <xju/ssh/encode.hh>
#include <xju/ssh/transport/messages/ServiceAccept.hh>

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

void test2() {
  {
    xju::ssh::transport::messages::Disconnect const x(
      27,
      "shrug",
      xju::LanguageName("SP"));
    xju::MemOBuf b(256,1024);
    {
      xju::net::ostream s(b);
      encode(s,x);
    }
    xju::MemIBuf c(b.data().first,b.data().second);
    xju::net::istream s(c);
    xju::assert_equal(decode<uint8_t>(s),(uint8_t)xju::ssh::transport::MSG::DISCONNECT);
    xju::assert_equal(decode<xju::ssh::transport::messages::Disconnect>(s),x);
  }
  {
    xju::ssh::transport::messages::ServiceRequest const x(
      xju::ssh::misc::ServiceName("userauth"));
    xju::MemOBuf b(256,1024);
    {
      xju::net::ostream s(b);
      encode(s,x);
    }
    xju::MemIBuf c(b.data().first,b.data().second);
    xju::net::istream s(c);
    xju::assert_equal(decode<uint8_t>(s),(uint8_t)xju::ssh::transport::MSG::SERVICE_REQUEST);
    xju::assert_equal(decode<xju::ssh::transport::messages::ServiceRequest>(s),x);
  }
  {
    xju::ssh::transport::messages::ServiceAccept const x(
      xju::ssh::misc::ServiceName("userauth"));
    xju::MemOBuf b(256,1024);
    {
      xju::net::ostream s(b);
      encode(s,x);
    }
    xju::MemIBuf c(b.data().first,b.data().second);
    xju::net::istream s(c);
    xju::assert_equal(decode<uint8_t>(s),(uint8_t)xju::ssh::transport::MSG::SERVICE_ACCEPT);
    xju::assert_equal(decode<xju::ssh::transport::messages::ServiceAccept>(s),x);
  }
  
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

