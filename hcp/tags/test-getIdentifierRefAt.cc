// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/tags/getIdentifierRefAt.hh>

#include <iostream>
#include <xju/assert.hh>

namespace hcp
{
namespace tags
{

void test1() {
  xju::assert_equal(getIdentifierRefAt("abc_12",0),IdentifierRef("abc_12"));
  xju::assert_equal(getIdentifierRefAt("abc_12",1),IdentifierRef("abc_12"));
  xju::assert_equal(getIdentifierRefAt("abc_12",2),IdentifierRef("abc_12"));
  xju::assert_equal(getIdentifierRefAt("abc_12",3),IdentifierRef("abc_12"));
  xju::assert_equal(getIdentifierRefAt("abc_12",4),IdentifierRef("abc_12"));
  xju::assert_equal(getIdentifierRefAt("abc_12",5),IdentifierRef("abc_12"));
  xju::assert_equal(getIdentifierRefAt("abc_12",6),IdentifierRef("abc_12"));
  xju::assert_equal(getIdentifierRefAt("a[33]",1),IdentifierRef("a"));
  try {
    auto const x(getIdentifierRefAt("abc_12",7));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to get C++ identifier ref at or just before offset 7 in text because\noffset 7 is not within text, which has length 6.");
  }
  try {
    auto const x(getIdentifierRefAt("0abc_12",0));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to get C++ identifier ref at or just before offset 0 in text because\nfailed to get identifier ref at or just before \"^^\" in \"^^0abc_12\" because\nidentifier cannot start with digit ('0' at offset 0).");
  }
  try {
    auto const x(getIdentifierRefAt("<0abc_12",1));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to get C++ identifier ref at or just before offset 1 in text because\nfailed to get identifier ref at or just before \"^^\" in \"<^^0abc_12\" because\nidentifier cannot start with digit ('0' at offset 1).");
  }
  try {
    auto const x(getIdentifierRefAt("<0abc_12",3));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to get C++ identifier ref at or just before offset 3 in text because\nfailed to get identifier ref at or just before \"^^\" in \"<0a^^bc_12\" because\nidentifier cannot start with digit ('0' at offset 1).");
  }
  try {
    auto const x(getIdentifierRefAt("some stuff and then <0abc_12 and then some stuff",25));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to get C++ identifier ref at or just before offset 25 in text because\nfailed to get identifier ref at or just before \"^^\" in \"then <0abc^^_12 and th\" because\nidentifier cannot start with digit ('0' at offset 21).");
  }
  try {
    auto const x(getIdentifierRefAt("a[33]",5));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to get C++ identifier ref at or just before offset 5 in text because\nfailed to get identifier ref at or just before \"^^\" in \"a[33]^^\" because\ncharacter at or just before offset 5 is not an identifier char ie not a..z, A..Z, 0..9 or _.");
  }
}

}
}

using namespace hcp::tags;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

