// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/http/copyBody.hh>

#include <iostream>
#include <xju/assert.hh>
#include <sstream>
#include <string>

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
    auto const body(copyBody(s,256));
    for(int i=0; i<256; ++i){
      xju::assert_equal(body[i],i);
    }
  }
  try{
    std::ostringstream x;
    for(int i=0; i<56; ++i){
      x.put(i);
    }
    std::string const y(x.str());
    std::istringstream s(y);
    auto const body(copyBody(s,256));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to copy HTTP body of size 256 because\nread of next byte failed having read 56 bytes.");
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

