// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/utf8/encodeCodePoint.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace utf8
{

void test1() {
  xju::assert_equal(std::string(encodeCodePoint(0x24)),
                    std::string{0x24});
  xju::assert_equal(std::string(encodeCodePoint(0xA2)),
                    std::string{(char)0xC2,(char)0xA2});
  xju::assert_equal(std::string(encodeCodePoint(0x20AC)),
                    std::string{(char)0xE2,(char)0x82,(char)0xAC});
  xju::assert_equal(std::string(encodeCodePoint(0x10348)),
                    std::string{(char)0xF0,(char)0x90,(char)0x8D,(char)0x88});
}

void test2() {
  // boundaries
  xju::assert_equal(std::string(encodeCodePoint(0x00)),
                    std::string{0x00});
  xju::assert_equal(std::string(encodeCodePoint(0x7f)),
                    std::string{0x7f});

  xju::assert_equal(std::string(encodeCodePoint(0x80)),
                    std::string{(char)0xC2,(char)0x80});
  xju::assert_equal(std::string(encodeCodePoint(0x7ff)),
                    std::string{(char)0xDF,(char)0xbf});

  xju::assert_equal(std::string(encodeCodePoint(0x800)),
                    std::string{(char)0xE0,(char)0xa0,(char)0x80});
  xju::assert_equal(std::string(encodeCodePoint(0xffff)),
                    std::string{(char)0xEF,(char)0xbf,(char)0xbf});

  xju::assert_equal(std::string(encodeCodePoint(0x10000)),
                    std::string{(char)0xF0,(char)0x90,(char)0x80,(char)0x80});
  xju::assert_equal(std::string(encodeCodePoint(0x10ffff)),
                    std::string{(char)0xF4,(char)0x8f,(char)0xbf,(char)0xbf});
  try{
    encodeCodePoint(0x110000);
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"0x00110000 > 0x10FFFF (the last Unicode character).");
  }
}

}
}

using namespace xju::utf8;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

