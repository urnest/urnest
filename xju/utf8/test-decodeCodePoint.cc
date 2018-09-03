// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/utf8/decodeCodePoint.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/utf8/encodeCodePoint.hh>

namespace xju
{
namespace utf8
{

void test1() {
  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0x24))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0x24);
    xju::assert_equal(y.second.atEnd(),true);
  }
  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0xa2))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0xa2);
    xju::assert_equal(y.second.atEnd(),true);
  }
  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0x20ac))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0x20ac);
    xju::assert_equal(y.second.atEnd(),true);
  }
  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0x10348))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0x10348);
    xju::assert_equal(y.second.atEnd(),true);
  }
}

void test2() {
  // boundaries
  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0x00))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0x00);
    xju::assert_equal(y.second.atEnd(),true);
  }
  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0x7f))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0x7f);
    xju::assert_equal(y.second.atEnd(),true);
  }
  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0x80))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0x80);
    xju::assert_equal(y.second.atEnd(),true);
  }

  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0x7ff))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0x7ff);
    xju::assert_equal(y.second.atEnd(),true);
  }

  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0x800))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0x800);
    xju::assert_equal(y.second.atEnd(),true);
  }
  
  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0xd7ff))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0xd7ff);
    xju::assert_equal(y.second.atEnd(),true);
  }

  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0xe000))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0xe000);
    xju::assert_equal(y.second.atEnd(),true);
  }

  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0xffff))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0xffff);
    xju::assert_equal(y.second.atEnd(),true);
  }

  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0x10000))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0x10000);
    xju::assert_equal(y.second.atEnd(),true);
  }
  
  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0x10ffff))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0x10ffff);
    xju::assert_equal(y.second.atEnd(),true);
  }

  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0xd7ff))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0xd7ff);
    xju::assert_equal(y.second.atEnd(),true);
  }

  {
    auto const x{std::string(xju::utf8::encodeCodePoint(0xe000))};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_equal(y.first,0xe000);
    xju::assert_equal(y.second.atEnd(),true);
  }

  try
  {
    std::string const x{std::string{(char)0xc0}};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to decode unicode character from UTF-8 string because
failed to decode byte 2 of 2-byte utf-8 char sequence because
end of input at line 1 column 2.
)--");
  }
  
  try
  {
    std::string const x{std::string{(char)0xc0,(char)0xf1}};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to decode unicode character from UTF-8 string because
failed to decode byte 2 of 2-byte utf-8 char sequence because
0xf1 does not have bit pattern 10xxxxxx.
)--");
  }
  
  try
  {
    std::string const x{std::string{(char)0xfa}};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to decode unicode character from UTF-8 string because
first byte of character, 0xfa, has invalid bit pattern 11111xxxx.
)--");
  }
  
  try
  {
    std::string const x{std::string{(char)0x80}};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to decode unicode character from UTF-8 string because
first byte of character, 0x80, has invalid bit pattern 10xxxxxx.
)--");
  }
  
  try
  {
    std::string const x{std::string{(char)0xf7,(char)0x80,(char)0x80,(char)0x80}};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to decode unicode character from UTF-8 string because
0x001c0000 is invalid (the last valid unicode code point is 0x10FFFF).
)--");
  }
  
  try
  {
    std::string const x{std::string{(char)0xed,(char)0xbf,(char)0xbf}};
    auto const y{decodeCodePoint(xju::parse::iterator(x.begin(),x.end()))};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    std::cerr << readableRepr(e) << std::endl;
    xju::assert_equal("\n"+readableRepr(e)+"\n",R"--(
Failed to decode unicode character from UTF-8 string because
0x0000dfff is not a valid Unicode character because it is in the utf-16 surrogate pair reserved range 0xD800..0xDFFF.
)--");
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

