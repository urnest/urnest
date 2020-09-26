// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/net/istream.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/MemIBuf.hh>

namespace xju
{
namespace net
{

void test1() {
  xju::MemIBuf b({0x32,
                  0x46,0x47,
                  0x51,0x52,0x53,0x54,
                  0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77});
  istream s(b);
  xju::assert_equal(s.get8(),0x32);
  xju::assert_equal(s.get16(),0x4647);
  xju::assert_equal(s.get32(),0x51525354);
  xju::assert_equal(s.get64(),0x7071727374757677);

  try{
    s.get8();
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"end of input.");
  }
}

void test2() {
  std::string const x("the quick brown fox jumped over the lazy dog");
  xju::MemIBuf b(std::vector<uint8_t>(x.begin(),x.end()),
                 9);
  istream s(b);
  std::string y;
  s.getN(15,std::back_inserter(y));
  xju::assert_equal(y,"the quick brown");
  y+=s.readThrough("jumped",1000);
  xju::assert_equal(y,"the quick brown fox jumped");
  try{
    y=s.readThrough("dog",10);
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to read up to and including \"dog\" expected within 10 characters because\n10 bytes read without seeing token.");
  }
  try{
    y=s.readThrough("cat",1000);
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to read up to and including \"cat\" expected within 1000 characters because\nend of input.");
  }
    
}

}
}

using namespace xju::net;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

