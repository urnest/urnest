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
  at = hcp_parser::parseAnyChar->parse(root, at, options);
  xju::assert_equal(at.x_, x.begin()+1);
  at = hcp_parser::parseAnyChar->parse(root, at, options);
  xju::assert_equal(at.x_, x.begin()+2);
  
  xju::assert_equal(reconstruct(root), "ab");

  try {
    hcp_parser::I const y3(hcp_parser::parseAnyChar->parse(
      root,
      at,
      options));
    xju::assert_not_equal(y3, y3);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_equal(readableRepr(e), "line 1 column 3: failed to parse any char because\n  line 1 column 3: end of input");
  }

}

void test2()
{
  hcp_parser::Cache cache(new hcp_parser::CacheVal());
  hcp_parser::Options const options(false, cache);
  std::string const x("ab");
  hcp_ast::CompositeItem root;
  hcp_parser::I at(x.begin(), x.end());
  at = (hcp_parser::zeroOrMore*hcp_parser::parseAnyChar)->parse(root, at, options);
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

  at = (hcp_parser::parseAnyChar+
        hcp_parser::parseAnyChar)->parse(
          root, at, options);
  xju::assert_equal(reconstruct(root), "ab");
  xju::assert_equal(at.atEnd(), true);

 {
   hcp_parser::PR p(hcp_parser::parseAnyChar+
                    hcp_parser::parseAnyChar+
                    hcp_parser::parseAnyChar);
   try {
     at = p->parse(
             root, at, options);
     xju::assert_not_equal(at, at);
   }
   catch(hcp_parser::Exception const& e) {
     xju::assert_equal(readableRepr(e), "line 1 column 3: failed to parse any char then any char then any char because\n  line 1 column 3: failed to parse any char because\n  line 1 column 3: end of input");
   }
 }
}

void test4()
{
  hcp_parser::Cache cache(new hcp_parser::CacheVal());
  hcp_parser::Options const options(false, cache);
  std::string const x("abcd");
  hcp_ast::CompositeItem root;
  hcp_parser::I at(x.begin(), x.end());
  at = (hcp_parser::zeroOrMore*hcp_parser::parseOneOfChars("abc"))->parse(root, at, options);
  xju::assert_equal(reconstruct(root), "abc");
  xju::assert_equal(at.atEnd(), false);

  {
    hcp_parser::PR p(hcp_parser::parseOneOfChars("abc"));
    try {
      at = p->parse(
        root, at, options);
      xju::assert_abort();
    }
    catch(hcp_parser::Exception const& e) {
      xju::assert_equal(readableRepr(e), "line 1 column 4: failed to parse one of chars [abc] because\n  line 1 column 4: unexpected character");
    }
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
    at = (hcp_parser::zeroOrMore*
          (hcp_parser::parseOneOfChars("ab")|
           hcp_parser::parseOneOfChars("c")))->parse(
             root, at, options);
    xju::assert_equal(reconstruct(root), "abca");
    xju::assert_equal(at.atEnd(), false);

    {
      hcp_parser::PR p(hcp_parser::parseOneOfChars("ab")|
                       hcp_parser::parseOneOfChars("c"));
      try {
        at = p->parse(
          root, at, options);
        xju::assert_abort();
      }
      catch(hcp_parser::Exception const& e) {
        xju::assert_equal(readableRepr(e), "line 1 column 5: failed to parse one of chars [ab] or one of chars [c] because\n  line 1 column 5: failed to parse one of chars [c] because\n  line 1 column 5: unexpected character");
      }
    }
  }
  
  {
    
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("abcad");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end()); 
    {
      hcp_parser::PR p(hcp_parser::parseOneOfChars("b")|
                       hcp_parser::parseOneOfChars("c")|
                       hcp_parser::parseOneOfChars("e"));
      try {
        at = p->parse(
          root, at, options);
        xju::assert_abort();
      }
      catch(hcp_parser::Exception const& e) {
        xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse one of chars [b] or one of chars [c] or one of chars [e] because\n  line 1 column 1: failed to parse one of chars [e] because\n  line 1 column 1: unexpected character");
      }
    }
  }
  
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("abcad");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    at = (hcp_parser::zeroOrMore*
          (hcp_parser::parseOneOfChars("ab")|
           hcp_parser::parseOneOfChars("c")|
           hcp_parser::parseOneOfChars("d")))->parse(
             root, at, options);
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
  at = (hcp_parser::atLeastOne*hcp_parser::parseOneOfChars("abc"))->parse(root, at, options);
  xju::assert_equal(reconstruct(root), "abc");
  xju::assert_equal(at.atEnd(), false);
}

void test7()
{
  hcp_parser::Cache cache(new hcp_parser::CacheVal());
  hcp_parser::Options const options(false, cache);
  std::string const x("abcd");
  hcp_ast::CompositeItem root;
  hcp_parser::I at(x.begin(), x.end());
  {
    hcp_parser::PR p(hcp_parser::atLeastOne*hcp_parser::parseOneOfChars("e"));
    
    try {
      at = p->parse(root, at, options);
      xju::assert_abort();
    }
    catch(hcp_parser::Exception const& e) {
      xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse at least one occurrance of one of chars [e] because\n  line 1 column 1: failed to parse one of chars [e] then zero or more occurrances of one of chars [e] because\n  line 1 column 1: failed to parse one of chars [e] because\n  line 1 column 1: unexpected character");
    }
  }
}

void test8()
{
  hcp_parser::Cache cache(new hcp_parser::CacheVal());
  hcp_parser::Options const options(false, cache);
  std::string const x("abcdefg");
  hcp_ast::CompositeItem root;
  hcp_parser::I at(x.begin(), x.end());
  at = (hcp_parser::parseUntil(
    hcp_parser::parseOneOfChars("e")))->parse(root, at, options);
  xju::assert_equal(reconstruct(root), "abcd");
  xju::assert_equal(at.atEnd(), false);
}

void test9()
{
  hcp_parser::Cache cache(new hcp_parser::CacheVal());
  hcp_parser::Options const options(false, cache);
  std::string const x("abcdefg");
  hcp_ast::CompositeItem root;
  hcp_parser::I at(x.begin(), x.end());
  {
    hcp_parser::PR p(hcp_parser::parseUntil(
                       hcp_parser::parseOneOfChars("h")));
    try {
      at = p->parse(root, at, options);
      xju::assert_abort();
    }
    catch(hcp_parser::Exception const& e) {
      xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse up to but not including one of chars [h] because\n  line 1 column 8: end of input");
    }
  }
}

void test10()
{
  hcp_parser::Cache cache(new hcp_parser::CacheVal());
  hcp_parser::Options const options(false, cache);
  std::string const x("abcd");
  hcp_ast::CompositeItem root;
  hcp_parser::I at(x.begin(), x.end());
  
  try {
    at = hcp_parser::whitespaceChar->parse(
      root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse one of chars [\\t\\n ] because\n  line 1 column 1: unexpected character");
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
    
    at = hcp_parser::stringLiteral->parse(root, at, options);
    xju::assert_equal(reconstruct(root), "\"abcd\"");
    xju::assert_equal(at.atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"\"");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::stringLiteral->parse(root, at, options);
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
    
    at = hcp_parser::stringLiteral->parse(root, at, options);
    xju::assert_equal(reconstruct(root), "\"\\\"\"");
    xju::assert_equal(at.atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"\\\"de\"");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::stringLiteral->parse(root, at, options);
    xju::assert_equal(reconstruct(root), "\"\\\"de\"");
    xju::assert_equal(at.atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"ab\\\"de\"");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::stringLiteral->parse(root, at, options);
    xju::assert_equal(reconstruct(root), "\"ab\\\"de\"");
    xju::assert_equal(at.atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"ab\\\"d\ne\"");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::stringLiteral->parse(root, at, options);
    xju::assert_equal(reconstruct(root), "\"ab\\\"d\ne\"");
    xju::assert_equal(at.atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"ab\\\"d\ne\"   ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::stringLiteral->parse(root, at, options);
    xju::assert_equal(reconstruct(root), "\"ab\\\"d\ne\"   ");
    xju::assert_equal(at.atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"abcde\"  \n \"fred\"  ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::stringLiteral->parse(root, at, options);
    xju::assert_equal(reconstruct(root), "\"abcde\"  \n \"fred\"  ");
    xju::assert_equal(at.atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"abcde\"  \n \"fred\"\n\"jock\"  ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::stringLiteral->parse(root, at, options);
    xju::assert_equal(reconstruct(root), "\"abcde\"  \n \"fred\"\n\"jock\"  ");
    xju::assert_equal(at.atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"abcde\" \"fred\"");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    at = hcp_parser::stringLiteral->parse(root, at, options);
    xju::assert_equal(reconstruct(root), "\"abcde\" ");
    xju::assert_equal(at.atEnd(), false);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("\"abcde");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    at = hcp_parser::stringLiteral->parse(root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    // REVISIT: indecipherable error message
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse string literal because\n  line 1 column 1: failed to parse one of chars [\"] then (zero or more occurrances of (up to but not including one of chars [\"\\] then one of chars [\\] then one of chars [\"])) then up to but not including one of chars [\"] then one of chars [\"] then (zero or more occurrances of ((zero or more occurrances of one of chars [\\t ]) then one of chars [\\n] then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then one of chars [\"] then (zero or more occurrances of (up to but not including one of chars [\"\\] then one of chars [\\] then one of chars [\"])) then up to but not including one of chars [\"] then one of chars [\"])) then zero or more occurrances of (one of chars [\\t\\n ] or comments) because\n  line 1 column 2: failed to parse up to but not including one of chars [\"] because\n  line 1 column 7: end of input");
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
    
    at = hcp_parser::parseLiteral("abcd")->parse(
      root, at, options);
    xju::assert_equal(reconstruct(root), "abcd");
    xju::assert_equal(at.atEnd(), true);
  }
  {
    hcp_parser::PR p(hcp_parser::parseLiteral("abed"));
    try {
      hcp_parser::Cache cache(new hcp_parser::CacheVal());
      hcp_parser::Options const options(false, cache);
      std::string const x("abcd");
      hcp_ast::CompositeItem root;
      hcp_parser::I at(x.begin(), x.end());
      
      at = p->parse(
        root, at, options);
      xju::assert_abort();
    }
    catch(hcp_parser::Exception const& e) {
      xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse \"abed\" because\n  line 1 column 3: mismatch");
    }
  }
}

// comments
void test13()
{
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("// comment \n   ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::comments->parse(
      root, at, options);
    xju::assert_equal(reconstruct(root), "// comment \n   ");
    xju::assert_equal(at.atEnd(), true);
  }
  try {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("/ / comment \n   ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::comments->parse(root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    // REVISIT: error message too verbose
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse comments because\n  line 1 column 1: failed to parse at least one occurrance of line comment or block comment because\n  line 1 column 1: failed to parse (line comment or block comment) then zero or more occurrances of (line comment or block comment) because\n  line 1 column 1: failed to parse line comment or block comment because\n  line 1 column 1: failed to parse block comment because\n  line 1 column 1: failed to parse \"/*\" then up to but not including \"*/\" then zero or more occurrances of one of chars [\\t\\n ] because\n  line 1 column 1: failed to parse \"/*\" because\n  line 1 column 2: mismatch");
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
    
    at = hcp_parser::hash->parse(
      root, at, options);
    xju::assert_equal(reconstruct(root), "#if\n  ");
    xju::assert_equal(at.atEnd(), true);
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(" #if");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::hash->parse(
      root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse other preprocessor directive because\n  line 1 column 1: failed to parse '#' at start of line then up to but not including one of chars [\\n] then zero or more occurrances of (one of chars [\\t\\n ] or comments) because\n  line 1 column 1: failed to parse '#' at start of line because\n  line 1 column 1: line does not start with '#'");
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
    
    at = hcp_parser::hashInclude->parse(
      root, at, options);
    xju::assert_equal(reconstruct(root), "#include <string>\n  ");
    xju::assert_equal(at.atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("#  include <string>\n  ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::hashInclude->parse(
      root, at, options);
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
    
    at = hcp_parser::hashInclude->parse(
      root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    // REVISIT: too verbose
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse #include because\n  line 1 column 1: failed to parse '#' at start of line then (zero or more occurrances of one of chars [\\t ]) then \"include\" then up to but not including \"\n\" or (\"//\" then (zero or more occurrances of one of chars [\\t ]) then \"impl\" then (zero or more occurrances of one of chars [\\t ]) then \"\n\") then one of chars [\\n] then zero or more occurrances of (one of chars [\\t\\n ] or comments) because\n  line 1 column 2: failed to parse \"include\" because\n  line 1 column 3: mismatch");
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
    
    at = hcp_parser::hashIncludeImpl->parse(
      root, at, options);
    xju::assert_equal(reconstruct(root), "#include <string> // impl \n  ");
    xju::assert_equal(at.atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("#  include <string>//impl\n  ");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::hashIncludeImpl->parse(
      root, at, options);
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
    
    at = hcp_parser::hashIncludeImpl->parse(
      root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    // REVISIT: too verbose
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse #include with //impl marker because\n  line 1 column 1: failed to parse '#' at start of line then (zero or more occurrances of one of chars [\\t ]) then \"include\" then up to but not including \"\n\" or (\"//\" then (zero or more occurrances of one of chars [\\t ]) then \"impl\" then (zero or more occurrances of one of chars [\\t ]) then \"\n\") then \"//\" then (zero or more occurrances of one of chars [\\t ]) then \"impl\" then (zero or more occurrances of one of chars [\\t ]) then \"\n\" then zero or more occurrances of (one of chars [\\t\\n ] or comments) because\n  line 1 column 43: failed to parse \"//\" because\n  line 1 column 43: mismatch");
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
    
    at = hcp_parser::balanced(hcp_parser::parseOneOfChars("("))->parse(
      root, at, options);
    xju::assert_equal(reconstruct(root), "");
    xju::assert_equal((++at).atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("fred(");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::balanced(hcp_parser::parseOneOfChars("("))->parse(
      root, at, options);
    xju::assert_equal(reconstruct(root), "fred");
    xju::assert_equal((++at).atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("fred[jock](");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::balanced(hcp_parser::parseOneOfChars("("))->parse(
      root, at, options);
    xju::assert_equal(reconstruct(root), "fred[jock]");
    xju::assert_equal((++at).atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("{a}(b);}");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::balanced(hcp_parser::parseOneOfChars("}"))->parse(
      root, at, options);
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
    
    at = hcp_parser::balanced(hcp_parser::parseOneOfChars("("))->parse(
      root, at, options);
    xju::assert_equal(reconstruct(root), "fred[\"jock(\"]");
    xju::assert_equal((++at).atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("template<fred, jock> fred[\"jock(\"](");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::balanced(hcp_parser::parseOneOfChars("("))->parse(
      root, at, options);
    xju::assert_equal(reconstruct(root), 
                      "template<fred, jock> fred[\"jock(\"]");
    xju::assert_equal((++at).atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("template<fred, jock> fred[// john(fred)\n\"jock(\"](");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::balanced(hcp_parser::parseOneOfChars("("))->parse(
      root, at, options);
    xju::assert_equal(reconstruct(root), std::string(x.begin(), x.end()-1));
    xju::assert_equal((++at).atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  {
    hcp_parser::PR p(hcp_parser::balanced(hcp_parser::parseOneOfChars("(")));
    try
      {
        hcp_parser::Cache cache(new hcp_parser::CacheVal());
        hcp_parser::Options const options(false, cache);
        std::string const x("[]");
        hcp_ast::CompositeItem root;
        hcp_parser::I at(x.begin(), x.end());
        
        at = p->parse(
          root, at, options);
        xju::assert_abort();
      }
    catch(hcp_parser::Exception const& e) {
      xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [(] because\n  line 1 column 1: end of input");
    }
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
    
    at = hcp_parser::typedef_statement->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("typedef\nstd::vector<int>\nInts;");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::typedef_statement->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("typedefine std::vector<int>\nInts;");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::typedef_statement->parse(root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse typedef statement because\n  line 1 column 1: failed to parse \"typedef\" then some whitespace then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [;] then one of chars [;] then zero or more occurrances of (one of chars [\\t\\n ] or comments) because\n  line 1 column 8: failed to parse some whitespace because\n  line 1 column 8: failed to parse at least one occurrance of one of chars [\\t\\n ] because\n  line 1 column 8: failed to parse one of chars [\\t\\n ] then zero or more occurrances of one of chars [\\t\\n ] because\n  line 1 column 8: failed to parse one of chars [\\t\\n ] because\n  line 1 column 8: unexpected character");
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
    
    at = hcp_parser::using_statement->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("using\nnamespace\nstd;");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::using_statement->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("usingme as something;");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::using_statement->parse(root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse using statement because\n  line 1 column 1: failed to parse \"using\" then some whitespace then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [;] then one of chars [;] then zero or more occurrances of (one of chars [\\t\\n ] or comments) because\n  line 1 column 6: failed to parse some whitespace because\n  line 1 column 6: failed to parse at least one occurrance of one of chars [\\t\\n ] because\n  line 1 column 6: failed to parse one of chars [\\t\\n ] then zero or more occurrances of one of chars [\\t\\n ] because\n  line 1 column 6: failed to parse one of chars [\\t\\n ] because\n  line 1 column 6: unexpected character");
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
    
    at = hcp_parser::enum_def->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x("enumy { FRED, JOCK };");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::enum_def->parse(root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse enum definition because\n  line 1 column 1: failed to parse \"enum\" then some whitespace then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [;] then one of chars [;] then zero or more occurrances of (one of chars [\\t\\n ] or comments) because\n  line 1 column 5: failed to parse some whitespace because\n  line 1 column 5: failed to parse at least one occurrance of one of chars [\\t\\n ] because\n  line 1 column 5: failed to parse one of chars [\\t\\n ] then zero or more occurrances of one of chars [\\t\\n ] because\n  line 1 column 5: failed to parse one of chars [\\t\\n ] because\n  line 1 column 5: unexpected character");
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
    
    at = hcp_parser::function_decl->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(
      "std::ostream& operator<<(std::ostream& s, const TaskId& x) throw();");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::function_decl->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
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
    
    at = hcp_parser::function_decl->parse(root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse function declaration because\n  line 1 column 1: failed to parse function qualifiers then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [();[]{}] or (((\"operator\" then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then (\"<<\" or \">>\" or \"==\" or \"<=\" or \">=\" or \"<\" or \">\" or \"++\" or \"--\" or \"->\" or \"+\" or \"-\" or \"|=\" or \"&=\" or \"|\" or \"&\" or \"[]\" or \"!\" or \"%=\" or \"%\" or \"=\" or \"*\") then zero or more occurrances of (one of chars [\\t\\n ] or comments)) or ((zero or more occurrances of (at least one occurrance of one of chars 'a'..'z' or one of chars 'A'..'Z' or one of chars '0'..'9' or one of chars [_] then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then \"::\" then zero or more occurrances of (one of chars [\\t\\n ] or comments))) then at least one occurrance of one of chars 'a'..'z' or one of chars 'A'..'Z' or one of chars '0'..'9' or one of chars [_] then zero or more occurrances of (one of chars [\\t\\n ] or comments))) then one of chars [(]) then function name then \"(\" then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including \")\" then \")\" then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [:;{] or (\"try\" then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then one of chars [:]) then one of chars [;] then zero or more occurrances of (one of chars [\\t\\n ] or comments) because\n  line 2 column 27: failed to parse one of chars [;] because\n  line 2 column 27: unexpected character");
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
    
    at = hcp_parser::function_def->parse(root, at, options);
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
    
    at = hcp_parser::function_def->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  {
    hcp_parser::PR p(hcp_parser::parseLiteral("class")|
                     hcp_parser::parseLiteral("struct")|
                     hcp_parser::parseLiteral("union"));
    try
      {
        hcp_parser::Cache cache(new hcp_parser::CacheVal());
        hcp_parser::Options const options(false, cache);
        std::string const x(
          "void fred() const throw();");
        hcp_ast::CompositeItem root;
        hcp_parser::I at(x.begin(), x.end());
        
        at = p->parse(root, at, options);
        xju::assert_abort();
      }
    catch(hcp_parser::Exception const& e) {
      xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse \"class\" or \"struct\" or \"union\" because\n  line 1 column 1: failed to parse \"union\" because\n  line 1 column 1: mismatch");
    }
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(
      "template<class T>\n"
      "void fred() const throw();");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::function_def->parse(root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse function definition because\n  line 1 column 1: failed to parse function qualifiers then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [();[]{}] or (((\"operator\" then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then (\"<<\" or \">>\" or \"==\" or \"<=\" or \">=\" or \"<\" or \">\" or \"++\" or \"--\" or \"->\" or \"+\" or \"-\" or \"|=\" or \"&=\" or \"|\" or \"&\" or \"[]\" or \"!\" or \"%=\" or \"%\" or \"=\" or \"*\") then zero or more occurrances of (one of chars [\\t\\n ] or comments)) or ((zero or more occurrances of (at least one occurrance of one of chars 'a'..'z' or one of chars 'A'..'Z' or one of chars '0'..'9' or one of chars [_] then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then \"::\" then zero or more occurrances of (one of chars [\\t\\n ] or comments))) then at least one occurrance of one of chars 'a'..'z' or one of chars 'A'..'Z' or one of chars '0'..'9' or one of chars [_] then zero or more occurrances of (one of chars [\\t\\n ] or comments))) then one of chars [(]) then function name then \"(\" then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including \")\" then \")\" then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [:;{] or (\"try\" then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then one of chars [:]) then function implementation because\n  line 2 column 26: failed to parse function implementation because\n  line 2 column 26: failed to parse (zero or more occurrances of (\"try\" then zero or more occurrances of (one of chars [\\t\\n ] or comments))) then (zero or more occurrances of (initialiser list then zero or more occurrances of (one of chars [\\t\\n ] or comments))) then block then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then zero or more occurrances of (\"catch\" then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then \"(\" then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including \")\" then \")\" then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then block then zero or more occurrances of (one of chars [\\t\\n ] or comments)) because\n  line 2 column 26: failed to parse block because\n  line 2 column 26: failed to parse \"{\" then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [}] then \"}\" because\n  line 2 column 26: failed to parse \"{\" because\n  line 2 column 26: mismatch");
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
    
    at = hcp_parser::template_function_def->parse(root, at, options);
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
    
    at = hcp_parser::function_def->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
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
    
    at = hcp_parser::template_function_def->parse(root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse template function definition because\n  line 1 column 1: failed to parse at least one occurrance of \"template\" then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then one of chars [<] then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [>] then one of chars [>] then zero or more occurrances of (one of chars [\\t\\n ] or comments) then function definition because\n  line 1 column 1: failed to parse at least one occurrance of \"template\" then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then one of chars [<] then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [>] then one of chars [>] then zero or more occurrances of (one of chars [\\t\\n ] or comments) because\n  line 1 column 1: failed to parse \"template\" then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then one of chars [<] then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [>] then one of chars [>] then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then zero or more occurrances of (\"template\" then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then one of chars [<] then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [>] then one of chars [>] then zero or more occurrances of (one of chars [\\t\\n ] or comments)) because\n  line 1 column 1: failed to parse \"template\" because\n  line 1 column 1: mismatch");
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
    
    at = hcp_parser::class_def->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
    xju::assert_equal(root.items_[0]->isA<hcp_ast::ClassDef>(), true);
    xju::assert_equal(root.items_[0]->asA<hcp_ast::ClassDef>().className_, "X");
  }
  catch(hcp_parser::Exception const& e) {
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
    

    at = hcp_parser::class_def->parse(root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse class definition because\n  line 1 column 1: failed to parse non-template class definition because\n  line 1 column 1: failed to parse (zero or more occurrances of (\"template\" then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then one of chars [<] then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [>] then one of chars [>] then zero or more occurrances of (one of chars [\\t\\n ] or comments))) then (\"class\" or \"struct\" or \"union\") then some whitespace then class name then parse text, balancing (), [], {}, <>, stringLiteral, up to but not including one of chars [;{] then one of chars [{] then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then (zero or more occurrances of (comments or function declaration or template function definition or public/private/protected: marker or class forward-declaration or enum definition or typedef statement or class definition or function definition or object declaration)) then one of chars [}] then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then one of chars [;] then zero or more occurrances of (one of chars [\\t\\n ] or comments) because\n  line 1 column 1: failed to parse \"class\" or \"struct\" or \"union\" because\n  line 1 column 1: failed to parse \"union\" because\n  line 1 column 1: mismatch");
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(xju::readFile(f[1]));
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::class_def->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
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
    
    at = hcp_parser::anonymous_namespace->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
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
    
    at = hcp_parser::anonymous_namespace->parse(root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse anonymous namespace because\n  line 1 column 1: failed to parse \"namespace\" then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then one of chars [{] then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then (zero or more occurrances of (comments or #include with //impl marker or #include or other preprocessor directive or class definition or typedef statement or using statement or enum definition or some whitespace or function declaration or template function definition or function definition or object declaration)) then one of chars [}] then zero or more occurrances of (one of chars [\\t\\n ] or comments) because\n  line 1 column 1: failed to parse \"namespace\" because\n  line 1 column 1: mismatch");
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(xju::readFile(f[3]));
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::anonymous_namespace->parse(root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse anonymous namespace because\n  line 1 column 1: failed to parse \"namespace\" then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then one of chars [{] then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then (zero or more occurrances of (comments or #include with //impl marker or #include or other preprocessor directive or class definition or typedef statement or using statement or enum definition or some whitespace or function declaration or template function definition or function definition or object declaration)) then one of chars [}] then zero or more occurrances of (one of chars [\\t\\n ] or comments) because\n  line 1 column 11: failed to parse one of chars [{] because\n  line 1 column 11: unexpected character");
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
    
    at = hcp_parser::namespace_def->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(xju::readFile(f[4]));
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::namespace_def->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
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
    
    at = hcp_parser::anonymous_namespace->parse(root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse anonymous namespace because\n  line 1 column 1: failed to parse \"namespace\" then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then one of chars [{] then (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then (zero or more occurrances of (comments or #include with //impl marker or #include or other preprocessor directive or class definition or typedef statement or using statement or enum definition or some whitespace or function declaration or template function definition or function definition or object declaration)) then one of chars [}] then zero or more occurrances of (one of chars [\\t\\n ] or comments) because\n  line 1 column 1: failed to parse \"namespace\" because\n  line 1 column 1: mismatch");
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
    
    at = hcp_parser::file->parse(root, at, options);
    xju::assert_equal(at.atEnd(), true);
    xju::assert_equal(reconstruct(root), x);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(xju::readFile(f[3])+"xxx");
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::file->parse(root, at, options);
    xju::assert_abort();
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_equal(readableRepr(e), "line 1 column 1: failed to parse file because\n  line 1 column 1: failed to parse (zero or more occurrances of (one of chars [\\t\\n ] or comments)) then (zero or more occurrances of (anonymous namespace or namespace or comments or #include with //impl marker or #include or other preprocessor directive or class definition or typedef statement or using statement or enum definition or some whitespace or function declaration or template function definition or function definition or object declaration)) then end of file because\n  line 22 column 1: failed to parse end of file because\n  line 22 column 1: expected end of input");
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
    
    at = hcp_parser::file->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
    xju::assert_not_equal(readableRepr(e), readableRepr(e));
  }
  try
  {
    hcp_parser::Cache cache(new hcp_parser::CacheVal());
    hcp_parser::Options const options(false, cache);
    std::string const x(xju::readFile(f[5]));
    hcp_ast::CompositeItem root;
    hcp_parser::I at(x.begin(), x.end());
    
    at = hcp_parser::file->parse(root, at, options);
    xju::assert_equal(reconstruct(root), x);
    xju::assert_equal(at.atEnd(), true);
  }
  catch(hcp_parser::Exception const& e) {
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
