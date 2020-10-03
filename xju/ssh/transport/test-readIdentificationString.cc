// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/readIdentificationString.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/ssh/transport/writeIdentificationString.hh>
#include <xju/MemOBuf.hh>
#include <xju/MemIBuf.hh>
#include <xju/net/ostream.hh>

namespace xju
{
namespace ssh
{
namespace transport
{

void test1() {
  // readIdentificationString
  {
    xju::MemOBuf b(256);
    {
      xju::net::ostream s(b);
      transport::writeIdentificationString(
        s,
        transport::SoftwareVersion("fred's_ssh_2.6"),
        std::string("slower than all the rest"));
    }
    xju::MemIBuf ib(std::vector<uint8_t>(b.data().first,b.data().second));
    xju::net::istream si(ib);
    auto const y(transport::readIdentificationString(si));
    xju::assert_equal(
      std::get<0>(y),
      std::vector<std::string>());
    xju::assert_equal(
      std::get<1>(y),
      transport::SSHVersion("2.0"));
    xju::assert_equal(
      std::get<2>(y),
      transport::SoftwareVersion("fred's_ssh_2.6"));
    xju::assert_equal(
      std::get<3>(y),
      xju::Optional<std::string>("slower than all the rest"));
  }
  {
    xju::MemOBuf b(256);
    {
      xju::net::ostream s(b);
      s.put("preamble 1\r\n")
        .put("preamble 2\r\n");
      transport::writeIdentificationString(
        s,
        transport::SoftwareVersion("fred's_ssh_2.6"),
        xju::Optional<std::string>());
    }
    xju::MemIBuf ib(std::vector<uint8_t>(b.data().first,b.data().second));
    xju::net::istream si(ib);

    auto const y(transport::readIdentificationString(si));
    xju::assert_equal(
      std::get<0>(y),
      std::vector<std::string>({std::string("preamble 1"),
                                std::string("preamble 2")}));
    xju::assert_equal(
      std::get<1>(y),
      transport::SSHVersion("2.0"));
    xju::assert_equal(
      std::get<2>(y),
      transport::SoftwareVersion("fred's_ssh_2.6"));
    xju::assert_equal(
      std::get<3>(y),
      xju::Optional<std::string>());
  }
  {
    xju::MemOBuf b(256);
    {
      xju::net::ostream s(b);
      s.put("preamble 1\r\n")
        .put("preamble 2\r\n")
        .put("SSH-2.0-incomplete");
    }
    xju::MemIBuf ib(std::vector<uint8_t>(b.data().first,b.data().second));
    xju::net::istream si(ib);

    try{
      auto const y(transport::readIdentificationString(si));
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e){
      xju::assert_equal(readableRepr(e),"Failed to read SSH identification string from istream having read 2 preamble lines: \"preamble 1\", \"preamble 2\" because\nfailed to read up to and including \"\\r\\n\" expected within the next 237 characters having read \"SSH-2.0-incomplete\" because\nend of input.");
    }
  }

}

}
}
}

using namespace xju::ssh::transport;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

