// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/getIdentifierAt.hh>

#include <iostream>
#include <xju/assert.hh>

namespace hcp
{

void test1() {
  xju::assert_equal(getIdentifierAt("abc_12",0),Identifier("abc_12"));
  xju::assert_equal(getIdentifierAt("abc_12",1),Identifier("abc_12"));
  xju::assert_equal(getIdentifierAt("abc_12",2),Identifier("abc_12"));
  xju::assert_equal(getIdentifierAt("abc_12",3),Identifier("abc_12"));
  xju::assert_equal(getIdentifierAt("abc_12",4),Identifier("abc_12"));
  xju::assert_equal(getIdentifierAt("abc_12",5),Identifier("abc_12"));
  xju::assert_equal(getIdentifierAt("abc_12",6),Identifier("abc_12"));
  xju::assert_equal(getIdentifierAt("a[33]",1),Identifier("a"));
  try {
    auto const x(getIdentifierAt("abc_12",7));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to get C++ identifier at or just before offset 7 in text because\noffset 7 is not within text, which has length 6.");
  }
  try {
    auto const x(getIdentifierAt("0abc_12",0));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to get C++ identifier at or just before offset 0 in text because\nfailed to get identifier at or just before \"^^\" in \"^^0abc_12\" because\nidentifier cannot start with digit ('0' at offset 0).");
  }
  try {
    auto const x(getIdentifierAt("<0abc_12",1));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to get C++ identifier at or just before offset 1 in text because\nfailed to get identifier at or just before \"^^\" in \"<^^0abc_12\" because\nidentifier cannot start with digit ('0' at offset 1).");
  }
  try {
    auto const x(getIdentifierAt("<0abc_12",3));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to get C++ identifier at or just before offset 3 in text because\nfailed to get identifier at or just before \"^^\" in \"<0a^^bc_12\" because\nidentifier cannot start with digit ('0' at offset 1).");
  }
  try {
    auto const x(getIdentifierAt("some stuff and then <0abc_12 and then some stuff",25));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to get C++ identifier at or just before offset 25 in text because\nfailed to get identifier at or just before \"^^\" in \"then <0abc^^_12 and th\" because\nidentifier cannot start with digit ('0' at offset 21).");
  }
  try {
    auto const x(getIdentifierAt("a[33]",5));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e),"Failed to get C++ identifier at or just before offset 5 in text because\nfailed to get identifier at or just before \"^^\" in \"a[33]^^\" because\ncharacter at or just before offset 5 is not an identifier char ie not a..z, A..Z, 0..9 or _.");
  }
}

}

using namespace hcp;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

