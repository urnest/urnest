// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/http/chunkExtParser.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace http
{

void test1() {
  {
    std::string const s(";name=fred");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         chunkExtParser()));
    xju::assert_equal(hcp_ast::findOnlyChildOfType<ChunkExtItem>(r).get(),
                      Chunk::Extension(
                        Chunk::ExtName("name"),
                        Chunk::ExtValue("fred",false)));
  }
  {
    std::string const s(";name=\"fred\"");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         chunkExtParser()));
    xju::assert_equal(hcp_ast::findOnlyChildOfType<ChunkExtItem>(r).get(),
                      Chunk::Extension(
                        Chunk::ExtName("name"),
                        Chunk::ExtValue("fred",true)));
  }
  try
  {
    std::string const s(";name =\"fred\"");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         chunkExtParser()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse HTTP Chunk Extension at line 1 column 1 because\nfailed to parse \"=\" at line 1 column 6 because\nline 1 column 6: expected '=' but found ' '.");
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

