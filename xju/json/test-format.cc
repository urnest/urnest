// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/json/format.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/Utf8String.hh>
#include <xju/json/parse.hh>

namespace xju
{
namespace json
{

void test1() {
  xju::Utf8String const s{R"--({
  "e": {},
  "f": [],
  "fred": 5,
  "jock": [
    89.6,
    false,
    true,
    null,
    "\"\\\/\a\b\f\n\r\t"
  ]
})--"};
  auto const e{xju::json::parse(s)};
  xju::assert_equal(xju::json::format(*e,xju::Utf8String("  ")),s);

  xju::Utf8String const t{R"--({"e":{},"f":[],"fred":5,"jock":[89.6,false,true,null,"\"\\\/\a\b\f\n\r\t"]})--"};
  xju::assert_equal(xju::json::format(*e,xju::Utf8String("")),t);
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

