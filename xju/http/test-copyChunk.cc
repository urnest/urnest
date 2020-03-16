// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/http/copyChunk.hh>

#include <iostream>
#include <xju/assert.hh>
#include <sstream>

namespace xju
{
namespace http
{

void test1() {

  // no extensions, no data
  {
    std::string const x(R"--(0)--" "\r\n");
    std::istringstream s(x);
    Chunk const c(copyChunk(s,x.size()));
    xju::assert_equal(c.ext_,Chunk::Extensions());
    xju::assert_equal(c.data_,std::vector<uint8_t>());
  }
  // extensions, no data
  {
    std::string const x(R"--(0;name=fred)--" "\r\n");
    std::istringstream s(x);
    Chunk const c(copyChunk(s,x.size()));
    xju::assert_equal(c.ext_,Chunk::Extensions({Chunk::Extension(Chunk::ExtName("name"),Chunk::ExtValue("fred",false))}));
    xju::assert_equal(c.data_,std::vector<uint8_t>());
  }
  // extensions, data
  {
    std::string const x(R"--(A;name=fred;age="12")--" "\r\n"
                        R"--(0123456789)--" "\r\n");
    std::istringstream s(x);
    Chunk const c(copyChunk(s,x.size()));
    xju::assert_equal(c.ext_,Chunk::Extensions({
          Chunk::Extension(Chunk::ExtName("name"),Chunk::ExtValue("fred",false)),
          Chunk::Extension(Chunk::ExtName("age"),Chunk::ExtValue("12",true))}));
    xju::assert_equal(c.data_,std::vector<uint8_t>({'0','1','2','3','4','5','6','7','8','9'}));
  }
  // no extensions, data
  {
    std::string const x(R"--(A)--" "\r\n"
                        R"--(0123456789)--" "\r\n");
    std::istringstream s(x);
    Chunk const c(copyChunk(s,x.size()));
    xju::assert_equal(c.ext_,Chunk::Extensions());
    xju::assert_equal(c.data_,std::vector<uint8_t>({'0','1','2','3','4','5','6','7','8','9'}));
  }
  
  // too big
  REVISIT;
  // not enough data
  // bad ext
  // bad size
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

