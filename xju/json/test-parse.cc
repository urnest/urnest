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

