//     -*- mode: c++ ; c-file-style: "xju" ; -*-
//
// Copyright (c) 2008
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/path.hh>
#include <iostream>
#include "xju/assert.hh"
#include "xju/Exception.hh"
#include "xju/next.hh"

namespace xju
{
namespace path
{

void test1()
{
  {
    xju::path::RelativePath x("");
    xju::assert_equal(x.size(), 0);
  }
  {
    xju::path::RelativePath x("a");
    xju::assert_equal(x.size(), 1);
    xju::assert_equal((*x.begin()), xju::path::DirName("a"));
  }
  {
    xju::path::RelativePath x("a//b");
    xju::assert_equal(x.size(), 2);
    xju::assert_equal((*x.begin()), xju::path::DirName("a"));
    xju::assert_equal((*(x.begin()+1)), xju::path::DirName("b"));
    xju::assert_equal(str(x), "a/b");
  }
}

void test2()
{
  try {
    xju::path::AbsolutePath x("");
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e), "Failed to validate absolute path \"\" because\n\"\" does not start with '/'.");
  }
  try {
    xju::path::AbsolutePath x("a/b");
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e), "Failed to validate absolute path \"a/b\" because\n\"a/b\" does not start with '/'.");
  }
  {
    xju::path::AbsolutePath x("/a/b");
    xju::assert_equal(x.size(), 2);
    xju::assert_equal((*x.begin())._, "a");
    xju::assert_equal((*xju::next(x.begin()))._, "b");
  }
  {
    xju::path::AbsolutePath x("/a/../b");
    xju::assert_equal(x.size(), 1);
    xju::assert_equal((*x.begin())._, "b");
  }
  {
    xju::path::AbsolutePath x("/a/..//b/./");
    xju::assert_equal(x.size(), 1);
    xju::assert_equal((*x.begin()), xju::path::DirName("b"));
  }
  try {
    xju::path::AbsolutePath x("/a/../../b");
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e), "Failed to validate absolute path \"/a/../../b\" because\nfailed to normalise path a/../../b because\ntoo many '..'s at component 3.");
  }
  
}

// AbsolutePath+RelativePath
void test3()
{
  {
    xju::path::AbsolutePath x(
      xju::path::AbsolutePath("/a/..//b/./")+
      xju::path::RelativePath("c/d"));
    xju::assert_equal(x.size(), 3);
    xju::assert_equal((*x.begin()), xju::path::DirName("b"));
    xju::assert_equal((*(x.begin()+1)), xju::path::DirName("c"));
    xju::assert_equal((*(x.begin()+2)), xju::path::DirName("d"));
  }
  try
  {
    xju::path::AbsolutePath x(
      xju::path::AbsolutePath("/a")+
      xju::path::RelativePath("../../c"));
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e), "Failed to normalise absolute path /a/../../c because\nfailed to normalise path a/../../c because\ntoo many '..'s at component 3.");
  }
}

// root
void test4()
{
  xju::assert_equal(str(xju::path::root()), "/");
}

void test5()
{
  xju::assert_equal(xju::path::basename("/a/b"), xju::path::FileName("b"));
  xju::assert_equal(xju::path::basename("c"), xju::path::FileName("c"));
  xju::assert_equal(xju::path::basename("/c"), xju::path::FileName("c"));
}

void test6()
{
  xju::assert_equal(str(xju::path::relative_dirname("a/b")), "a");
  xju::assert_equal(str(xju::path::relative_dirname("a/b/c")), "a/b");
  xju::assert_equal(str(xju::path::relative_dirname("c")), "");
  xju::assert_equal(str(xju::path::relative_dirname("")), "");
  try {
    xju::path::relative_dirname("/a/b");
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e), "Failed to get directory part of /a/b assuming it is a relative path because\n/a/b starts with '/'.");
  }
}

void test7()
{
  xju::assert_equal(str(xju::path::absolute_dirname("/a/b")), "/a");
  xju::assert_equal(str(xju::path::absolute_dirname("/a/b/c")), "/a/b");
  xju::assert_equal(str(xju::path::absolute_dirname("/c")), "/");
  xju::assert_equal(str(xju::path::absolute_dirname("/")), "/");
  try {
    xju::path::absolute_dirname("a/b");
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e), "Failed to get directory part of a/b assuming it is an absolute path because\na/b does not start with '/'.");
  }
}

void test8()
{
  xju::assert_equal(
    xju::path::split("/a/b/c"),
    std::make_pair(xju::path::AbsolutePath("/a/b"), xju::path::FileName("c")));
  xju::assert_equal(
    xju::path::dirname(xju::path::split("/a/b/c")),
    std::make_pair(xju::path::AbsolutePath("/a"), xju::path::DirName("b")));

  xju::assert_equal(
    xju::path::splitdir("/a/b/c"),
    std::make_pair(xju::path::AbsolutePath("/a/b"), xju::path::DirName("c")));
  xju::assert_equal(
    xju::path::dirname(xju::path::splitdir("/a/b/c")),
    std::make_pair(xju::path::AbsolutePath("/a"), xju::path::DirName("b")));
}

void test9()
{
  xju::assert_equal(split(FileName("x")),
                    std::make_pair(BaseName("x"),Extension("")));
  xju::assert_equal(split(FileName(".y")),
                    std::make_pair(BaseName(".y"),Extension("")));
  xju::assert_equal(split(FileName(".y.txt")),
                    std::make_pair(BaseName(".y"),Extension(".txt")));
  xju::assert_equal(split(FileName("y.tar.gz")),
                    std::make_pair(BaseName("y.tar"),Extension(".gz")));
}

}
}

using namespace xju::path;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  test3(), ++n;
  test4(), ++n;
  test5(), ++n;
  test6(), ++n;
  test7(), ++n;
  test8(), ++n;
  test9(), ++n;
  
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}
