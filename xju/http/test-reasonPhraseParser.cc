// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/http/reasonPhraseParser.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace http
{

void test1() {
  {
    std::string const s("fred the \t fox");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         reasonPhraseParser()));
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<ReasonPhraseItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<ReasonPhraseItem>(r).get(),
                      ReasonPhrase("fred the \t fox"));
  }
  try
  {
    std::string const s("fred\003the \t fox");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         reasonPhraseParser()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"only parsed until line 1 column 5.");
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
