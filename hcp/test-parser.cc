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
#include <hcp/parser.hh>
#include "xju/assert.hh"
#include <hcp/readFile.hh>

bool atLeastOneReadableReprFailed=false;

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
    atLeastOneReadableReprFailed=true;
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
  hcp_parser::Options const options(false, cache, false);
  std::string const x("ab");
  hcp_parser::PV const y(*hcp_parser::parseAnyChar()->parse_(
    hcp_parser::I(x.begin(), x.end()),
    options));
  xju::assert_equal(reconstruct(y.first), "a");
  xju::assert_equal(y.second.x_, x.begin()+1);

  hcp_parser::PV const y2(*hcp_parser::parseAnyChar()->parse_(
    y.second,
    options));
  xju::assert_equal(reconstruct(y2.first), "b");
  xju::assert_equal(y2.second.x_, x.begin()+2);
  
  hcp_parser::ParseResult const r(
    hcp_parser::parseAnyChar()->parse_(
      y2.second,
      options));
  xju::assert_equal(r.failed(), true);
  xju::assert_equal(readableRepr(r.e()), "line 1 column 3: end of input");

  hcp_parser::I at(x.begin(), x.end());
  auto const r2{parse(at, hcp_parser::parseAnyChar())};
  xju::assert_equal(r2.second.x_, x.begin()+1);
  xju::assert_equal(reconstruct(r2.first), "a");
  auto const r3{parse(r2.second, hcp_parser::parseAnyChar())};

  try {
    parse(r3.second,hcp_parser::parseAnyChar());
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse any char at line 1 column 3 because\nline 1 column 3: end of input.", XJU_TRACED);
  }

}

void test2()
{
  std::string const x("ab");
  hcp_parser::I at(x.begin(), x.end());
  auto const r{
    parseString(x.begin(),x.end(),
                hcp_parser::zeroOrMore()*hcp_parser::parseAnyChar())};
  xju::assert_equal(reconstruct(r), "ab");
}

void test3()
{
  std::string const x("ab");
  auto const r{
    parseString(x.begin(),x.end(),
                (hcp_parser::parseAnyChar()+
                 hcp_parser::parseAnyChar()))};
  xju::assert_equal(reconstruct(r), "ab");

  try {
    hcp_parser::PR p(hcp_parser::parseAnyChar()+
                     hcp_parser::parseAnyChar()+
                     hcp_parser::parseAnyChar());
    auto const r{parseString(x.begin(),x.end(), p)};
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse any char at line 1 column 3 because\nline 1 column 3: end of input.", XJU_TRACED);
 }
}

void test4()
{
  std::string const x("abcd");
  hcp_parser::I at(x.begin(), x.end());
  auto const r{
    parse(at, (hcp_parser::zeroOrMore()*hcp_parser::parseOneOfChars("abc")))};
  xju::assert_equal(reconstruct(r.first), "abc");
  xju::assert_equal(r.second.atEnd(), false);

  try {
    hcp_parser::PR p(hcp_parser::parseOneOfChars("abc"));
    parse(r.second, p);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    xju::assert_equal(readableRepr(e), "Failed to parse one of chars \"abc\" at line 1 column 4 because\nline 1 column 4: 'd' is not one of chars \"abc\".");
  }
}

void test5()
{
  {
    std::string const x("abcad");
    hcp_parser::I at(x.begin(), x.end());
    auto const r{
      parse(
        at, (hcp_parser::zeroOrMore()*
             (hcp_parser::parseOneOfChars("ab")|
              hcp_parser::parseOneOfChars("c"))))};
    xju::assert_equal(reconstruct(r.first), "abca");
    xju::assert_equal(at.atEnd(), false);

    try {
      hcp_parser::PR p(hcp_parser::parseOneOfChars("ab")|
                       hcp_parser::parseOneOfChars("c"));
      parse(r.second, p);
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "Failed to parse one of chars \"c\" at line 1 column 5 because\nline 1 column 5: \'d\' is not one of chars \"c\".", XJU_TRACED);
    }
  }
  
  try {
    std::string const x("abcad");
    hcp_parser::I at(x.begin(), x.end()); 
    hcp_parser::PR p(hcp_parser::parseOneOfChars("b")|
                     hcp_parser::parseOneOfChars("c")|
                     hcp_parser::parseOneOfChars("e"));
    auto const r{parse(at, p)};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse one of chars \"e\" at line 1 column 1 because\nline 1 column 1: \'a\' is not one of chars \"e\".", XJU_TRACED);
  }
  
  {
    std::string const x("abcad");
    hcp_parser::PR p(hcp_parser::zeroOrMore()*
                     (hcp_parser::parseOneOfChars("ab")|
                      hcp_parser::parseOneOfChars("c")|
                      hcp_parser::parseOneOfChars("d")));
    auto const root{parseString(x.begin(),x.end(), p)};
    xju::assert_equal(reconstruct(root), "abcad");
  }
}

void test6()
{
  std::string const x("abcd");
  hcp_parser::I at(x.begin(), x.end());
  auto const r{
    parse(at, (hcp_parser::atLeastOne(hcp_parser::parseOneOfChars("abc"))))};
  xju::assert_equal(reconstruct(r.first), "abc");
  xju::assert_equal(r.second.atEnd(), false);
}

void test7()
{
  try {
    std::string const x("abcd");
    hcp_parser::I at(x.begin(), x.end());
    hcp_parser::PR p(hcp_parser::atLeastOne(hcp_parser::parseOneOfChars("e")));
    parse(at, p);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse at least one occurrance of one of chars \"e\" at line 1 column 1 because\nfailed to parse one of chars \"e\" at line 1 column 1 because\nline 1 column 1: \'a\' is not one of chars \"e\".", XJU_TRACED);
  }
}

void test8()
{
  std::string const x("abcdefg");
  hcp_parser::I at(x.begin(), x.end());
  auto const r{
    parse(at, hcp_parser::parseUntil(
            hcp_parser::parseOneOfChars("e")))};
  xju::assert_equal(reconstruct(r.first), "abcd");
  xju::assert_equal(r.second.atEnd(), false);

  {
    std::string const x("abcdefg");
    hcp_parser::I at(x.begin(), x.end());
    auto const r{
      parse(at, (hcp_parser::parseUntil(
                   hcp_parser::parseOneOfChars("abcd"),
                   hcp_parser::parseOneOfChars("e"))))};
    xju::assert_equal(reconstruct(r.first), "abcd");
    xju::assert_equal(r.second.atEnd(), false);
  }
}

void test9()
{
  try {
    std::string const x("abcdefg");
    hcp_parser::I at(x.begin(), x.end());
    hcp_parser::PR p(hcp_parser::parseUntil(
                       hcp_parser::parseOneOfChars("h")));
    parse(at, p);
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse up to but not including one of chars \"h\" at line 1 column 1 because\nline 1 column 8: end of input.", XJU_TRACED);
  }
  {
    try {
      std::string const x("fredjockalicealan");
      hcp_parser::I at(x.begin(), x.end());
      hcp_parser::PR p(hcp_parser::parseUntil(
                         hcp_parser::parseLiteral("fred")|
                         hcp_parser::parseLiteral("jock")|
                         hcp_parser::parseLiteral("alice"),
                         hcp_parser::parseLiteral("peter")));
      parse(at, p);
      xju::assert_abort();
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "Failed to parse \"alice\" at line 1 column 14 because\nline 1 column 16: expected \'i\' but found \'a\'.", XJU_TRACED);
    }
  }
}

void test10()
{
  try {
    std::string const x("abcd");
    hcp_parser::I at(x.begin(), x.end());
    parse(
      at, hcp_parser::whitespaceChar());
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse one of chars \"\\t\\n\\r \" at line 1 column 1 because\nline 1 column 1: \'a\' is not one of chars \"\\t\\n\\r \".", XJU_TRACED);
  }
}

// stringLiteral
void test11()
{
  {
    std::string const x("\"abcd\"");
    auto const r{parseString(x.begin(),x.end(), hcp_parser::stringLiteral())};
    xju::assert_equal(reconstruct(r), "\"abcd\"");
  }
  {
    std::string const x("\"\"");
    auto const r{parseString(x.begin(),x.end(), hcp_parser::stringLiteral())};
    xju::assert_equal(reconstruct(r), "\"\"");
  }
  try
  {
    std::string const x("\"\\\"\"");
    auto const r{parseString(x.begin(),x.end(), hcp_parser::stringLiteral())};
    xju::assert_equal(reconstruct(r), "\"\\\"\"");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x("\"\\\"de\"");
    auto const r{parseString(x.begin(),x.end(), hcp_parser::stringLiteral())};
    xju::assert_equal(reconstruct(r), "\"\\\"de\"");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x("\"ab\\\"de\"");
    auto const r{parseString(x.begin(),x.end(), hcp_parser::stringLiteral())};
    xju::assert_equal(reconstruct(r), "\"ab\\\"de\"");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x("\"ab\\\"d\\ne\"");
    auto const r{parseString(x.begin(),x.end(), hcp_parser::stringLiteral())};
    xju::assert_equal(reconstruct(r), "\"ab\\\"d\\ne\"");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x("\"ab\\\\d\\ne\"   ");
    auto const r{parseString(x.begin(),x.end(), hcp_parser::stringLiteral())};
    xju::assert_equal(reconstruct(r), "\"ab\\\\d\\ne\"   ");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x("\"abcde\"  \n \"fred\"  ");
    auto const r{parseString(x.begin(),x.end(), hcp_parser::stringLiteral())};
    xju::assert_equal(reconstruct(r), "\"abcde\"  \n \"fred\"  ");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x("\"abcde\"  \n \"fred\"\n\"jock\"  ");
    auto const r{parseString(x.begin(),x.end(), hcp_parser::stringLiteral())};
    xju::assert_equal(reconstruct(r), "\"abcde\"  \n \"fred\"\n\"jock\"  ");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    // adjacent string literals combine
    std::string const x("\"abcde\" \"fred\"");
    auto const r{parseString(x.begin(),x.end(), hcp_parser::stringLiteral())};
    xju::assert_equal(reconstruct(r), "\"abcde\" \"fred\"");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x("\"abcde");
    auto const r{parseString(x.begin(),x.end(), hcp_parser::stringLiteral())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    // REVISIT: error message contains irrelevant stuff, since it is the
    // first x of atLeastOneOf*x that fails to parse but the error message
    // waffles about the optional extras; for "at least one" it will only
    // ever fail on the first one
    assert_readableRepr_equal(e, "Failed to parse string literal at line 1 column 1 because\nfailed to parse at least one occurrance of one of chars \"\\\"\" then string literal characters then one of chars \"\\\"\" then optional whitespace at line 1 column 1 because\nfailed to parse string literal characters at line 1 column 2 because\nfailed to parse \"\\\" at line 1 column 7 because\nline 1 column 7: end of input.", XJU_TRACED);
  }
  try
  {
    std::string const x("\"abc\nde");
    auto const r{parseString(x.begin(),x.end(), hcp_parser::stringLiteral())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    // REVISIT: error message contains irrelevant stuff, since it is the
    // first x of atLeastOneOf*x that fails to parse but the error message
    // waffles about the optional extras; for "at least one" it will only
    // ever fail on the first one
    assert_readableRepr_equal(e, "Failed to parse string literal at line 1 column 1 because\nfailed to parse at least one occurrance of one of chars \"\\\"\" then string literal characters then one of chars \"\\\"\" then optional whitespace at line 1 column 1 because\nfailed to parse string literal characters at line 1 column 2 because\nfailed to parse \"\\\" at line 1 column 5 because\nline 1 column 5: expected \'\\\\\' but found \'\\n\'.", XJU_TRACED);
  }
  try
  {
    std::string const x("\"abc\\999");
    auto const r{parseString(x.begin(),x.end(), hcp_parser::stringLiteral())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    // REVISIT: error message contains irrelevant stuff, since it is the
    // first x of atLeastOneOf*x that fails to parse but the error message
    // waffles about the optional extras; for "at least one" it will only
    // ever fail on the first one
    assert_readableRepr_equal(e, "Failed to parse string literal at line 1 column 1 because\nfailed to parse at least one occurrance of one of chars \"\\\"\" then string literal characters then one of chars \"\\\"\" then optional whitespace at line 1 column 1 because\nfailed to parse string literal characters at line 1 column 2 because\nfailed to parse \"x\" at line 1 column 6 because\nline 1 column 6: expected \'x\' but found \'9\'.", XJU_TRACED);
  }
}

// ParseLiteral
void test12()
{
  {
    std::string const x("abcd");
    auto const r{
      parseString(x.begin(),x.end(), hcp_parser::parseLiteral("abcd"))};
    xju::assert_equal(reconstruct(r), "abcd");
  }
  try {
    hcp_parser::PR p(hcp_parser::parseLiteral("abed"));
    std::string const x("abcd");
    auto const r{parseString(x.begin(),x.end(), p)};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse \"abed\" at line 1 column 1 because\nline 1 column 3: expected \'e\' but found \'c\'.", XJU_TRACED);
  }
}

// comments
void test13()
{
  std::string const x("// comment \n   ");
  auto const r{parseString(x.begin(),x.end(), hcp_parser::comments())};
  xju::assert_equal(reconstruct(r), "// comment \n   ");
  try {
    std::string const x("/ / comment \n   ");
    parseString(x.begin(),x.end(), hcp_parser::comments());
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse comments at line 1 column 1 because\nfailed to parse at least one occurrance of line comment or block comment at line 1 column 1 because\nfailed to parse block comment at line 1 column 1 because\nfailed to parse \"/*\" at line 1 column 1 because\nline 1 column 2: expected \'*\' but found \' \'.", XJU_TRACED);
  }
}

// hash
void test14()
{
  {
    std::string const x("#if\n  ");
    auto const root{parseString(x.begin(),x.end(),hcp_parser::hash())};
    xju::assert_equal(reconstruct(root), "#if\n  ");
  }
  {
    std::string const x("#if\\\n  fred\n ");
    auto const root{parseString(x.begin(),x.end(), hcp_parser::hash())};
    xju::assert_equal(reconstruct(root), x);
  }
  try
  {
    std::string const x(" #if");
    auto const root{parseString(x.begin(),x.end(), hcp_parser::hash())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse other preprocessor directive at line 1 column 1 because\nfailed to parse \'#\' at start of line at line 1 column 1 because\nline 1 column 1: line starts with \' \', not \'#\'.", XJU_TRACED);
  }
}


// hashInclude
void test15()
{
  {
    std::string const x("#include <string>\n  ");
    auto const root{parseString(x.begin(),x.end(), hcp_parser::hashInclude())};
    xju::assert_equal(reconstruct(root), "#include <string>\n  ");
    auto k(hcp_ast::findOnlyChildOfType<hcp_ast::TargetOfHashInclude>(root));
    xju::assert_equal(reconstruct(k),"string");
  }
  {
    std::string const x("#  include \"string\"\n  ");
    auto const root{parseString(x.begin(),x.end(), hcp_parser::hashInclude())};
    xju::assert_equal(reconstruct(root), "#  include \"string\"\n  ");
    auto k(hcp_ast::findOnlyChildOfType<hcp_ast::TargetOfHashInclude>(root));
    xju::assert_equal(reconstruct(k),"string");
  }
  try
  {
    std::string const x("#if");
    auto const root{parseString(x.begin(),x.end(), hcp_parser::hashInclude())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse #include at line 1 column 1 because\nfailed to parse \"include\" at line 1 column 2 because\nline 1 column 3: expected \'n\' but found \'f\'.", XJU_TRACED);
  }
  
}

// hashIncludeImpl
void test16()
{
  {
    std::string const x("#include <string> // impl \n  ");
    auto const root{
      parseString(x.begin(),x.end(), hcp_parser::hashIncludeImpl())};
    xju::assert_equal(reconstruct(root), "#include <string> // impl \n  ");
  }
  {
    std::string const x("#  include <string>//impl\n  ");
    auto const root{
      parseString(x.begin(),x.end(), hcp_parser::hashIncludeImpl())};
    xju::assert_equal(reconstruct(root), "#  include <string>//impl\n  ");
  }
  try
  {
    std::string const x("#include <string> //implementation defines\n");
    auto const root{
      parseString(x.begin(),x.end(), hcp_parser::hashIncludeImpl())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    // REVISIT: unhelpful
    assert_readableRepr_equal(e, "Failed to parse #include with //impl marker at line 1 column 1 because\nfailed to parse \"//\" at line 1 column 43 because\nline 1 column 43: expected \'/\' but found \'\\n\'.", XJU_TRACED);
  }
}

// balanced
void test17()
{
  {
    std::string const x("(");
    auto const root{parseString(
      x.begin(),x.end(),
      hcp_parser::balanced(hcp_parser::parseOneOfChars("("))+
      hcp_parser::parseLiteral("("))};
  }
  {
    std::string const x("fred(");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::balanced(hcp_parser::parseOneOfChars("("))+
        hcp_parser::parseLiteral("("))};
  }
  {
    std::string const x("fred[jock](");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::balanced(hcp_parser::parseOneOfChars("("))+
        hcp_parser::parseLiteral("("))};
  }
  {
    std::string const x("{a}(b);}");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::balanced(hcp_parser::parseOneOfChars("}"))+
        hcp_parser::parseLiteral("}"))};
  }
  try
  {
    std::string const x("fred[\"jock(\"](");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::balanced(hcp_parser::parseOneOfChars("("))+
        hcp_parser::parseLiteral("("))};
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x("(*i) != '\"')");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::balanced(hcp_parser::parseOneOfChars(")"))+
        hcp_parser::parseLiteral(")"))};
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x("template<fred, jock> fred[\"jock(\"](");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::balanced(hcp_parser::parseOneOfChars("("))+
        hcp_parser::parseLiteral("("))};
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x("template<fred, jock> fred[// john(fred)\n\"jock(\"](");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::balanced(hcp_parser::parseOneOfChars("("))+
        hcp_parser::parseLiteral("("))};
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x("[]");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::balanced(hcp_parser::parseOneOfChars("(")))};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars \"(\" at line 1 column 1 because\nline 1 column 3: end of input.", XJU_TRACED);
  }

  //type_ref
  {
    std::string const x("I<3>");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::type_ref())};
      xju::assert_equal(reconstruct(root), x);
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
  {
    std::string const x("const& ");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::zeroOrMore()*hcp_parser::type_qual())};
      xju::assert_equal(reconstruct(root), x);
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
  {
    std::string const x("X const& ");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::type_ref())};
      xju::assert_equal(reconstruct(root), x);
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
  {
    std::string const x("const T&");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::type_ref())};
      xju::assert_equal(reconstruct(root), x);
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
  {
    std::string const x("operator const T&");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::conversion_operator_name())};
      xju::assert_equal(reconstruct(root), x);
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
}

// typedef_statement
void test18()
{
  {
    std::string const x("typedef std::vector<int> Ints;");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::typedef_statement())};
    xju::assert_equal(reconstruct(root), x);
  }
  {
    std::string const x("typedef void (T::*Method)(P);");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::typedef_statement())};
    xju::assert_equal(reconstruct(root), x);
    auto k(hcp_ast::findOnlyChildOfType<hcp_ast::Typedef>(root));
    auto l(hcp_ast::findOnlyChildOfType<hcp_ast::DefinedType>(k));
    xju::assert_equal(reconstruct(l),"Method");
  }
  {
    std::string const x("typedef\nstd::vector<int>\nInts;");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::typedef_statement())};
    xju::assert_equal(reconstruct(root), x);
  }
  try
  {
    std::string const x("typedefine std::vector<int>\nInts;");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::typedef_statement())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse typedef statement at line 1 column 1 because\nfailed to parse !one of chars \'a\'..\'z\' or one of chars \'A\'..\'Z\' or one of chars \'0\'..\'9\' or one of chars \"_\" at line 1 column 8 because\nline 1 column 8: expected parse failure.", XJU_TRACED);
  }
}

// using_statement
void test19()
{
  {
    std::string const x("using namespace std;");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::using_statement())};
    xju::assert_equal(reconstruct(root), x);
  }
  {
    std::string const x("using\nnamespace\nstd;");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::using_statement())};
    xju::assert_equal(reconstruct(root), x);
  }
  try
  {
    std::string const x("usingme as something;");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::using_statement())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse using statement at line 1 column 1 because\nfailed to parse !one of chars \'a\'..\'z\' or one of chars \'A\'..\'Z\' or one of chars \'0\'..\'9\' or one of chars \"_\" at line 1 column 6 because\nline 1 column 6: expected parse failure.", XJU_TRACED);
  }
}

// enum_def and scoped_enum_def
void test20()
{
  {
    std::string const x(
      "enum Lateness {\n"
      "  LITTLE,\n"
      "  LOTS\n"
      "};\n");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::enum_def())};
    xju::assert_equal(reconstruct(root), x);
  }
  try
  {
    std::string const x("enumy { FRED, JOCK };");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::enum_def())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse enum definition at line 1 column 1 because\nfailed to parse !one of chars \'a\'..\'z\' or one of chars \'A\'..\'Z\' or one of chars \'0\'..\'9\' or one of chars \"_\" at line 1 column 5 because\nline 1 column 5: expected parse failure.", XJU_TRACED);
  }
  {
    std::string const x(
      "enum class Lateness {\n"
      "  LITTLE,\n"
      "  LOTS\n"
      "};\n");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::scoped_enum_def())};
    xju::assert_equal(reconstruct(root), x);
  }
  try
  {
    std::string const x("enum classy Lateness { FRED, JOCK };");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::enum_def())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse enum definition at line 1 column 1 because\nfailed to parse \"{\" at line 1 column 13 because\nline 1 column 13: expected \'{\' but found \'L\'.", XJU_TRACED);
  }
}


// function_decl
void test21()
{
  try {
    std::string const x("void fred();");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::function_proto()+hcp_parser::parseLiteral(";"))};
    xju::assert_equal(reconstruct(root), "void fred();");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try {
    std::string const x(
      "operator const T&();");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::function_proto()+hcp_parser::parseLiteral(";"))};
    xju::assert_equal(reconstruct(root), "operator const T&();");
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try {
    std::string const x(
      "std::string x(char const* const x[]=\"fred\"\n"
              "\"doesn't\"\n"
              "\"lie\");");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::function_proto()+hcp_parser::parseLiteral(";"))};
    xju::assert_equal(reconstruct(root), std::string(x.begin(),x.end()));
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try {
    std::string const x(
      "template<class T>\n"
      "void fred() const throw();\n");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::function_decl())};
    xju::assert_equal(reconstruct(root), x);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try {
    std::string const x(
      "void fred() const noexcept;\n");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::function_decl())};
    xju::assert_equal(reconstruct(root), x);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try {
    std::string const x(
      "void fred() const noexcept(false);\n");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::function_decl())};
    xju::assert_equal(reconstruct(root), x);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try {
    std::string const x(
      "std::ostream& operator<<(std::ostream& s, const TaskId& x) throw();");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::function_decl())};
    xju::assert_equal(reconstruct(root), x);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x(
      "template<class T>\n"
      "void fred() const throw() { fred(); }");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::function_decl())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse function declaration at line 1 column 1 because\nfailed to parse one of chars \";\" at line 2 column 27 because\nline 2 column 27: \'{\' is not one of chars \";\".", XJU_TRACED);
  }
  try {
    std::string const x(
      "operator const T&() const throw();");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::function_decl())};
    xju::assert_equal(reconstruct(root), x);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  
}


// function_def
void test22()
{
  {
    std::string const x(
      "void fred() const throw()\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::function_def())};
    xju::assert_equal(reconstruct(root), x);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache, false);
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
    
    hcp_parser::PV const y = *hcp_parser::function_def()->parse(at, options);
    xju::assert_equal(reconstruct(y.first), x);
    xju::assert_equal(y.second.atEnd(), true);
    xju::assert_equal(y.first.front()->isA<hcp_ast::FunctionDef>(), true);
    xju::assert_equal((*y.first.front()->asA<hcp_ast::FunctionDef>().items().
                       begin())->isA<hcp_ast::FunctionQualifiers>(), true);
  }
  {
    std::string const x(
      "void fred() throw() try\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n"
      "catch(xju::Exception& e) {\n"
      "  abort();\n"
      "}");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::function_def())};
    xju::assert_equal(reconstruct(root), x);
  }
  {
    std::string const x(
      "template<>\n"
      "int X::of<int>()\n"
      "{\n"
      "  return 0;\n"
      "}\n");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::function_def())};
    xju::assert_equal(reconstruct(root), x);
  }
  try
  {
    hcp_parser::PR p(hcp_parser::parseLiteral("class")|
                     hcp_parser::parseLiteral("struct")|
                     hcp_parser::parseLiteral("union"));
    std::string const x(
      "void fred() const throw();");
    auto const root{parseString(
        x.begin(),x.end(),
        p)};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    // REVISIT: the "longest match" doesn't work that well here
    assert_readableRepr_equal(e, "Failed to parse \"union\" at line 1 column 1 because\nline 1 column 1: expected \'u\' but found \'v\'.", XJU_TRACED);
  }
  try
  {
    std::string const x(
      "void fred() const throw();");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::function_def())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse non-template function definition at line 1 column 1 because\nfailed to parse function implementation at line 1 column 26 because\nfailed to parse block at line 1 column 26 because\nfailed to parse block open at line 1 column 26 because\nfailed to parse \"{\" at line 1 column 26 because\nline 1 column 26: expected \'{\' but found \';\'.", XJU_TRACED);
  }
}


// template_function_def
void test23()
{
  {
    std::string const x(
      "template<class T>\n"
      "void fred() const throw()\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::template_function_def())};
  }
  {
    std::string const x(
      "template<>\n"
      "void fred() throw() try:\n"
      "  x_(3)\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n"
      "catch(xju::Exception& e) {\n"
      "  abort();\n"
      "}");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::function_def())};
    xju::assert_equal(reconstruct(root), x);
  }
  try
  {
    std::string const x(
      "void fred() const throw()\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::template_function_def())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse template function definition at line 1 column 1 because\nfailed to parse at least one occurrance of template preamble at line 1 column 1 because\nfailed to parse template preamble at line 1 column 1 because\nfailed to parse \"template\" at line 1 column 1 because\nline 1 column 1: expected \'t\' but found \'v\'.", XJU_TRACED);
  }
}


// class_def
void test24(std::vector<std::string> const& f)
{
  try
  {
    std::string const x(hcp::readFile(xju::path::split(f[0])));
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::class_def())};
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(root.items()[0]->isA<hcp_ast::ClassDef>(), true);
    xju::assert_equal(root.items()[0]->asA<hcp_ast::ClassDef>().className_, "X");
    xju::assert_equal(root.items()[0]->asA<hcp_ast::ClassDef>().isTemplateSpecialisation_, false);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }

  try
  {
    std::string const x(
      "void fred() const throw()\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::class_def())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    // REVISIT: unhelpful
    assert_readableRepr_equal(e, "Failed to parse non-template class definition at line 1 column 1 because\nfailed to parse \"union\" at line 1 column 1 because\nline 1 column 1: expected \'u\' but found \'v\'.", XJU_TRACED);
  }
  try
  {
    std::string const x(hcp::readFile(xju::path::split(f[1])));
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::class_def())};
    xju::assert_equal(reconstruct(root), x);
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
    std::string const x(hcp::readFile(xju::path::split(f[2])));
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::anonymous_namespace())};
    xju::assert_equal(reconstruct(root), x);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x(
      "void fred() const throw()\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::anonymous_namespace())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse anonymous namespace at line 1 column 1 because\nfailed to parse \"namespace\" at line 1 column 1 because\nline 1 column 1: expected \'n\' but found \'v\'.", XJU_TRACED);
  }
  try
  {
    std::string const x(hcp::readFile(xju::path::split(f[3])));
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::anonymous_namespace())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse anonymous namespace at line 1 column 1 because\nfailed to parse anon namespace open at line 1 column 11 because\nfailed to parse one of chars \"{\" at line 1 column 11 because\nline 1 column 11: 'h' is not one of chars \"{\".", XJU_TRACED);
  }
}


// namespace_def
void test26(std::vector<std::string> const& f)
{
  try
  {
    std::string const x(hcp::readFile(xju::path::split(f[3])));
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::namespace_def())};
    xju::assert_equal(reconstruct(root), x);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x(hcp::readFile(xju::path::split(f[4])));
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::namespace_def())};
    xju::assert_equal(reconstruct(root), x);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x(
      "void fred() const throw()\n"
      "{\n"
      "  implemented();\n"
      "  return x;\n"
      "}\n");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::anonymous_namespace())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "Failed to parse anonymous namespace at line 1 column 1 because\nfailed to parse \"namespace\" at line 1 column 1 because\nline 1 column 1: expected \'n\' but found \'v\'.", XJU_TRACED);
  }
}


// file
void test27(std::vector<std::string> const& f)
{
  try
  {
    std::string const x(hcp::readFile(xju::path::split(f[3])));
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::file())};
    xju::assert_equal(reconstruct(root), x);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x(hcp::readFile(xju::path::split(f[3]))+"xxx");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::file())};
    xju::assert_abort();
  }
  catch(xju::Exception const& e) {
    // REVISIT: not that helpful
    assert_readableRepr_equal(e, "Failed to parse file at line 1 column 1 because\nfailed to parse global variable definition at line 22 column 1 because\nfailed to parse function pointer var at line 22 column 1 because\nfailed to parse \"(\" at line 22 column 4 because\nline 22 column 4: end of input.", XJU_TRACED);
  }
}


void test28(std::vector<std::string> const& f)
{
  try
  {
    std::string const x(hcp::readFile(xju::path::split(f[4])));
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::file())};
    xju::assert_equal(reconstruct(root), x);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    std::string const x(hcp::readFile(xju::path::split(f[5])));
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::file())};
    xju::assert_equal(reconstruct(root), x);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
}

void test29(std::vector<std::string> const& f)
{
  try
  {
    std::string const x(
      "template<>\n"
      "class X<int> : std::vector<int>\n"
      "{\n"
      "public:\n"
      "  int f() const throw()\n"
      "  {\n"
      "    return 1;\n"
      "  }\n"
      "};\n");
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::class_def())};
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(root.items()[0]->asA<hcp_ast::ClassDef>().className_, "X<int>");
    xju::assert_equal(root.items()[0]->asA<hcp_ast::ClassDef>().isTemplateSpecialisation_, true);
  }
  catch(xju::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
}

void test30()
{
  std::string const x("static const char* const _user_exns[] = {\n"
                      "  0\n"
                      "};  ");

  try {
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::static_var_def())};
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(root.items()[0]->isA<hcp_ast::StaticVarDef>(),true);
    auto const y(
      hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(
        root.items()[0]->asA<hcp_ast::StaticVarDef>()));
    xju::assert_equal(reconstruct(y),"_user_exns");
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "", XJU_TRACED);
    xju::assert_equal(true,false);
  }
}

void test31()
{
 {
   std::string const x("static const int* x = 3;");
   
   try {
     auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::static_var_def())};
     xju::assert_equal(reconstruct(root), x);

     auto d(hcp_ast::findOnlyChildOfType<hcp_ast::StaticVarDef>(root));
     hcp_ast::asA_<hcp_ast::StaticVarDef>(*root.items().begin());
     auto n(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(d));
     xju::assert_equal(reconstruct(n),"x");
     auto i(hcp_ast::findOnlyChildOfType<hcp_ast::VarInitialiser>(d));
     xju::assert_equal(reconstruct(i),"= 3");
   }
   catch(xju::Exception const& e) {
     assert_readableRepr_equal(e, "", XJU_TRACED);
     xju::assert_equal(true,false);
   }
 }
 
 {
   std::string const x("static X y=Z(3);");
   
   try {
     auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::static_var_def())};
     xju::assert_equal(reconstruct(root), x);

     auto d(hcp_ast::findOnlyChildOfType<hcp_ast::StaticVarDef>(root));
     hcp_ast::asA_<hcp_ast::StaticVarDef>(*root.items().begin());
     auto n(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(d));
     xju::assert_equal(reconstruct(n),"y");
     auto i(hcp_ast::findOnlyChildOfType<hcp_ast::VarInitialiser>(d));
     xju::assert_equal(reconstruct(i),"=Z(3)");

   }
   catch(xju::Exception const& e) {
     assert_readableRepr_equal(e, "", XJU_TRACED);
     xju::assert_equal(true,false);
   }
 }
 {
   std::string const x("static X y{Z(3)};");
   
   try {
     auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::static_var_def())};
     xju::assert_equal(reconstruct(root), x);

     auto d(hcp_ast::findOnlyChildOfType<hcp_ast::StaticVarDef>(root));
     hcp_ast::asA_<hcp_ast::StaticVarDef>(*root.items().begin());
     auto n(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(d));
     xju::assert_equal(reconstruct(n),"y");
     auto i(hcp_ast::findOnlyChildOfType<hcp_ast::VarInitialiser>(d));
     xju::assert_equal(reconstruct(i),"{Z(3)}");
   }
   catch(xju::Exception const& e) {
     assert_readableRepr_equal(e, "", XJU_TRACED);
     xju::assert_equal(true,false);
   }
 }
 {
   std::string const x("X y=Z(3);");
   
   try {
     auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::global_var_def())};
     xju::assert_equal(reconstruct(root), x);
   }
   catch(xju::Exception const& e) {
     assert_readableRepr_equal(e, "", XJU_TRACED);
     xju::assert_equal(true,false);
   }
 }
 {
   std::string const x("std::chrono::seconds const x{a};");
   
   try {
     auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::global_var_def())};
     xju::assert_equal(reconstruct(root), x);
   }
   catch(xju::Exception const& e) {
     assert_readableRepr_equal(e, "", XJU_TRACED);
     xju::assert_equal(true,false);
   }
 }
 {
   std::string const x("typename std::list<item_type>::iterator m_i;");
   try {
     auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::global_var_def())};
     xju::assert_equal(reconstruct(root), x);
   }
   catch(xju::Exception const& e) {
     assert_readableRepr_equal(e, "", XJU_TRACED);
     xju::assert_equal(true,false);
   }
 }
 {
   std::string const x("static X (*y)(int x, I<p> const& p)=pq;");
   try {
     auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::static_var_def())};
     xju::assert_equal(reconstruct(root), x);

     auto d(hcp_ast::findOnlyChildOfType<hcp_ast::StaticVarDef>(root));
     hcp_ast::asA_<hcp_ast::StaticVarDef>(*root.items().begin());
     auto n(hcp_ast::findChildrenOfType<hcp_ast::VarName>(d));
     xju::assert_equal(reconstruct(n[0]),"y");
     xju::assert_equal(reconstruct(n[1]),"x");
     xju::assert_equal(reconstruct(n[2]),"p");
     xju::assert_equal(n.size(),3);
     auto i(hcp_ast::findOnlyChildOfType<hcp_ast::VarInitialiser>(d));
     xju::assert_equal(reconstruct(i),"=pq");
   }
   catch(xju::Exception const& e) {
     assert_readableRepr_equal(e, "", XJU_TRACED);
     xju::assert_equal(true,false);
   }
 }
}


void test32()
{
  {
    std::string const x("static pof me;");
    
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::static_var_def())};
      xju::assert_equal(reconstruct(root), x);

     auto d(hcp_ast::findOnlyChildOfType<hcp_ast::StaticVarDef>(root));
     hcp_ast::asA_<hcp_ast::StaticVarDef>(*root.items().begin());
     auto n(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(d));
     xju::assert_equal(reconstruct(n),"me");
     auto i(hcp_ast::findChildrenOfType<hcp_ast::VarInitialiser>(d,1));
     xju::assert_equal(i.size(),0);
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
}

void test33()
{
  std::string const x("X<int>::Y");
  
  try {
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::type_name())};
    xju::assert_equal(reconstruct(root), x);
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "", XJU_TRACED);
    xju::assert_equal(true,false);
  }
}
  
void test34()
{
  std::string const x(
    "operator int() const throw()"
    "{"
    "  return x_;"
    "}");

  try {
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::function_def())};
    xju::assert_equal(reconstruct(root), x);
    std::vector<hcp_ast::IR>::const_iterator j(
      std::find_if(root.items()[0]->asA<hcp_ast::FunctionDef>().items().begin(),
                   root.items()[0]->asA<hcp_ast::FunctionDef>().items().end(),
                   hcp_ast::isA_<hcp_ast::FunctionName>));
    xju::assert_not_equal(
      j, 
      root.items()[0]->asA<hcp_ast::FunctionDef>().items().end());
    xju::assert_equal(reconstruct(*j), "operator int");
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "", XJU_TRACED);
    xju::assert_equal(true,false);
  }
}

void test35()
{
  std::string const x("(x)");
  try {
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::bracketed(hcp_parser::parseLiteral("x")))};
    xju::assert_equal(reconstruct(root), x);
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "", XJU_TRACED);
    xju::assert_equal(true,false);
  }
  
}

void test36()
{
  std::string const x("( /*fred*/x)");
  
  try {
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::bracketed(hcp_parser::parseLiteral("x")))};
    xju::assert_equal(reconstruct(root), x);
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "", XJU_TRACED);
    xju::assert_equal(true,false);
  }
  
}

void test37()
{
  {
    std::string const x("const ");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::cv())};
      xju::assert_equal(reconstruct(root), x);
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
  
  {
    std::string const x("const volatile ");
    
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::cv())};
      xju::assert_equal(reconstruct(root), x);
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
  
  {
    std::string const x("volatile const");
    
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::cv())};
      xju::assert_equal(reconstruct(root), x);
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
  
  {
    std::string const x("volatile ");
    
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::cv())};
      xju::assert_equal(reconstruct(root), x);
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
  
  {
    std::string const x("volatiles ");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::cv()+hcp_parser::parseLiteral(x))};
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
  
}

void test38()
{
  {
    std::string const x("int f");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::type_ref()+hcp_parser::parseLiteral("f"))};
      xju::assert_equal(reconstruct(root), x);
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
  {
    std::string const x("const int& f");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::type_ref()+hcp_parser::parseLiteral("f"))};
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
  {
    std::string const x("int* const& f");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::type_ref()+hcp_parser::parseLiteral("f"))};
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
  {
    std::string const x("fred<int* const&,float,jock<float>> f");
    
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::type_ref()+hcp_parser::parseLiteral("f"))};
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
}

void test39()
{
  std::string const x(
    "extern const SyscallF3<int, const char*, int, mode_t> open;");
  try {
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::extern_var_def())};
    auto const y(
      hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(
        root.items()[0]->asA<hcp_ast::ExternVarDef>()));
    xju::assert_equal(reconstruct(y),"open");
    
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "", XJU_TRACED);
    xju::assert_equal(true,false);
  }
}

void test40()
{
  std::string const x(
    "template<class T>\n"
    "    inline EventP<T>::EventP() throw():\n"
    "      m_observers(0)\n"
    "    {\n"
    "}\n");
  try {
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::template_function_def())};
    xju::assert_equal(reconstruct(root), x);
    std::vector<hcp_ast::IR>::const_iterator j(
      std::find_if(
        root.items()[0]->asA<hcp_ast::TemplateFunctionDef>().items().begin(),
        root.items()[0]->asA<hcp_ast::TemplateFunctionDef>().items().end(),
        hcp_ast::isA_<hcp_ast::FunctionName>));
    xju::assert_not_equal(
      j, 
      root.items()[0]->asA<hcp_ast::TemplateFunctionDef>().items().end());
    xju::assert_equal(reconstruct(*j), "EventP<T>::EventP");
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "", XJU_TRACED);
    xju::assert_equal(true,false);
  }
}

void test41()
{
  std::string const x(
    "template<class T>\n"
    "inline EventP<T>& EventP<T>::operator=(const EventP& b)\n"
    "    throw()\n"
    "{\n"
    "    return *this;\n"
    "}\n");
  try {
    auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::template_function_def())};
    std::vector<hcp_ast::IR>::const_iterator j(
      std::find_if(
        root.items()[0]->asA<hcp_ast::TemplateFunctionDef>().items().begin(),
        root.items()[0]->asA<hcp_ast::TemplateFunctionDef>().items().end(),
        hcp_ast::isA_<hcp_ast::FunctionName>));
    xju::assert_not_equal(
      j, 
      root.items()[0]->asA<hcp_ast::TemplateFunctionDef>().items().end());
    xju::assert_equal(reconstruct(*j), "EventP<T>::operator=");
  }
  catch(xju::Exception const& e) {
    assert_readableRepr_equal(e, "", XJU_TRACED);
    xju::assert_equal(true,false);
  }
}

void test42()
{
  {
    std::string const x(
      "void (T::*f)(P)");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::var_fp())};
      xju::assert_equal(reconstruct(root), x);
      std::vector<hcp_ast::IR>::const_iterator j(
        std::find_if(
          root.items().begin(),
          root.items().end(),
          hcp_ast::isA_<hcp_ast::VarName>));
      xju::assert_not_equal(
        j, 
        root.items().end());
      xju::assert_equal(reconstruct(*j), "f");
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
  {
    std::string const x(
      "void (T::*f)() throw()");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::var_fp(),true)};
      xju::assert_equal(reconstruct(root), x);
      std::vector<hcp_ast::IR>::const_iterator j(
        std::find_if(
          root.items().begin(),
          root.items().end(),
          hcp_ast::isA_<hcp_ast::VarName>));
      xju::assert_not_equal(
        j, 
        root.items().end());
      xju::assert_equal(reconstruct(*j), "f");
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
}
void test43()
{
  {
    std::string const x(
      "xju::Optional<size_t> const& length");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::var_non_fp(),true)};
      xju::assert_equal(reconstruct(root), x);

      auto const y(
        hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(
          root.items()[0]->asA<hcp_ast::Item>()));
      xju::assert_equal(reconstruct(y),"length");
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
}
void test44()
{
  {
    std::string const x("abc");
    auto r(hcp_parser::parseString(x.begin(),x.end(),
                                   hcp_parser::parseLiteral("abc")));
    xju::assert_equal(reconstruct(r),"abc");
  }
}
void test45()
{
  {
    std::string const x("aaa");
    auto r(hcp_parser::parseString(x.begin(),x.end(),
                                   hcp_parser::parseUntil(
                                     hcp_parser::parseLiteral("a"),
                                     hcp_parser::endOfFile())));
    xju::assert_equal(reconstruct(r),"aaa");
  }
  {
    std::string const x("aaa");
    auto r(hcp_parser::parseString(x.begin(),x.end(),
                                   hcp_parser::parseUntil(
                                     hcp_parser::endOfFile())));
    xju::assert_equal(reconstruct(r),"aaa");
  }
}

void test46()
{
  {
    std::string const x(
      "A ");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::base_specifier_list(),true)};
      xju::assert_equal(reconstruct(root), x);

      auto const y(
        hcp_ast::findChildrenOfType<hcp_ast::ScopedName>(root));
      xju::assert_equal(reconstruct(y[0]),"A ");
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
  {
    std::string const x(
      "A , private virtual I::B<int,C>");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::base_specifier_list(),true)};
      xju::assert_equal(reconstruct(root), x);

      {
        auto const y(
          hcp_ast::findChildrenOfType<hcp_ast::BaseSpecifier>(
            root));
        xju::assert_equal(reconstruct(y[0]),"A ");
        xju::assert_equal(reconstruct(y[1]),"private virtual I::B<int,C>");
      }
      {
        auto const y(
          hcp_ast::findChildrenOfType<hcp_ast::ScopedName>(
            root));
        xju::assert_equal(reconstruct(y[0]),"A ");
        xju::assert_equal(reconstruct(y[1]),"I::B<int,C>");
      }
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "", XJU_TRACED);
      xju::assert_equal(true,false);
    }
  }
  {
    std::string const x(
      "A , private virtual I::B<int,C>, private private D");
    try {
      auto const root{parseString(
        x.begin(),x.end(),
        hcp_parser::base_specifier_list(),true)};
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e) {
      assert_readableRepr_equal(e, "Failed to parse base specifier at line 1 column 34 because\nfailed to parse !(\"public\" then !one of chars 'a'..'z' or one of chars 'A'..'Z' or one of chars '0'..'9' or one of chars \"_\" then optional whitespace) or (\"private\" then !one of chars 'a'..'z' or one of chars 'A'..'Z' or one of chars '0'..'9' or one of chars \"_\" then optional whitespace) or (\"protected\" then !one of chars 'a'..'z' or one of chars 'A'..'Z' or one of chars '0'..'9' or one of chars \"_\" then optional whitespace) or (\"virtual\" then !one of chars 'a'..'z' or one of chars 'A'..'Z' or one of chars '0'..'9' or one of chars \"_\" then optional whitespace) at line 1 column 42 because\nline 1 column 42: expected parse failure.", XJU_TRACED);
    }
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
  test29(std::vector<std::string>(&argv[1], &argv[argc])), ++n;
  test30(), ++n;
  test31(), ++n;
  test32(), ++n;
  test33(), ++n;
  test34(), ++n;
  test35(), ++n;
  test36(), ++n;
  test37(), ++n;
  test38(), ++n;
  test39(), ++n;
  test40(), ++n;
  test41(), ++n;
  test42(), ++n;
  test43(), ++n;
  test44(), ++n;
  test45(), ++n;
  test46(), ++n;
  
  xju::assert_equal(atLeastOneReadableReprFailed, false);
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}
