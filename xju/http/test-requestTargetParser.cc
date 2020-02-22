// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/http/requestTargetParser.hh>

#include <iostream>
#include <xju/assert.hh>
#include <hcp/ast.hh>
#include <xju/uri/Path.hh>
#include <xju/uri/Query.hh>

namespace xju
{
namespace http
{

void test1() {
  //origin-form
  {
    const std::string s{"/a/x.txt?name=jock%20allen"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         requestTargetParser())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).get(),
                      RequestTarget(
                        {xju::uri::Segment(""),
                         xju::uri::Segment("a"),
                         xju::uri::Segment("x.txt")},
                        xju::uri::Query("name=jock allen")));
  }
  {
    const std::string s{"/"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         requestTargetParser())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).get(),
                      RequestTarget(
                        {xju::uri::Segment(""),xju::uri::Segment("")},
                        xju::uri::Query("")));
  }
  {
    const std::string s{"/a/b/"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         requestTargetParser())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).get(),
                      RequestTarget(
                        {xju::uri::Segment(""),
                         xju::uri::Segment("a"),
                         xju::uri::Segment("b"),
                         xju::uri::Segment("")},
                        xju::uri::Query("")));
  }
  {
    const std::string s{"/a/b/"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         requestTargetParser())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).get(),
                      RequestTarget(
                        {xju::uri::Segment(""),
                         xju::uri::Segment("a"),
                         xju::uri::Segment("b"),
                         xju::uri::Segment("")},
                        xju::uri::Query("")));
  }
  {
    const std::string s{"/a/b/.."};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         requestTargetParser())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).get(),
                      RequestTarget(
                        {xju::uri::Segment(""),
                         xju::uri::Segment("a"),
                         xju::uri::Segment("b"),
                         xju::uri::Segment("..")},
                        xju::uri::Query("")));
  }
}

void test2() {
  //absolute-form
  {
    const std::string s{"sip://a.com/b/../x.txt?name=jock%20allen"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         requestTargetParser())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).get(),
                      RequestTarget(
                        xju::uri::Scheme("sip"),
                        xju::uri::Authority(
                          xju::uri::Host(xju::HostName("a.com")),
                          xju::Optional<xju::ip::Port>(),
                          xju::Optional<xju::uri::UserInfo>()),
                        {xju::uri::Segment(""),
                         xju::uri::Segment("b"),
                         xju::uri::Segment(".."),
                         xju::uri::Segment("x.txt")},
                        xju::uri::Query("name=jock allen")));
  }
  {
    const std::string s{"sip://a.com/b/../x.txt"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         requestTargetParser())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).get(),
                      RequestTarget(
                        xju::uri::Scheme("sip"),
                        xju::uri::Authority(
                          xju::uri::Host(xju::HostName("a.com")),
                          xju::Optional<xju::ip::Port>(),
                          xju::Optional<xju::uri::UserInfo>()),
                        {xju::uri::Segment(""),
                         xju::uri::Segment("b"),
                         xju::uri::Segment(".."),
                         xju::uri::Segment("x.txt")},
                        xju::uri::Query("")));
  }
  {
    const std::string s{"sip://a.com"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         requestTargetParser())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).get(),
                      RequestTarget(
                        xju::uri::Scheme("sip"),
                        xju::uri::Authority(
                          xju::uri::Host(xju::HostName("a.com")),
                          xju::Optional<xju::ip::Port>(),
                          xju::Optional<xju::uri::UserInfo>()),
                        xju::uri::Path(),
                        xju::uri::Query("")));
  }
}
void test3()
{
  //authority-form
  {
    const std::string s{"a.com"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         requestTargetParser())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(
      hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).get(),
      RequestTarget(
        xju::uri::Authority(
          xju::uri::Host(xju::HostName("a.com")),
          xju::Optional<xju::ip::Port>(),
          xju::Optional<xju::uri::UserInfo>())));
  }
  if(false) // a.com:99 is a valid URI with scheme a.com and host 99 :-(
  {
    const std::string s{"a.com:99"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         requestTargetParser())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(
      hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).get(),
      RequestTarget(
        xju::uri::Authority(
          xju::uri::Host(xju::HostName("a.com")),
          xju::ip::Port(99),
          xju::Optional<xju::uri::UserInfo>())));
  }
  
}
void test4()
{
  //asterisk-form

  {
    const std::string s{"*"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         requestTargetParser())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).get(),
                      RequestTarget());
  }
  
}

}
}

using namespace xju::http;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  test3(), ++n;
  test4(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

