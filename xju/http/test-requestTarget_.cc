// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/http/requestTarget_.hh>

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
                                         requestTarget_())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).requestTarget_,
                      RequestTarget(
                        xju::path::AbsFile(
                          std::make_pair(
                            xju::path::AbsolutePath(
                              {xju::path::DirName("a")}),
                            xju::path::FileName("x.txt"))),
                        xju::uri::Query("name=jock allen")));
  }
  {
    const std::string s{"/"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         requestTarget_())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).requestTarget_,
                      RequestTarget(
                        xju::path::AbsFile(
                          std::make_pair(
                            xju::path::AbsolutePath(
                              std::vector<xju::path::DirName>()),
                            xju::path::FileName(""))),
                        xju::uri::Query("")));
  }
  {
    const std::string s{"/a/b/"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         requestTarget_())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).requestTarget_,
                      RequestTarget(
                        xju::path::AbsFile(
                          std::make_pair(
                            xju::path::AbsolutePath(
                              {xju::path::DirName("a")}),
                            xju::path::FileName("b"))),
                        xju::uri::Query("")));
  }
  {
    const std::string s{"/a/b/"};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         requestTarget_())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).requestTarget_,
                      RequestTarget(
                        xju::path::AbsFile(
                          std::make_pair(
                            xju::path::AbsolutePath(
                              {xju::path::DirName("a")}),
                            xju::path::FileName("b"))),
                        xju::uri::Query("")));
  }
  {
    const std::string s{"/a/b/.."};
    auto const r{hcp_parser::parseString(s.begin(),s.end(),
                                         requestTarget_())};
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<RequestTargetItem>(r).requestTarget_,
                      RequestTarget(
                        xju::path::AbsFile(
                          std::make_pair(
                            xju::path::AbsolutePath(
                              std::vector<xju::path::DirName>()),
                            xju::path::FileName("a"))),
                        xju::uri::Query("")));
  }
}

}
}

using namespace xju::http;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

