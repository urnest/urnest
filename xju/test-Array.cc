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
  friend bool operator==(X const& x, X const& y) noexcept
  {
    return std::make_pair(x.name_,x.value_)==std::make_pair(y.name_,y.value_);
  }
  friend bool operator<(X const& x, X const& y) noexcept
  {
    return std::make_pair(x.name_,x.value_)<std::make_pair(y.name_,y.value_);
  }
};

void test1() {
  xju::Array<X,3> x(X("",0));
  xju::assert_equal(x.size(),3U);
  xju::assert_equal(x.max_size(),3U);
  xju::assert_equal(x.empty(),false);
  x[1]=X("fred",1U);
  x[2]=X("jock",2U);
  xju::assert_equal(x.front(),X("",0U));
  xju::assert_equal(x.back(),X("jock",2U));
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

  i-=2;
  assert_equal(*i--,X("fred",1U));
  assert_equal(*i,X("",0U));
  assert_equal(i,x.begin());

  assert_equal(i+3,x.end());
  assert_equal(x.end()-3,x.begin());
  assert_equal(x.end()-x.begin(),3U);
  i+=3;
  assert_equal(i,x.end());
  
  xju::assert_equal(x.begin(),x.begin());
  xju::assert_equal(x.rbegin(),x.rbegin());
  xju::assert_equal(x.cbegin(),x.cbegin());
  xju::assert_equal(x.crbegin(),x.crbegin());
  xju::assert_not_equal(x.begin(),x.end());
  xju::assert_not_equal(x.rbegin(),x.rend());
  xju::assert_not_equal(x.cbegin(),x.cend());
  xju::assert_not_equal(x.crbegin(),x.crend());

  xju::assert_less(i-1,i);
  xju::assert_greater(i,i-1);
  xju::assert_less_equal(i-1,i);
  xju::assert_greater_equal(i,i-1);
  xju::assert_less_equal(i,i);
  xju::assert_greater_equal(i,i);
  
  try{
    x.at(3);
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to get element at offset n because\nArray of size 3 cannot have 4 elements.");
  }
}

void test2() {
  xju::Array<X,3> const x{X("",0),X("fred",1U),X("jock",2U)};
  xju::assert_equal(x.front(),X("",0U));
  xju::assert_equal(x[1],X("fred",1U));
  xju::assert_equal(x.back(),X("jock",2U));
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

  i-=2;
  assert_equal(*i--,X("fred",1U));
  assert_equal(*i,X("",0U));
  assert_equal(i,x.begin());

  assert_equal(i+3,x.end());
  assert_equal(x.end()-3,x.begin());
  assert_equal(x.end()-x.begin(),3U);
  i+=3;
  assert_equal(i,x.end());

  try{
    x.at(3);
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to get element at offset n because\nArray of size 3 cannot have 4 elements.");
  }
}

void test3() {
  xju::Array<X,3> const x{X("",0),X("fred",1U),X("jock",2U)};
  xju::Array<X,3> y(x);
  xju::assert_equal(x[0],y[0]);
  xju::assert_equal(x[1],y[1]);
  xju::assert_equal(x[2],y[2]);
}

void test4() {
  xju::Array<X,3> const x{X("",0),X("fred",1U),X("jock",2U)};
  xju::Array<X,3> y(std::move(x));
  xju::assert_equal(y[0],X("",0));
  xju::assert_equal(y[1],X("fred",1U));
  xju::assert_equal(y[2],X("jock",2U));
}

void test5() {
  xju::Array<X,3> const x{X("",0),X("fred",1U),X("jock",2U)};
  xju::Array<X,3> y{X("a",4U),X("b",5U),X("c",6U)};
  y=x;
  xju::assert_equal(y[0],X("",0));
  xju::assert_equal(y[1],X("fred",1U));
  xju::assert_equal(y[2],X("jock",2U));
}

void test6() {
  xju::Array<X,3> const x{X("",0),X("fred",1U),X("jock",2U)};
  assert_equal(x.data()[0],X("",0));
  xju::Array<X,3> y{X("a",4U),X("b",5U),X("c",6U)};
  std::copy(x.begin(),x.end(),y.data());
  assert_equal(y,x);

  y[1]=X("a",5U);
  assert_not_equal(x,y);
  assert_less(y,x);
  assert_less_equal(y,x);
  assert_greater_equal(x,y);
  assert_greater(x,y);
}

}

using namespace xju;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  test3(), ++n;
  test4(), ++n;
  test5(), ++n;
  test6(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

