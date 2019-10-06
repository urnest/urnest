// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/Chars.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/format.hh>

namespace hcp
{

void test1() {
  {
    Chars const chars("");
    xju::assert_equal(chars.pattern(),"");
    xju::assert_equal(chars.bits(),std::bitset<256>());
    xju::assert_equal(chars.chars(),std::set<char>());
    for(int c=0; c<256; ++c){
      xju::assert_equal(chars.chars().find(c),chars.chars().end());
      xju::assert_equal(chars.bits().test(c),false);
    }
  }
  {
    Chars const chars("a3/");
    xju::assert_equal(chars.pattern(),"a3/");
    for(int c=0; c<256; ++c){
      if (c=='a' || c=='3' || c=='/'){
        xju::assert_not_equal(chars.chars().find(c),chars.chars().end());
        xju::assert_equal(chars.bits().test(c),true);
      }
      else{
        xju::assert_equal(chars.chars().find(c),chars.chars().end());
        xju::assert_equal(chars.bits().test(c),false);
      }
    }
    xju::assert_equal(xju::format::str(chars),"\"a3/\"");
  }
  {
    Chars const chars("a-f");
    xju::assert_equal(chars.pattern(),"a-f");
    for(int c=0; c<256; ++c){
      if (c=='a' || c=='b' || c=='c' || c=='d' || c=='e' || c=='f'){
        xju::assert_equal(chars.bits().test(c),true);
        xju::assert_not_equal(chars.chars().find(c),chars.chars().end());
      }
      else{
        xju::assert_equal(chars.chars().find(c),chars.chars().end());
        xju::assert_equal(chars.bits().test(c),false);
      }
    }
    xju::assert_equal(xju::format::str(chars),"\"a-f\"");
  }
  {
    Chars const chars("-a");
    xju::assert_equal(chars.pattern(),"-a");
    for(int c=0; c<256; ++c){
      if (c=='a' || c=='-'){
        xju::assert_equal(chars.bits().test(c),true);
        xju::assert_not_equal(chars.chars().find(c),chars.chars().end());
      }
      else{
        xju::assert_equal(chars.chars().find(c),chars.chars().end());
        xju::assert_equal(chars.bits().test(c),false);
      }
    }
    xju::assert_equal(xju::format::str(chars),"\"-a\"");
  }
  {
    Chars const chars("a-");
    xju::assert_equal(chars.pattern(),"a-");
    for(int c=0; c<256; ++c){
      if (c=='a' || c=='-'){
        xju::assert_equal(chars.bits().test(c),true);
        xju::assert_not_equal(chars.chars().find(c),chars.chars().end());
      }
      else{
        xju::assert_equal(chars.chars().find(c),chars.chars().end());
        xju::assert_equal(chars.bits().test(c),false);
      }
    }
    xju::assert_equal(xju::format::str(chars),"\"a-\"");
  }
  {
    Chars const chars("-");
    xju::assert_equal(chars.pattern(),"-");
    for(int c=0; c<256; ++c){
      if (c=='-'){
        xju::assert_equal(chars.bits().test(c),true);
        xju::assert_not_equal(chars.chars().find(c),chars.chars().end());
      }
      else{
        xju::assert_equal(chars.chars().find(c),chars.chars().end());
        xju::assert_equal(chars.bits().test(c),false);
      }
    }
    xju::assert_equal(xju::format::str(chars),"\"-\"");
  }
  {
    Chars const chars("+--");
    xju::assert_equal(chars.pattern(),"+--");
    for(int c=0; c<256; ++c){
      if (c=='+' || c==',' || c=='-'){
        xju::assert_equal(chars.bits().test(c),true);
        xju::assert_not_equal(chars.chars().find(c),chars.chars().end());
      }
      else{
        xju::assert_equal(chars.chars().find(c),chars.chars().end());
        xju::assert_equal(chars.bits().test(c),false);
      }
    }
    xju::assert_equal(xju::format::str(chars),"\"+--\"");
  }
  {
    Chars const chars("f-a");
    xju::assert_equal(chars.pattern(),"f-a");
    for(int c=0; c<256; ++c){
      if (c=='-' || c=='f' || c=='a'){
        xju::assert_equal(chars.bits().test(c),true);
        xju::assert_not_equal(chars.chars().find(c),chars.chars().end());
      }
      else{
        xju::assert_equal(chars.chars().find(c),chars.chars().end());
        xju::assert_equal(chars.bits().test(c),false);
      }
    }
    xju::assert_equal(xju::format::str(chars),"\"f-a\"");
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

