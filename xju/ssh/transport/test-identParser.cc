// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/identParser.hh>

#include <iostream>
#include <xju/assert.hh>
#include <hcp/parser.hh>
#include <hcp/ast.hh>

namespace xju
{
namespace ssh
{
namespace transport
{

void test1() {
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
      hcp_ast::findChildrenOfType<ssh::transport::IdentCommentItem>(y).size(),
      0U);
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

