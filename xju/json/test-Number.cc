// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/json/Number.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace json
{

void test1() {
  {
    Number x{"3"};
    xju::assert_equal(x.asDouble(),3.0);
    xju::assert_equal(x.asInt(),3);
  }
  {
    Number x{"3"};
    Number y{"3.6"};
    xju::assert_less(x,y);
  }
}

void test2() {
  try
  {
    Number x{"3.62"};
    x.asInt();
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to treat 3.62 as an int because\nfailed to convert \"3.62\" to an integer because\ncharacter 2 ('.') of \"3.62\" unexpected.");
  }
}

}
}

using namespace xju::json;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}
