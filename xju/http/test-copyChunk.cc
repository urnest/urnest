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
  try
  {
    std::string const x(R"--(A)--" "\r\n"
                        R"--(0123456789)--" "\r\n");
    std::istringstream s(x);
    Chunk const c(copyChunk(s,x.size()-1));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to copy HTTP chunk from specified std::istream assuming end of chunk occurs within 14 bytes because\n10 bytes of chunk data would exceed 14 total bytes.");
  }
  // not enough data
  try
  {
    std::string const x(R"--(30)--" "\r\n"
                        R"--(0123456789)--" "\r\n");
    std::istringstream s(x);
    Chunk const c(copyChunk(s,1024));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to copy HTTP chunk from specified std::istream assuming end of chunk occurs within 1024 bytes because\nfailed to read 48 bytes of chunk-data then \\r\\n because\nread of next byte failed having read 12 bytes.");
  }
  try
  {
    std::string const x(R"--(B)--" "\r\n"
                        R"--(0123456789)--" "\r\n");
    std::istringstream s(x);
    Chunk const c(copyChunk(s,1024));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to copy HTTP chunk from specified std::istream assuming end of chunk occurs within 1024 bytes because\nfailed to read 11 bytes of chunk-data then \\r\\n because\nfailed to read \\r\\n after data bytes because\nfailed to read stream until \"\\r\\n\" reading at most 2 chars because\nend of file before \"\\r\\n\" amongst \"\\n\".");
  }
  // bad ext
  try
  {
    std::string const x(R"--(A;name=")--" "\r\n"
                        R"--(0123456789)--" "\r\n");
    std::istringstream s(x);
    Chunk const c(copyChunk(s,1024));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to copy HTTP chunk from specified std::istream assuming end of chunk occurs within 1024 bytes because\nfailed to parse HTTP Chunk Extension Value at line 1 column 8 because\nfailed to parse HTTP quoted-string at line 1 column 8 because\nfailed to parse \"\\\" at line 1 column 9 because\nline 1 column 9: expected '\\\\' but found '\\r'.");
  }
  // bad size
  try
  {
    std::string const x(R"--(9)--" "\r\n"
                        R"--(0123456789)--" "\r\n");
    std::istringstream s(x);
    Chunk const c(copyChunk(s,x.size()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to copy HTTP chunk from specified std::istream assuming end of chunk occurs within 15 bytes because\nfailed to read 9 bytes of chunk-data then \\r\\n because\nfailed to read \\r\\n after data bytes because\nfailed to read stream until \"\\r\\n\" reading at most 2 chars because\n2 chars read without seeing \"\\r\\n\" amongst \"9\\r\".");
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

