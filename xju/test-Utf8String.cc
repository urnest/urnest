// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/Utf8String.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

void test1() {
  {
    Utf8String x{""};
    xju::assert_equal(std::string(""),std::string(x));
    xju::assert_equal(x.size(),0U);
  }
  {
    Utf8String x{"fred"};
    xju::assert_equal(std::string("fred"),std::string(x));
    xju::assert_equal(x.size(),4U);
  }
  {
    Utf8String x{std::string{'f','r',(char)0xc0,(char)0x95,'d'}};
    xju::assert_equal(std::string{'f','r',(char)0xc0,(char)0x95,'d'},
                      std::string(x));
    xju::assert_equal(x.size(),4U);
  }
  {
    Utf8String x{std::string{'f','r',(char)0xe7,(char)0x95,(char)0xaf,'d'}};
    xju::assert_equal(std::string{'f','r',(char)0xe7,(char)0x95,(char)0xaf,'d'},
                      std::string(x));
    xju::assert_equal(x.size(),4U);
  }
  {
    Utf8String x{std::string{'f','r',(char)0xf7,(char)0x95,(char)0xaf,(char)0xa2,'d'}};
    xju::assert_equal(std::string{'f','r',(char)0xf7,(char)0x95,(char)0xaf,(char)0xa2,'d'},
                      std::string(x));
    xju::assert_equal(x.size(),4U);
  }
  {
    Utf8String x{std::string{'f','r',(char)0xf7,(char)0x95,(char)0xaf,(char)0xa2,'d',(char)0xc0,(char)0x95,(char)0xe7,(char)0x95,(char)0xaf}};
    xju::assert_equal(std::string{
        'f',
          'r',
          (char)0xf7,(char)0x95,(char)0xaf,(char)0xa2,
          'd',
          (char)0xc0,(char)0x95,
          (char)0xe7,(char)0x95,(char)0xaf},
      std::string(x));
    xju::assert_equal(x.size(),6U);
  }
}

void test2() {
  try
  {
    Utf8String x{std::string{'f','r',(char)0xc0}};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to validate UTF-8 character 3 at byte-offset 2 because\nfailed to validate trailing bytes of 2-byte character because\nfailed to validate trailing byte number 1 of 1 because\nend of input.");
  }
  try
  {
    Utf8String x{std::string{'f','r',(char)0xc0,(char)0xf1}};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to validate UTF-8 character 3 at byte-offset 2 because\nfailed to validate trailing bytes of 2-byte character because\nfailed to validate trailing byte number 1 of 1 because\n0xf1 does not have bit pattern 10xxxxxx.");
  }
  try
  {
    Utf8String x{std::string{'f','r',(char)0xfa}};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to validate UTF-8 character 3 at byte-offset 2 because\nfirst byte of character, 0xfa, has invalid bit pattern 11111xxxx.");
  }
  try
  {
    Utf8String x{std::string{'f','r',(char)0x80}};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to validate UTF-8 character 3 at byte-offset 2 because\nfirst byte of character, 0x80, has invalid bit pattern 10xxxxxx.");
  }
}

}

using namespace xju;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

