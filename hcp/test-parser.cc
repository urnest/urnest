//     -*- mode: c++ ; c-file-style: "xju" ; -*-
//
// Copyright (c) 2008
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
static const char rcsid[] = "$RCSfile: twt_cxx.el,v $ $Revision: 1.2 $";
#include <hcp/parser.hh>
#include "xju/assert.hh"
#include <xju/readFile.hh>

void assert_readableRepr_equal(std::string const& rr,
                               std::string const& expect,
                               xju::Traced const& t) throw() {
  if (rr != expect) {
    std::cerr << t.file() << ":" << t.line() << ": error: " 
              << "exception readable-repr mismatch" << std::endl
              << "got: " << std::endl
              << xju::format::quote(xju::format::cEscapeString(rr))
              << std::endl
              << "expected: " << std::endl
              << xju::format::quote(xju::format::cEscapeString(expect))
              << std::endl;
    xju::assert_equal(rr, expect);
  }
}

void assert_readableRepr_equal(xju::Exception const& e,
                               std::string const& expect,
                               xju::Traced const& t) throw() {
  std::string const rr(readableRepr(e));
  assert_readableRepr_equal(rr, expect, t);
}

void assert_readableRepr_equal(hcp_parser::Exception const& e,
                               std::string const& expect,
                               xju::Traced const& t) throw() {
  std::string const rr(readableRepr(e));
  assert_readableRepr_equal(rr, expect, t);
}

void test1()
{
  hcp_parser::Cache cache(new hcp_parser::CacheVal());
  hcp_parser::Options const options(false, cache);
  std::string const x("ab");
  hcp_parser::PV const y(*hcp_parser::parseAnyChar->parse_(
    hcp_parser::I(x.begin(), x.end()),
    options));
  xju::assert_equal(reconstruct(y.first), "a");
  xju::assert_equal(y.second.x_, x.begin()+1);

  hcp_parser::PV const y2(*hcp_parser::parseAnyChar->parse_(
    y.second,
    options));
  xju::assert_equal(reconstruct(y2.first), "b");
  xju::assert_equal(y2.second.x_, x.begin()+2);
  
  hcp_parser::ParseResult const r(
    hcp_parser::parseAnyChar->parse_(
      y2.second,
      options));
  xju::assert_equal(r.failed(), true);
  xju::assert_equal(readableRepr(r.e()), "line 1 column 3: end of input");

  hcp_ast::CompositeItem root;
  hcp_parser::I at(x.begin(), x.end());
  at = parse(root, at, hcp_parser::parseAnyChar);
  xju::assert_equal(at.x_, x.begin()+1);
  at = parse(root, at, hcp_parser::parseAnyChar);
  xju::assert_equal(at.x_, x.begin()+2);
  
  xju::assert_equal(reconstruct(root), "ab");

  try {
    hcp_parser::I const y3(parse(
      root,
      at,
      hcp_parser::parseAnyChar));
    xju::assert_not_equal(y3, y3);
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse any char at line 1 column 3 because\nline 1 column 3: end of input.", XJU_TRACED);
  }

}

void test2()
{
  std::string const x("ab");
  hcp_ast::CompositeItem root;
  hcp_parser::I at(x.begin(), x.end());
  at = parse(root, at, hcp_parser::zeroOrMore*hcp_parser::parseAnyChar);
  xju::assert_equal(reconstruct(root), "ab");
  xju::assert_equal(at.atEnd(), true);
}

void test3()
{
  hcp_parser::Cache cache(new hcp_parser::CacheVal());

  hcp_parser::Options const options(false, cache);
  
  std::string const x("ab");
  hcp_ast::CompositeItem root;
  hcp_parser::I at(x.begin(), x.end());

  at = parse(root, at, (hcp_parser::parseAnyChar+
                        hcp_parser::parseAnyChar));
  xju::assert_equal(reconstruct(root), "ab");
  xju::assert_equal(at.atEnd(), true);

  try {
    hcp_parser::PR p(hcp_parser::parseAnyChar+
                     hcp_parser::parseAnyChar+
                     hcp_parser::parseAnyChar);
    at = parse(root, at, p);
    xju::assert_not_equal(at, at);
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse any char then any char then any char at line 1 column 3 because\nline 1 column 3: end of input.", XJU_TRACED);
 }
}

void test4()
{
  hcp_parser::Cache cache(new hcp_parser::CacheVal());
  hcp_parser::Options const options(false, cache);
  std::string const x("abcd");
  hcp_ast::CompositeItem root;
  hcp_parser::I at(x.begin(), x.end());
  at = parse(root, at, (hcp_parser::zeroOrMore*hcp_parser::parseOneOfChars("abc")));
  xju::assert_equal(reconstruct(root), "abc");
  xju::assert_equal(at.atEnd(), false);

  try {
    hcp_parser::PR p(hcp_parser::parseOneOfChars("abc"));
    at = parse(root, at, p);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e), "Failed to parse one of chars [abc] at line 1 column 4 because\nline 1 column 4: 'd' is not one of chars [abc].");
  }
}

void test5()
{
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    
    hcp_parser::Options const options(false, cache);
    std::string const x("abcad");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    at = parse(
      root, at, (hcp_parser::zeroOrMore*
                 (hcp_parser::parseOneOfChars("ab")|
                  hcp_parser::parseOneOfChars("c"))));
    xju::assert_equal(reconstruct(root), "abca");
    xju::assert_equal(at.atEnd(), false);

    try {
      hcp_parser::PR p(hcp_parser::parseOneOfChars("ab")|
                       hcp_parser::parseOneOfChars("c"));
      at = parse(root, at, p);
      xju::assert_abort();
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "Failed to parse one of chars [ab] or one of chars [c] at line 1 column 5 because\nline 1 column 5: \'d\' is not one of chars [c].", XJU_TRACED);
    }
  }
  
  try {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("abcad");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end()); 
    hcp_parser::PR p(hcp_parser::parseOneOfChars("b")|
                     hcp_parser::parseOneOfChars("c")|
                     hcp_parser::parseOneOfChars("e"));
    at = parse(root, at, p);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse one of chars [b] or one of chars [c] or one of chars [e] at line 1 column 1 because\nline 1 column 1: \'a\' is not one of chars [e].", XJU_TRACED);
  }
  
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("abcad");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    hcp_parser::PR p(hcp_parser::zeroOrMore*
                     (hcp_parser::parseOneOfChars("ab")|
                      hcp_parser::parseOneOfChars("c")|
                      hcp_parser::parseOneOfChars("d")));
    at = parse(root, at, p);
    xju::assert_equal(reconstruct(root), "abcad");
    xju::assert_equal(at.atEnd(), true);
  }
}

void test6()
{
  hcp_parser::Cache cache(new hcp_parser::CacheVal());
  hcp_parser::Options const options(false, cache);
  std::string const x("abcd");
  hcp_ast::CompositeItem root;
  hcp_parser::I at(x.begin(), x.end());
  at = parse(root, at, (hcp_parser::atLeastOne*hcp_parser::parseOneOfChars("abc")));
  xju::assert_equal(reconstruct(root), "abc");
  xju::assert_equal(at.atEnd(), false);
}

void test7()
{
  hcp_parser::Cache cache(new hcp_parser::CacheVal());
  hcp_parser::Options const options(false, cache);
  
  try {
    std::string const x("abcd");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    hcp_parser::PR p(hcp_parser::atLeastOne*hcp_parser::parseOneOfChars("e"));
    at = parse(root, at, p);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse at least one occurrance of one of chars [e] at line 1 column 1 because\nline 1 column 1: \'a\' is not one of chars [e].", XJU_TRACED);
  }
}

void test8()
{
  hcp_parser::Cache cache(new hcp_parser::CacheVal());
  hcp_parser::Options const options(false, cache);
  std::string const x("abcdefg");
  hcp_ast::CompositeItem root;
  hcp_parser::I at(x.begin(), x.end());
  at = parse(root, at, (hcp_parser::parseUntil(
                          hcp_parser::parseOneOfChars("e"))));
  xju::assert_equal(reconstruct(root), "abcd");
  xju::assert_equal(at.atEnd(), false);
}

void test9()
{
  hcp_parser::Cache cache(new hcp_parser::CacheVal());
  hcp_parser::Options const options(false, cache);
  try {
    std::string const x("abcdefg");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    hcp_parser::PR p(hcp_parser::parseUntil(
                       hcp_parser::parseOneOfChars("h")));
    at = parse(root, at, p);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse up to but not including one of chars [h] at line 1 column 1 because\nline 1 column 8: end of input.", XJU_TRACED);
  }
}

void test10()
{
  hcp_parser::Cache cache(new hcp_parser::CacheVal());
  hcp_parser::Options const options(false, cache);
  
  try {
    std::string const x("abcd");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    at = parse(
      root, at, hcp_parser::whitespaceChar);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse one of chars [\\t\\n ] at line 1 column 1 because\nline 1 column 1: \'a\' is not one of chars [\t\n ].", XJU_TRACED);
  }
}

// stringLiteral
void test11()
{
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"abcd\"");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::stringLiteral);
    xju::assert_equal(reconstruct(root), "\"abcd\"");
    xju::assert_equal(at.atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"\"");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::stringLiteral);
    xju::assert_equal(reconstruct(root), "\"\"");
    xju::assert_equal(at.atEnd(), true);
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"\\\"\"");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::stringLiteral);
    xju::assert_equal(reconstruct(root), "\"\\\"\"");
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"\\\"de\"");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::stringLiteral);
    xju::assert_equal(reconstruct(root), "\"\\\"de\"");
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"ab\\\"de\"");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::stringLiteral);
    xju::assert_equal(reconstruct(root), "\"ab\\\"de\"");
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"ab\\\"d\\ne\"");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::stringLiteral);
    xju::assert_equal(reconstruct(root), "\"ab\\\"d\\ne\"");
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"ab\\\\d\\ne\"   ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::stringLiteral);
    xju::assert_equal(reconstruct(root), "\"ab\\\\d\\ne\"   ");
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"abcde\"  \n \"fred\"  ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::stringLiteral);
    xju::assert_equal(reconstruct(root), "\"abcde\"  \n \"fred\"  ");
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"abcde\"  \n \"fred\"\n\"jock\"  ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::stringLiteral);
    xju::assert_equal(reconstruct(root), "\"abcde\"  \n \"fred\"\n\"jock\"  ");
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    // adjacent string literals combine
    std::string const x("\"abcde\" \"fred\"");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    at = parse(root, at, hcp_parser::stringLiteral);
    xju::assert_equal(reconstruct(root), "\"abcde\" \"fred\"");
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x("\"abcde");
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    at = parse(root, at, hcp_parser::stringLiteral);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    // REVISIT: error message contains irrelevant stuff, since it is the
    // first x of atLeastOneOf*x that fails to parse but the error message
    // waffles about the optional extras; for "at least one" it will only
    // ever fail on the first one
    assert_readableRepr_equal(e, "Failed to parse string literal at line 1 column 1 because\nfailed to parse at least one occurrance of one of chars [\"] then string literal characters then one of chars [\"] then zero or more occurrances of (one of chars [\\t\\n ] or comments) at line 1 column 1 because\nline 1 column 7: end of input.", XJU_TRACED);
  }
  try
  {
    std::string const x("\"abc\nde");
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    at = parse(root, at, hcp_parser::stringLiteral);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    // REVISIT: error message contains irrelevant stuff, since it is the
    // first x of atLeastOneOf*x that fails to parse but the error message
    // waffles about the optional extras; for "at least one" it will only
    // ever fail on the first one
    assert_readableRepr_equal(e, "Failed to parse string literal at line 1 column 1 because\nfailed to parse at least one occurrance of one of chars [\"] then string literal characters then one of chars [\"] then zero or more occurrances of (one of chars [\\t\\n ] or comments) at line 1 column 1 because\nline 1 column 5: \'\n\' is not one of chars [\"].", XJU_TRACED);
  }
  try
  {
    std::string const x("\"abc\\999");
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    at = parse(root, at, hcp_parser::stringLiteral);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    // REVISIT: error message contains irrelevant stuff, since it is the
    // first x of atLeastOneOf*x that fails to parse but the error message
    // waffles about the optional extras; for "at least one" it will only
    // ever fail on the first one
    assert_readableRepr_equal(e, "Failed to parse string literal at line 1 column 1 because\nfailed to parse at least one occurrance of one of chars [\"] then string literal characters then one of chars [\"] then zero or more occurrances of (one of chars [\\t\\n ] or comments) at line 1 column 1 because\nline 1 column 5: \'\\\' is not one of chars [\"].", XJU_TRACED);
  }
}

// ParseLiteral
void test12()
{
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("abcd");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::parseLiteral("abcd"));
    xju::assert_equal(reconstruct(root), "abcd");
    xju::assert_equal(at.atEnd(), true);
  }
  try {
    hcp_parser::PR p(hcp_parser::parseLiteral("abed"));
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("abcd");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, p);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse \"abed\" at line 1 column 1 because\nline 1 column 3: expected \'e\' but found \'c\'.", XJU_TRACED);
  }
}

// comments
void test13()
{
  hcp_parser::Cache cache(new hcp_parser::CacheVal());
  hcp_parser::Options const options(false, cache);
  std::string const x("// comment \n   ");
  hcp_ast::CompositeItem root;
  hcp_parser::I at(x.begin(), x.end());
  
  at = parse(
    root, at, hcp_parser::comments);
  xju::assert_equal(reconstruct(root), "// comment \n   ");
  xju::assert_equal(at.atEnd(), true);
  try {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("/ / comment \n   ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::comments);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse comments at line 1 column 1 because\nfailed to parse at least one occurrance of line comment or block comment at line 1 column 1 because\nfailed to parse block comment at line 1 column 1 because\nline 1 column 2: expected \'*\' but found \' \'.", XJU_TRACED);
  }
}

// hash
void test14()
{
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("#if\n  ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::hash);
    xju::assert_equal(reconstruct(root), "#if\n  ");
    xju::assert_equal(at.atEnd(), true);
  }
  try
  {
    std::string const x(" #if");
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::hash);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse other preprocessor directive at line 1 column 1 because\nline 1 column 1: line starts with \' \', not \'#\'.", XJU_TRACED);
  }
}


// hashInclude
void test15()
{
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("#include <string>\n  ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::hashInclude);
    xju::assert_equal(reconstruct(root), "#include <string>\n  ");
    xju::assert_equal(at.atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("#  include <string>\n  ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::hashInclude);
    xju::assert_equal(reconstruct(root), "#  include <string>\n  ");
    xju::assert_equal(at.atEnd(), true);
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("#if");
    
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::hashInclude);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse #include at line 1 column 1 because\nline 1 column 3: expected \'n\' but found \'f\'.", XJU_TRACED);
  }
  
}

// hashIncludeImpl
void test16()
{
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("#include <string> // impl \n  ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::hashIncludeImpl);
    xju::assert_equal(reconstruct(root), "#include <string> // impl \n  ");
    xju::assert_equal(at.atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("#  include <string>//impl\n  ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::hashIncludeImpl);
    xju::assert_equal(reconstruct(root), "#  include <string>//impl\n  ");
    xju::assert_equal(at.atEnd(), true);
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("#include <string> //implementation defines\n");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::hashIncludeImpl);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    // REVISIT: unhelpful
    assert_readableRepr_equal(e, "Failed to parse #include with //impl marker at line 1 column 1 because\nline 1 column 43: expected \'/\' but found \'\\n\'.", XJU_TRACED);
  }
}

// balanced
void test17()
{
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("(");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::balanced(hcp_parser::parseOneOfChars("(")));
    xju::assert_equal(reconstruct(root), "");
    xju::assert_equal((++at).atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("fred(");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::balanced(hcp_parser::parseOneOfChars("(")));
    xju::assert_equal(reconstruct(root), "fred");
    xju::assert_equal((++at).atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("fred[jock](");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::balanced(hcp_parser::parseOneOfChars("(")));
    xju::assert_equal(reconstruct(root), "fred[jock]");
    xju::assert_equal((++at).atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("{a}(b);}");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::balanced(hcp_parser::parseOneOfChars("}")));
    xju::assert_equal(reconstruct(root), "{a}(b);");
    xju::assert_equal((++at).atEnd(), true);
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("fred[\"jock(\"](");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::balanced(hcp_parser::parseOneOfChars("(")));
    xju::assert_equal(reconstruct(root), "fred[\"jock(\"]");
    xju::assert_equal((++at).atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("template<fred, jock> fred[\"jock(\"](");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::balanced(hcp_parser::parseOneOfChars("(")));
    xju::assert_equal(reconstruct(root), 
                      "template<fred, jock> fred[\"jock(\"]");
    xju::assert_equal((++at).atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("template<fred, jock> fred[// john(fred)\n\"jock(\"](");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(
      root, at, hcp_parser::balanced(hcp_parser::parseOneOfChars("(")));
    xju::assert_equal(reconstruct(root), std::string(x.begin(), x.end()-1));
    xju::assert_equal((++at).atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::PR p(hcp_parser::balanced(hcp_parser::parseOneOfChars("(")));
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("[]");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, 
               hcp_parser::balanced(hcp_parser::parseOneOfChars("(")));
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [(] at line 1 column 1 because\nline 1 column 1: end of input.", XJU_TRACED);
  }
}

// typedef_statement
void test18()
{
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("typedef std::vector<int> Ints;");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::typedef_statement);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("typedef\nstd::vector<int>\nInts;");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::typedef_statement);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  try
  {
    std::string const x("typedefine std::vector<int>\nInts;");
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::typedef_statement);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse typedef statement at line 1 column 1 because\nfailed to parse some whitespace at line 1 column 8 because\nfailed to parse at least one occurrance of one of chars [\\t\\n ] at line 1 column 8 because\nline 1 column 8: \'i\' is not one of chars [\t\n ].", XJU_TRACED);
  }
}

// using_statement
void test19()
{
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("using namespace std;");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::using_statement);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("using\nnamespace\nstd;");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::using_statement);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  try
  {
    std::string const x("usingme as something;");
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::using_statement);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse using statement at line 1 column 1 because\nfailed to parse some whitespace at line 1 column 6 because\nfailed to parse at least one occurrance of one of chars [\\t\\n ] at line 1 column 6 because\nline 1 column 6: \'m\' is not one of chars [\t\n ].", XJU_TRACED);
  }
}

// enum_def
void test20()
{
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(
      "enum Lateness {\n"
      "  LITTLE,\n"
      "  LOTS\n"
      "};\n");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::enum_def);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    hcp_ast::CompositeItem root;

    std::string const x("enumy { FRED, JOCK };");
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::enum_def);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse enum definition at line 1 column 1 because\nfailed to parse some whitespace at line 1 column 5 because\nfailed to parse at least one occurrance of one of chars [\\t\\n ] at line 1 column 5 because\nline 1 column 5: \'y\' is not one of chars [\t\n ].", XJU_TRACED);
  }
}


// function_decl
void test21()
{
  try {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(
      "template<class T>\n"
      "void fred() const throw();\n");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::function_decl);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(
      "std::ostream& operator<<(std::ostream& s, const TaskId& x) throw();");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::function_decl);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(
      "template<class T>\n"
      "void fred() const throw() { fred(); }");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::function_decl);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse function declaration at line 1 column 1 because\nline 2 column 27: \'{\' is not one of chars [;].", XJU_TRACED);
  }
}


// function_def
void test22()
{
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(
      "void fred() const throw()\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::function_def);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(
      "virtual void fred() throw() try:\n"
      "  x_(3)\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n"
      "catch(xju::Exception& e) {\n"
      "  abort();\n"
      "}");
    hcp_parser::I at(x.begin(), x.end());
    
    hcp_parser::PV const y = *hcp_parser::function_def->parse(at, options);
    xju::assert_equal(reconstruct(y.first), x);
    xju::assert_equal(y.second.atEnd(), true);
    xju::assert_equal(y.first.front()->isA<hcp_ast::FunctionDef>(), true);
    xju::assert_equal((*y.first.front()->asA<hcp_ast::FunctionDef>().items_.
                       begin())->isA<hcp_ast::FunctionQualifiers>(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(
      "void fred() throw() try\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n"
      "catch(xju::Exception& e) {\n"
      "  abort();\n"
      "}");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::function_def);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  try
  {
    hcp_parser::PR p(hcp_parser::parseLiteral("class")|
                     hcp_parser::parseLiteral("struct")|
                     hcp_parser::parseLiteral("union"));
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(
      "void fred() const throw();");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, p);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    // REVISIT: the "longest match" doesn't work that well here
    assert_readableRepr_equal(e, "Failed to parse \"class\" or \"struct\" or \"union\" at line 1 column 1 because\nline 1 column 1: expected \'u\' but found \'v\'.", XJU_TRACED);
  }
  try
  {
    std::string const x(
      "template<class T>\n"
      "void fred() const throw();");
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::function_def);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse function definition at line 1 column 1 because\nfailed to parse function implementation at line 2 column 26 because\nfailed to parse block at line 2 column 26 because\nline 2 column 26: expected \'{\' but found \';\'.", XJU_TRACED);
  }
}


// template_function_def
void test23()
{
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(
      "template<class T>\n"
      "void fred() const throw()\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::template_function_def);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(
      "template<class T>\n"
      "void fred() throw() try:\n"
      "  x_(3)\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n"
      "catch(xju::Exception& e) {\n"
      "  abort();\n"
      "}");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::function_def);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  try
  {
    std::string const x(
      "void fred() const throw()\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n");
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::template_function_def);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse template function definition at line 1 column 1 because\nfailed to parse at least one occurrance of \"template\" then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then one of chars [<] then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [>] then one of chars [>] then zero or more occurrances of (one of chars [\\t\\n ] or comments) at line 1 column 1 because\nline 1 column 1: expected \'t\' but found \'v\'.", XJU_TRACED);
  }
}


// class_def
void test24(std::vector<std::string> const& f)
{
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(xju::readFile(f[0]));
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::class_def);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
    xju::assert_equal(root.items_[0]->isA<hcp_ast::ClassDef>(), true);
    xju::assert_equal(root.items_[0]->asA<hcp_ast::ClassDef>().className_, "X");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }

  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(
      "void fred() const throw()\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    

    at = parse(root, at, hcp_parser::class_def);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    // REVISIT: unhelpful
    assert_readableRepr_equal(e, "Failed to parse class definition at line 1 column 1 because\nfailed to parse non-template class definition at line 1 column 1 because\nline 1 column 1: expected \'u\' but found \'v\'.", XJU_TRACED);
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(xju::readFile(f[1]));
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::class_def);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
}


// anonymous_namespace
void test25(std::vector<std::string> const& f)
{
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(xju::readFile(f[2]));
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::anonymous_namespace);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(
      "void fred() const throw()\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::anonymous_namespace);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse anonymous namespace at line 1 column 1 because\nline 1 column 1: expected \'n\' but found \'v\'.", XJU_TRACED);
  }
  try
  {
    std::string const x(xju::readFile(f[3]));
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::anonymous_namespace);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse anonymous namespace at line 1 column 1 because\nline 1 column 11: \'h\' is not one of chars [{].", XJU_TRACED);
  }
}


// namespace_def
void test26(std::vector<std::string> const& f)
{
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(xju::readFile(f[3]));
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::namespace_def);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(xju::readFile(f[4]));
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::namespace_def);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(
      "void fred() const throw()\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::anonymous_namespace);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse anonymous namespace at line 1 column 1 because\nline 1 column 1: expected \'n\' but found \'v\'.", XJU_TRACED);
  }
}


// file
void test27(std::vector<std::string> const& f)
{
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(xju::readFile(f[3]));
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::file);
    xju::assert_equal(at.atEnd(), true);
    xju::assert_equal(reconstruct(root), x);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  std::string const x(xju::readFile(f[3])+"xxx");
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::file);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    // REVISIT: not that helpful
    assert_readableRepr_equal(e, "Failed to parse file at line 1 column 1 because\nline 22 column 1: expected end of input, not \'x\'.", XJU_TRACED);
  }
}


void test28(std::vector<std::string> const& f)
{
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(xju::readFile(f[4]));
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::file);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(xju::readFile(f[5]));
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = parse(root, at, hcp_parser::file);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
}


int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  test3(), ++n;
  test4(), ++n;
  test5(), ++n;
  test6(), ++n;
  test7(), ++n;
  test8(), ++n;
  test9(), ++n;
  test10(), ++n;
  test11(), ++n;
  test12(), ++n;
  test13(), ++n;
  test14(), ++n;
  test15(), ++n;
  test16(), ++n;
  test17(), ++n;
  test18(), ++n;
  test19(), ++n;
  test20(), ++n;
  test21(), ++n;
  test22(), ++n;
  test23(), ++n;
  test24(std::vector<std::string>(&argv[1], &argv[argc])), ++n;
  test25(std::vector<std::string>(&argv[1], &argv[argc])), ++n;
  test26(std::vector<std::string>(&argv[1], &argv[argc])), ++n;
  test27(std::vector<std::string>(&argv[1], &argv[argc])), ++n;
  test28(std::vector<std::string>(&argv[1], &argv[argc])), ++n;

  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}
