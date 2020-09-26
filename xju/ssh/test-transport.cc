// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/pipe.hh>
#include <signal.h>
#include <sstream>
#include <xju/MemOBuf.hh>
#include <xju/MemIBuf.hh>

namespace xju
{
namespace ssh
{
void test1() {
  // writeIdentificationString
  {
    xju::MemOBuf buffer(256);
    {
      xju::net::ostream s(buffer);
      transport::writeIdentificationString(
        s,
        transport::SoftwareVersion("fred's-ssh-2.6"),
        "slower than all the rest");
    }
    xju::assert_equal(std::string(buffer.data().first,buffer.data().second),
                      "SSH-2.0-fred's-ssh-2.6 slower than all the rest\r\n");
  }
  {
    xju::MemOBuf buffer(256);
    {
      xju::net::ostream s(buffer);
      transport::writeIdentificationString(
        s,
        transport::SoftwareVersion("fred's-ssh-2.6"),
        std::string());
    }
    xju::assert_equal(std::string(buffer.data().first,buffer.data().second),
                      "SSH-2.0-fred's-ssh-2.6\r\n");
  }
}
void test2()
{
  // identParser
  {
    std::string const x("SSH-2.0-fred's_ssh_2.6 slower than all the rest\r\n");
    auto const y=hcp_parser::parseString(x.begin(),x.end(),
                                         ssh::transport::identParser());
    xju::assert_equal(
      hcp_ast::reconstruct(
        hcp_ast::findOnlyChildOfType<ssh::transport::SSHVersionItem>(y)),
      "2.0");
    xju::assert_equal(
      hcp_ast::reconstruct(
        hcp_ast::findOnlyChildOfType<ssh::transport::SoftwareVersionItem>(y)),
      "fred's_ssh_2.6");
    xju::assert_equal(
      hcp_ast::reconstruct(
        hcp_ast::findOnlyChildOfType<ssh::transport::IdentCommentItem>(y)),
      "slower than all the rest");
  }
  {
    std::string const x("SSH-2.0-fred's_ssh_2.6 \r\n");
    auto const y=hcp_parser::parseString(x.begin(),x.end(),
                                         ssh::transport::identParser());
    xju::assert_equal(
      hcp_ast::reconstruct(
        hcp_ast::findOnlyChildOfType<ssh::transport::SSHVersionItem>(y)),
      "2.0");
    xju::assert_equal(
      hcp_ast::reconstruct(
        hcp_ast::findOnlyChildOfType<ssh::transport::SoftwareVersionItem>(y)),
      "fred's_ssh_2.6");
    xju::assert_equal(
      hcp_ast::reconstruct(
        hcp_ast::findOnlyChildOfType<ssh::transport::IdentCommentItem>(y)),
      "");
  }
  {
    std::string const x("SSH-2.0- \r\n");
    auto const y=hcp_parser::parseString(x.begin(),x.end(),
                                         ssh::transport::identParser());
    xju::assert_equal(
      hcp_ast::reconstruct(
        hcp_ast::findOnlyChildOfType<ssh::transport::SSHVersionItem>(y)),
      "2.0");
    xju::assert_equal(
      hcp_ast::reconstruct(
        hcp_ast::findOnlyChildOfType<ssh::transport::SoftwareVersionItem>(y)),
      "");
    xju::assert_equal(
      hcp_ast::reconstruct(
        hcp_ast::findOnlyChildOfType<ssh::transport::IdentCommentItem>(y)),
      "");
  }
  {
    std::string const x("SSH-1.99-\r\n");
    auto const y=hcp_parser::parseString(x.begin(),x.end(),
                                         ssh::transport::identParser());
    xju::assert_equal(
      hcp_ast::reconstruct(
        hcp_ast::findOnlyChildOfType<ssh::transport::SSHVersionItem>(y)),
      "1.99");
    xju::assert_equal(
      hcp_ast::reconstruct(
        hcp_ast::findOnlyChildOfType<ssh::transport::SoftwareVersionItem>(y)),
      "");
    xju::assert_equal(
      hcp_ast::reconstruct(
        hcp_ast::findOnlyChildOfType<ssh::transport::IdentCommentItem>(y)),
      "");
  }
  try
  {
    std::string const x("SSH-2.0\r\n");
    auto const y=hcp_parser::parseString(x.begin(),x.end(),
                                         ssh::transport::identParser());
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse SSH version at line 1 column 5 because\nfailed to parse one of chars ' '..'\\0177' at line 1 column 8 because\nline 1 column 8: '\\r' is not one of chars ' '..'\\0177'.");
  }
  try
  {
    std::string const x("SSH-2.0-f\t1\r\n");
    auto const y=hcp_parser::parseString(x.begin(),x.end(),
                                         ssh::transport::identParser());
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse software version at line 1 column 9 because\nfailed to parse one of chars ' '..'\\0177' at line 1 column 10 because\nline 1 column 10: '\\t' is not one of chars ' '..'\\0177'.");
  }
}
void test3() {
  // readIdentificationString
  {
    xju::MemOBuf b(256);
    {
      xju::net::ostream s(b);
      transport::writeIdentificationString(
        s,
        transport::SoftwareVersion("fred's_ssh_2.6"),
        "slower than all the rest");
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
      std::string("slower than all the rest"));
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
        "slower than all the rest");
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
      std::string("slower than all the rest"));
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

using namespace xju::ssh;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  test3(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

