// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/json/parse.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace json
{

void test1() {
  {
    auto const x{parse(Utf8String("3.45"))};
    xju::assert_equal(x->asDouble(),3.45);
  }
  {
    auto const x{parse(Utf8String("-3e-2 "))};
    xju::assert_equal(x->asDouble(),-3e-2);
  }
  {
    auto const x{parse(Utf8String(" 0"))};
    xju::assert_equal(x->asDouble(),0.0);
  }
  {
    auto const x{parse(Utf8String("0.6e+1"))};
    xju::assert_equal(x->asDouble(),0.6e+1);
  }
  {
    auto const x{parse(Utf8String("1.06e10"))};
    xju::assert_equal(x->asDouble(),1.06e+10);
  }
  try{
    auto const x{parse(Utf8String("1.06e10"))};
    x->asInt();
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to get 1.06e10 (at line 1 column 1) as an int because\nfailed to convert \"1.06e10\" to an integer because\ncharacter 2 ('.') of \"1.06e10\" unexpected.");
  }
  try{
    auto const x{parse(Utf8String("1.06e10"))};
    x->asBool();
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to get 1.06e10 (at line 1 column 1) as a bool because\n1.06e10 (at line 1 column 1) is not true or false.");
  }

  {
    std::string const s{" [ 1,2 , 3] "};
    auto const x{parse(Utf8String(s))};
    auto const y{x->asArray()};
    xju::assert_equal(y.size(),3);
    xju::assert_equal(y[0]->asDouble(),1.0);
    xju::assert_equal(y[1]->asDouble(),2.0);
    xju::assert_equal(y[2]->asDouble(),3.0);
  }
  {
    std::string const s{" [ ] "};
    auto const x{parse(Utf8String(s))};
    auto const y{x->asArray()};
    xju::assert_equal(y.size(),0U);
  }
  try{
    std::string const s{" [ 3 4] "};
    auto const x{parse(Utf8String(s))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e),R"--(
Failed to parse array at line 1 column 2 because
failed to parse "," then optional whitespace-separated list of element (recurse) commencing with "[" then optional whitespace and terminated by "]" then optional whitespace at line 1 column 2 because
failed to parse "," at line 1 column 6 because
line 1 column 6: expected ',' but found '4'.)--");
  }
  try{
    auto const x{parse(Utf8String("1.06e10"))};
    x->asArray();
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e),R"--(
Failed to get 1.06e10 (at line 1 column 1) as an array because
1.06e10 (at line 1 column 1) is not an Array.)--");
  }
  {
    std::string const s{};
    auto const x{parse(Utf8String(" [ 1,2 , 3] "))};
    auto const y{parse(Utf8String(" [ 1,2 , 3,4] "))};
    xju::assert_less(*x,*y);
  }
}

}
}

using namespace xju::json;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

