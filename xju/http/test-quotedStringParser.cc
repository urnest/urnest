// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/http/quotedStringParser.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace http
{

void test1() {
  {
    std::string const s(R"--("")--");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         quotedStringParser()));
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<QuotedStringItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<QuotedStringItem>(r).get(),
                      "");
  }
  {
    std::ostringstream ss;
    ss << "\t ";
    ss.put(0x21);
    for(uint8_t i=0x23; i!=0x5c; ++i){
      ss.put(i);
    }
    ss.put(0xff);
    std::string const s("\""+ss.str()+"\"");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         quotedStringParser()));
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<QuotedStringItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<QuotedStringItem>(r).get(),
                      ss.str());
    
  }
  {
    std::ostringstream ss;
    for(uint8_t i=0x5d; i!=0x7f; ++i){
      ss.put(i);
    }
    std::string const s("\""+ss.str()+"\"");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         quotedStringParser()));
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<QuotedStringItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<QuotedStringItem>(r).get(),
                      ss.str());
    
  }
  {
    std::ostringstream ss;
    for(uint8_t i=0x80; i!=0xff; ++i){
      ss.put(i);
    }
    std::string const s("\""+ss.str()+"\"");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         quotedStringParser()));
    xju::assert_equal(
      hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<QuotedStringItem>(r)),
      s);
    xju::assert_equal(hcp_ast::findOnlyChildOfType<QuotedStringItem>(r).get(),
                      ss.str());
    
  }
  {
    std::ostringstream ss;
    std::ostringstream ss2;
    ss << "\t ";
    ss2 << "\\\t\\ ";
    for(uint8_t i=0x21; i!=0x7f; ++i){
      ss.put(i);
      ss2.put('\\');
      ss2.put(i);
    }
    std::string const s("\""+ss2.str()+"\"");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         quotedStringParser()));
    xju::assert_equal(hcp_ast::findOnlyChildOfType<QuotedStringItem>(r).get(),
                      ss.str());
  }
  {
    std::ostringstream ss;
    std::ostringstream ss2;
    for(uint8_t i=0x80; i!=0; ++i){
      ss.put(i);
      ss2.put('\\');
      ss2.put(i);
    }
    std::string const s("\""+ss2.str()+"\"");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         quotedStringParser()));
    xju::assert_equal(hcp_ast::findOnlyChildOfType<QuotedStringItem>(r).get(),
                      ss.str());
  }
  try
  {
    std::string const s(R"--(""")--");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         quotedStringParser()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"only parsed until line 1 column 3.");
  }
  try
  {
    std::string const s(R"--("\")--");
    auto const r(hcp_parser::parseString(s.begin(),s.end(),
                                         quotedStringParser()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to parse HTTP quoted-string at line 1 column 1 because\nfailed to parse \"\\\" at line 1 column 4 because\nline 1 column 4: end of input.");
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

