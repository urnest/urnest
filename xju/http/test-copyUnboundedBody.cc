// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/http/copyUnboundedBody.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace http
{

void test1() {
  {
    std::ostringstream x;
    for(int i=0; i<256; ++i){
      x.put(i);
    }
    std::string const y(x.str());
    std::istringstream s(y);
    auto const body(copyUnboundedBody(s,256));
    for(int i=0; i<256; ++i){
      xju::assert_equal(body[i],i);
    }
  }
  {
    std::ostringstream x;
    for(int i=0; i<256; ++i){
      x.put(i);
    }
    std::string const y(x.str());
    std::istringstream s(y);
    auto const body(copyUnboundedBody(s,257));
    for(int i=0; i<256; ++i){
      xju::assert_equal(body[i],i);
    }
  }
  try{
    std::ostringstream x;
    for(int i=0; i<256; ++i){
      x.put(i);
    }
    std::string const y(x.str());
    std::istringstream s(y);
    auto const body(copyUnboundedBody(s,255));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to read HTTP unbounded body of max size 255 from specified istream because\nread 255 bytes and not yet at end of file.");
  }
}

}
}

using namespace xju::http;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

