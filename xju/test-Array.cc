// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/Array.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
struct X
{
  X(std::string const& name, uint32_t value) noexcept
    :name_(name),
     value_(value)
  {
  }
  std::string name_;
  uint32_t value_;
};

void test1() {
  xju::Array<X,3> x(X("",0));
  xju::assert_equal(x.size(),3U);
  x[1]=X("fred",1U);
  x[2]=X("jock",2U);
  xju::assert_equal(x[1],X("fred",1U));
  xju::assert_equal(x.end()-x.begin(),3U);
  xju::assert_equal(x.rend()-x.rbegin(),3U);

  xju::assert_equal(x.begin()+3U,x.end());
  xju::assert_equal(3U+x.begin(),x.end());
  xju::assert_equal(x.end()-3U,x.begin());
  
  auto i=x.begin();
  assert_equal(*++i,X("fred",1U));
  assert_equal(*i++,X("fred",1U));
  assert_equal(*i,X("jock",2U));
  assert_equal(++i,x.end());

  x-=2;
  assert_equal(*i--,X("fred",1U));
  assert_equal(*i,X("",0U));
  assert_equal(i,x.begin());

  x+=3;
  assert_equal(i,x.end());
}

}

using namespace xju;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

