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

#include <vector>
#include <algorithm>
#include <sstream>
#include "xju/format.hh"
#include "xju/JoiningIterator.hh"

namespace hcp_parser
{

namespace
{
  std::string contextReadableRepr(
    std::pair<std::pair<Parser const*, I>, xju::Traced> const& c) throw()
  {
    std::ostringstream s;
    s << c.first.second << ": failed to parse " << c.first.first->target();
    return s.str();
  }
}
  
std::string readableRepr(Exception const& e) throw()
{
  std::ostringstream s;
  if (e.context_.size()==0) {
    s << e.at_ << ": " << e.cause_;
  }
  else {
    std::vector<std::string> context;
    std::transform(e.context_.rbegin(), e.context_.rend(),
                   xju::JoiningIterator<std::string, std::string>(
                     s,
                     " because\n  "),
                   std::ptr_fun(contextReadableRepr));
    s << " because\n  "
      << e.at_ << ": " << e.cause_;
  }
  return s.str();
}

namespace
{
class ParseOr;
class ParseAnd;
class ParseZeroOrMore;

class ParseZeroOrMore : public Parser
{
public:
  PR x_;
  
  virtual ~ParseZeroOrMore() throw() {
  }
  explicit ParseZeroOrMore(PR x) throw():
    x_(x) {
  }
  
  // Parser::
  virtual ParseResult parse_(I const at, Options const& options) throw() 
  {
    PV result(IRs(), at);
    while(true) {
      ParseResult const r(x_->parse(result.second, options));
      if (!r.failed()) {
        PV const x(*r);
        std::copy(x.first.begin(), x.first.end(),
                  std::back_inserter(result.first));
        result.second=x.second;
      }
      else {
        return ParseResult(result);
      }
    }
  }

  // Parser::
  virtual std::string target() const throw();
};

class ParseAnd : public Parser
{
public:
  std::vector<PR> terms_;
  
  virtual ~ParseAnd() throw() {
  }
  // Parser::
  virtual ParseResult parse_(I const at, Options const& options) throw() 
  {
    ParseResult first(terms_.front()->parse(at, options));
    if (first.failed()) {
      return first;
    }
    PV result(*first);
    for(std::vector<PR>::iterator i = xju::next(terms_.begin()); 
        i != terms_.end();
        ++i) {
      ParseResult const br((*i)->parse(result.second, options));
      if (br.failed()) {
        return br;
      }
      else {
        std::copy((*br).first.begin(), (*br).first.end(),
                  std::back_inserter(result.first));
        result.second=(*br).second;
      }
    }
    return ParseResult(result);
  }

  // Parser::
  virtual std::string target() const throw();
};

class ParseOr : public Parser
{
public:
  std::vector<PR> terms_;
  
  virtual ~ParseOr() throw() {
  }

  // Parser::
  virtual ParseResult parse_(I const at, Options const& options) throw() 
  {
    std::multimap<I, Exception> failures;
    for(std::vector<PR>::iterator i = terms_.begin(); i != terms_.end(); ++i) {
      ParseResult r((*i)->parse(at, options));
      if (r.failed()) {
        failures.insert(std::make_pair(r.e().at_, r.e()));
      }
      else
      {
        return r;
      }
    }
    return ParseResult((*failures.rbegin()).second);
  }

  // Parser::
  virtual std::string target() const throw();
};

class ParseNot : public Parser
{
  static std::string const expected_parse_failure;
public:
  explicit ParseNot(PR term) throw():
    term_(term)
  {
  }
  PR term_;
  
  virtual ~ParseNot() throw() {
  }

  // Parser::
  virtual ParseResult parse_(I const at, Options const& options) throw() 
  {
    ParseResult const r(term_->parse(at, options));
    if (r.failed()) {
      return ParseResult(PV(IRs(), at));
    }
    return ParseResult(
      Exception(ParseNot::expected_parse_failure, at, XJU_TRACED));
  }

  // Parser::
  virtual std::string target() const throw();

};
std::string const ParseNot::expected_parse_failure("expected parse failure");

std::string ParseZeroOrMore::target() const throw() 
{
  // bracket lhs/rhs if ambiguous (and/or/times)
  std::string const xt(x_->target());
  std::ostringstream s;
  s << "zero or more occurrances of " 
    << ((dynamic_cast<ParseOr const*>(&*x_)||
         dynamic_cast<ParseAnd const*>(&*x_))?
        std::string("(")+xt+std::string(")"):
        xt);
  return s.str();
}

std::string ParseAnd::target() const throw() 
{
  std::vector<std::string> x;
  for(std::vector<PR>::const_iterator i=terms_.begin(); i!=terms_.end(); ++i) {
    // bracket lhs/rhs if ambiguous (and/or/times)
    std::string const at((*i)->target());
    std::ostringstream s;
    s << ((dynamic_cast<ParseOr const*>(&**i)||
           ((xju::next(i)!=terms_.end())&&
            dynamic_cast<ParseZeroOrMore const*>(&**i)))?
          std::string("(")+at+std::string(")"):
          at);
    x.push_back(s.str());
  }
  return xju::format::join(x.begin(), x.end(), " then ");
}

std::string ParseOr::target() const throw() {
  std::vector<std::string> x;
  for(std::vector<PR>::const_iterator i=terms_.begin(); i!=terms_.end(); ++i) {
    // bracket term if ambiguous (and/or/times)
    std::string const at((*i)->target());
    std::ostringstream s;
    s << ((dynamic_cast<ParseAnd const*>(&**i)||
           ((xju::next(i)!=terms_.end())&&
            dynamic_cast<ParseZeroOrMore const*>(&**i)))?
          std::string("(")+at+std::string(")"):
          at);
    x.push_back(s.str());
  }
  return xju::format::join(x.begin(), x.end(), " or ");
}

std::string ParseNot::target() const throw() {
  return "!"+term_->target();
}

namespace
{
  std::string const end_of_input("end of input");
  Exception EndOfInput(I at, xju::Traced const& trace) throw()
  {
    return Exception(end_of_input, at, trace);
  }
}

class ParseAnyChar : public Parser
{
public:
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw() 
  {
    if (at.atEnd()) {
      return ParseResult(EndOfInput(at, XJU_TRACED));
    }
    return ParseResult(
      std::make_pair(IRs(1U, IR(new hcp_ast::String(at, xju::next(at)))), 
                     xju::next(at)));
  }
  virtual std::string target() const throw() {
    return "any char";
  }
};

// escape newline, tab
std::string printChar(char const c) throw() {
  std::ostringstream s;
  switch(c) {
  case '\t': s << "\\t"; break;
  case '\n': s << "\\n"; break;
  default:
    s << c;
  }
  return s.str();
}


class ParseOneOfChars : public Parser
{
public:
  std::set<char> const chars_;
  
  ~ParseOneOfChars() throw()
  {
  }
  
  explicit ParseOneOfChars(std::string const& chars) throw():
    chars_(chars.begin(), chars.end()) {
  }
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw()
  {
    if (at.atEnd()) {
      return ParseResult(EndOfInput(at, XJU_TRACED));
    }
    if (chars_.find(*at) == chars_.end()) {
      return ParseResult(
        Exception(ParseOneOfChars::unexpected_char, at, XJU_TRACED));
    }
    return ParseResult(
      std::make_pair(
        IRs(1U, new hcp_ast::String(at, xju::next(at))), xju::next(at)));
  }
  // Parser::
  virtual std::string target() const throw()
  {
    std::vector<std::string> x;
    std::transform(chars_.begin(), chars_.end(), 
                   std::back_inserter(x), 
                   printChar);
    std::ostringstream s;
    s << "one of chars [" << xju::format::join(x.begin(), x.end(), "") << "]";
    return s.str();
  }

  static std::string const unexpected_char;
};
std::string const ParseOneOfChars::unexpected_char("unexpected character");

class ParseCharInRange : public Parser
{
  static std::string const not_in_range;
public:
  char const min_;
  char const max_;
  
  explicit ParseCharInRange(char const min, char const max) throw():
    min_(min),
    max_(max) {
  }
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw() 
  {
    if (at.atEnd()) {
      return ParseResult(EndOfInput(at, XJU_TRACED));
    }
    if (((*at) < min_) || ((*at) > max_)) {
      return ParseResult(
        Exception(ParseCharInRange::not_in_range, at, XJU_TRACED));
    }
    return ParseResult(
      std::make_pair(
        IRs(1U, new hcp_ast::String(at, xju::next(at))), xju::next(at)));
  }
  // Parser::
  virtual std::string target() const throw()
  {
    std::ostringstream s;
    s << "one of chars '" << printChar(min_) <<"'..'" << printChar(max_) <<"'";
    return s.str();
  }
};
std::string const ParseCharInRange::not_in_range("character not in range");

class ParseUntil : public Parser
{
public:
  PR const x_;
  
  explicit ParseUntil(PR const x) throw():
    x_(x) {
  }
  
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw() 
  {
    I end(at);
    for(; !end.atEnd(); ++end) {
      ParseResult const r(x_->parse_(end, o));
      if (!r.failed()) {
        xju::Shared<hcp_ast::String> item(new hcp_ast::String(at, end));
        return ParseResult(std::make_pair(IRs(1U, item), end));
      }
    }
    return ParseResult(EndOfInput(end, XJU_TRACED));
  }
  virtual std::string target() const throw() {
    std::ostringstream s;
    s << "up to but not including " << x_->target();
    return s.str();
  }
};

class ParseLiteral : public Parser
{
  static std::string const mismatch;
public:
  std::string const x_;
  
  virtual ~ParseLiteral() throw() {
  }
  
  explicit ParseLiteral(std::string const& x) throw():
    x_(x) {
  }
  
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw()
  {
    try {
      std::pair<std::string::const_iterator, I> x(
        std::mismatch(x_.begin(), x_.end(), at));
      if (x.first != x_.end()) {
        return ParseResult(
          Exception(ParseLiteral::mismatch, x.second, XJU_TRACED));
      }
      return ParseResult(
        std::make_pair(IRs(1U, IR(new hcp_ast::String(at, x.second))),
                       x.second));
    }
    catch(I::EndOfInput const& e) {
      return ParseResult(EndOfInput(e.at_, XJU_TRACED));
    }
  }
  
  
  virtual std::string target() const throw() {
    std::ostringstream s;
    s << "\"" << x_ << "\"";
    return s.str();
  }
};
std::string const ParseLiteral::mismatch("mismatch");

class ParseHash : public Parser
{
public:
  static std::string not_at_column_1;
  static std::string line_does_not_start_with_hash;
  
  virtual ~ParseHash() throw() {
  }
  
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw()
  {
    if (at.atEnd()) {
      return ParseResult(EndOfInput(at, XJU_TRACED));
    }
    if (at.column_ != 1) {
      return ParseResult(
        Exception(ParseHash::not_at_column_1, at, XJU_TRACED));
    }
    if ((*at) != '#') {
      return ParseResult(
        Exception(ParseHash::line_does_not_start_with_hash, at, XJU_TRACED));
    }
    I const nowAt(xju::next(at));
    return ParseResult(
      std::make_pair(IRs(1U, IR(new hcp_ast::String(at, nowAt))), nowAt));
  }
  
  virtual std::string target() const throw() {
    std::ostringstream s;
    s << "'#' at start of line";
    return s.str();
  }
};
std::string ParseHash::not_at_column_1("not at column 1");
std::string ParseHash::line_does_not_start_with_hash("line does not start with '#'");

class ParseBalanced : public Parser
{
public:
  PR const until_;
  bool angles_;
  
  explicit ParseBalanced(PR const until, bool angles) throw():
    until_(until),
    angles_(angles) {
  }
  
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw() 
  {
    xju::Shared<hcp_ast::String> item(new hcp_ast::String(at, at));
    I end(at);
    while(true) {
      ParseResult const r1(until_->parse_(end, o));
      if (!r1.failed()) {
        item->end_=end;
        return ParseResult(std::make_pair(IRs(1U, item), end));
      }
      if (end.atEnd()) {
        return ParseResult(EndOfInput(at, XJU_TRACED));
      }
      switch(*end) {
      case '"':
      {
        ParseResult const r2(stringLiteral->parse_(end, o));
        if (r2.failed()) {
          return r2;
        }
        end=(*r2).second;
      }
      break;
      case '{':
      {
        ParseResult const r2(ParseBalanced(parseOneOfChars("}"), angles_).parse_(xju::next(end),o));
        if (r2.failed()) {
          return r2;
        }
        end=xju::next((*r2).second);
      }
      break;
      case '<':
        if (angles_) {
          ParseResult const r2(
            ParseBalanced(parseOneOfChars(">"), angles_).parse_(
              xju::next(end),o));
          if (r2.failed()) {
            return r2;
          }
          end=xju::next((*r2).second);
        }
        else {
          ++end;
        }
        break;
      case '[':
      {
        ParseResult const r2(
          ParseBalanced(parseOneOfChars("]"), angles_).parse_(
            xju::next(end),o));
        if (r2.failed()) {
          return r2;
        }
        end=xju::next((*r2).second);
      }
      break;
      case '(':
      {
        ParseResult const r2(
          ParseBalanced(parseOneOfChars(")"), angles_).parse_(
            xju::next(end),o));
        if (r2.failed()) {
          return r2;
        }
        end=xju::next((*r2).second);
      }
      break;
      case '/':
      {
        ParseResult const r2(
          comments->parse_(end,o));
        if (!r2.failed()) {
          end=(*r2).second;
        }
        else
        {
          ++end;
        }
      }
      break;
      default:
        ++end;
      }
    }
  }
  virtual std::string target() const throw() {
    std::ostringstream s;
    s << "parse text, balancing (), [], {}, <>, stringLiteral, up to but "
      << "not including " << until_->target();
    return s.str();
  }
};


}

PR parseOneOfChars(std::string const& chars) throw()
{
  return PR(new ParseOneOfChars(chars));
}

PR charInRange(char const min, char const max) throw()
{
  return PR(new ParseCharInRange(min, max));
}

PR parseUntil(PR const x) throw()
{
  return PR(new ParseUntil(x));
}

PR parseLiteral(std::string const& x) throw()
{
  return PR(new ParseLiteral(x));
}

PR operator*(ZeroOrMore const, PR const b) throw()
{
  return PR(new ParseZeroOrMore(b));
}

PR balanced(PR until, bool angles) throw()
{
  return PR(new ParseBalanced(until, angles));
}

PR operator+(PR a, PR b) throw()
{
  xju::Shared<ParseAnd> result(new ParseAnd);
  if (a->isA<ParseAnd>()) {
    ParseAnd const& x(a->asA<ParseAnd>());
    std::copy(x.terms_.begin(),
              x.terms_.end(),
              std::back_inserter(result->terms_));
  }
  else {
    result->asA<ParseAnd>().terms_.push_back(a);
  }
  if (b->isA<ParseAnd>()) {
    ParseAnd const& x(b->asA<ParseAnd>());
    std::copy(x.terms_.begin(),
              x.terms_.end(),
              std::back_inserter(result->terms_));
  }
  else {
    result->asA<ParseAnd>().terms_.push_back(b);
  }
  return result;
}

PR operator|(PR a, PR b) throw()
{
  xju::Shared<ParseOr> result(new ParseOr);
  if (a->isA<ParseOr>()) {
    ParseOr const& x(a->asA<ParseOr>());
    std::copy(x.terms_.begin(), x.terms_.end(),
              std::back_inserter(result->terms_));
  }
  else {
    result->terms_.push_back(a);
  }
  if (b->isA<ParseOr>()) {
    ParseOr const& x(b->asA<ParseOr>());
    std::copy(x.terms_.begin(), x.terms_.end(),
              std::back_inserter(result->terms_));
  }
  else {
    result->terms_.push_back(b);
  }
  return result;
}

PR operator!(PR x) throw()
{
  return xju::Shared<ParseNot>(new ParseNot(x));
}

PR operator*(AtLeastOne a, PR b) throw()
{
  std::ostringstream s;
  s << "at least one occurrance of " << b->target();
  return PR(new NamedParser<hcp_ast::CompositeItem>(s.str(),
                                                    b+(zeroOrMore*b)));
}

PR parseAnyChar(new ParseAnyChar);

PR whitespaceChar(parseOneOfChars(" \t\n"));
PR parseHash(new ParseHash);

PR lineComment(new NamedParser<hcp_ast::LineComment>(
  "line comment",
  parseLiteral("//")+
  parseUntil(parseOneOfChars("\n"))+
  (zeroOrMore*whitespaceChar)));

PR blockComment(new NamedParser<hcp_ast::BlockComment>(
  "block comment",
  parseLiteral("/*")+
  parseUntil(parseLiteral("*/"))+
  (zeroOrMore*whitespaceChar)));

PR comments(new NamedParser<hcp_ast::Comments>(
  "comments",
  atLeastOne*(lineComment|blockComment)));

// matches nothing or something
PR eatWhite(zeroOrMore*(whitespaceChar|comments));

PR nonBackslashDoubleQuote(new ParseUntil(parseOneOfChars("\\\"")));
PR nonDoubleQuote(new ParseUntil(parseOneOfChars("\"")));

PR doubleQuote(parseOneOfChars("\""));
PR backslash(parseOneOfChars("\\"));
PR oneChar(new ParseAnyChar);

// REVISIT: rework to make more sense
// string literal up to first double quote (does not include
// next-line continuations)
PR stringLiteralAtom(
  doubleQuote+
  (zeroOrMore*(nonBackslashDoubleQuote+backslash+doubleQuote))+
  nonDoubleQuote+
  doubleQuote);

PR stringLiteralContinuation(
  (zeroOrMore*parseOneOfChars(" \t"))+parseOneOfChars("\n")+
  eatWhite+stringLiteralAtom);

PR stringLiteral(new NamedParser<hcp_ast::StringLiteral>(
  "string literal",
  stringLiteralAtom+
  zeroOrMore*stringLiteralContinuation+
  eatWhite));

//
// to be able to split a combined .h and .cpp (ie a .hcp) file into
// .h and .cpp parts, we need to choose whether each #include goes
// into the .h or the .cpp; we use a simple convention of 
// adding //impl to the #include line, eg
//   #include <x.h> //impl
// ... which indicates #include <x.h> should go in the .cpp not the .h
//
PR hashIncludeImplMarker(
  parseLiteral("//")+
  (zeroOrMore*parseOneOfChars(" \t"))+
  parseLiteral("impl")+
  (zeroOrMore*parseOneOfChars(" \t"))+
  parseLiteral("\n"));

PR hashIncludeCommon(
    parseHash+
    (zeroOrMore*parseOneOfChars(" \t"))+
    parseLiteral("include")+
    parseUntil(parseLiteral("\n")|hashIncludeImplMarker));

PR hashInclude(new NamedParser<hcp_ast::HashInclude>(
  "#include",
  hashIncludeCommon+
  parseOneOfChars("\n")+
  eatWhite));
  
PR hashIncludeImpl(new NamedParser<hcp_ast::HashIncludeImpl>(
  "#include with //impl marker",
  hashIncludeCommon+
  hashIncludeImplMarker+
  eatWhite));

PR hash(new NamedParser<hcp_ast::OtherPreprocessor>(
  "other preprocessor directive",
  parseHash+
  parseUntil(parseOneOfChars("\n"))+
  eatWhite));

PR whitespace(new NamedParser<hcp_ast::Whitespace>(
  "some whitespace",
  atLeastOne*whitespaceChar));

PR typedef_statement(new NamedParser<hcp_ast::Typedef>(
  "typedef statement",
  parseLiteral("typedef")+
  whitespace+
  balanced(parseOneOfChars(";"))+
  parseOneOfChars(";")+
  eatWhite));

PR using_statement(new NamedParser<hcp_ast::Using>(
  "using statement",
  parseLiteral("using")+
  whitespace+
  balanced(parseOneOfChars(";"))+
  parseOneOfChars(";")+
  eatWhite));

PR enum_def(new NamedParser<hcp_ast::EnumDef>(
  "enum definition",
  parseLiteral("enum")+
  whitespace+
  balanced(parseOneOfChars(";"))+
  parseOneOfChars(";")+
  eatWhite));

PR bracketed(parseLiteral("(")+balanced(parseLiteral(")"))+parseLiteral(")"));

PR unqualifiedName(
  atLeastOne*(charInRange('a', 'z')|
              charInRange('A', 'Z')|
              charInRange('0', '9')|
              parseOneOfChars("_")));

PR operator_name(
  parseLiteral("operator")+
  eatWhite+
  (parseLiteral("<<")|
   parseLiteral(">>")|
   parseLiteral("==")|
   parseLiteral("<=")|
   parseLiteral(">=")|
   parseLiteral("<")|
   parseLiteral(">")|
   parseLiteral("++")|
   parseLiteral("--")|
   parseLiteral("->")|
   parseLiteral("+")|
   parseLiteral("-")|
   parseLiteral("|=")|
   parseLiteral("&=")|
   parseLiteral("|")|
   parseLiteral("&")|
   parseLiteral("[]")|
   parseLiteral("!")|
   parseLiteral("%=")|
   parseLiteral("%")|
   parseLiteral("=")|
   parseLiteral("*"))+
  eatWhite);
   
PR name(
  zeroOrMore*(unqualifiedName+eatWhite+parseLiteral("::")+eatWhite)+
  unqualifiedName+
  eatWhite);

PR function_qualifiers(
  new NamedParser<hcp_ast::FunctionQualifiers>(
    "function qualifiers",
    zeroOrMore*((parseLiteral("virtual")|
                 parseLiteral("explicit")|
                 parseLiteral("friend"))+eatWhite)));
                
PR function_proto(
  function_qualifiers+
  balanced(parseOneOfChars("();{}[]")|
           ((operator_name|name)+parseOneOfChars("(")))+
  new NamedParser<hcp_ast::FunctionName>(
  "function name",
  (operator_name|name))+
  bracketed+
  balanced(parseOneOfChars("{;:")|
           (parseLiteral("try")+eatWhite+parseOneOfChars(":"))));

PR function_decl(new NamedParser<hcp_ast::FunctionDecl>(
  "function declaration",
  function_proto+
  parseOneOfChars(";")+
  eatWhite));

PR template_preamble(
  parseLiteral("template")+
  eatWhite+
  parseOneOfChars("<")+
  balanced(parseOneOfChars(">"), true)+
  parseOneOfChars(">")+
  eatWhite);

PR block(new NamedParser<hcp_ast::Block>(
  "block",
  parseLiteral("{")+
  balanced(parseOneOfChars("}"))+
  parseLiteral("}")));

PR init_list(new NamedParser<hcp_ast::InitList>(
  "initialiser list",
  parseLiteral(":")+
  balanced(parseOneOfChars("{;:"))));

PR catch_block(parseLiteral("catch")+eatWhite+bracketed+eatWhite+block+
               eatWhite);

PR function_def(new NamedParser<hcp_ast::FunctionDef>(
  "function definition",
  function_proto+
  new NamedParser<hcp_ast::FunctionImpl>(
    "function implementation",
    (zeroOrMore*(parseLiteral("try")+eatWhite)+
     zeroOrMore*(init_list+eatWhite)+
     block+eatWhite+
     zeroOrMore*catch_block))));

PR template_function_def(new NamedParser<hcp_ast::TemplateFunctionDef>(
  "template function definition",
  atLeastOne*template_preamble+
  function_def));

PR class_proto(
  zeroOrMore*template_preamble+
  (parseLiteral("class")|parseLiteral("struct")|parseLiteral("union"))+
  whitespace+
  new NamedParser<hcp_ast::ClassName>(
  "class name",
  name)+
  balanced(parseOneOfChars("{;")));

PR class_decl(new NamedParser<hcp_ast::ClassForwardDecl>(
  "class forward-declaration",
  class_proto+
  parseOneOfChars(";")));

PR attr_decl(new NamedParser<hcp_ast::AttrDecl>(
  "object declaration",
  balanced(parseOneOfChars("{}();"))+
  parseOneOfChars(";")+
  eatWhite));

PR access_modifier(new NamedParser<hcp_ast::AccessModifier>(
  "public/private/protected: marker",
  (parseLiteral("public")|
   parseLiteral("private")|
   parseLiteral("protected"))+
  eatWhite+
  parseOneOfChars(":")+
  eatWhite));

namespace
{
class SelfParser : public Parser
{
public:
  Parser& self_;
  
  explicit SelfParser(Parser& self) throw():
    self_(self) {
  }

  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw() 
  {
    return self_.parse_(at, o);
  }

  // Parser::
  virtual std::string target() const throw() {
    return self_.target();
  }
};

class ParseClass : public Parser
{
public:
  PR x_;
  PR tp_;
  PR p_;
  
  ParseClass() throw():
    x_(class_proto+
       parseOneOfChars("{")+
       eatWhite+
       zeroOrMore*(comments|
                   function_decl|
                   template_function_def|
                   access_modifier|
                   class_decl|
                   enum_def|
                   typedef_statement|
                   PR(new SelfParser(*this))|
                   function_def|
                   attr_decl)+
       parseOneOfChars("}")+
       eatWhite+
       parseOneOfChars(";")+
       eatWhite),
    tp_(new NamedParser<hcp_ast::TemplateClassDef>(
      "template class definition",
      template_preamble+
      x_)),
    p_(new NamedParser<hcp_ast::ClassDef>(
      "non-template class definition",
      x_))
  {
  }
       
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw() 
  {
    return (tp_|p_)->parse_(at, o);
  }

  // Parser::
  virtual std::string target() const throw() {
    return "class definition";
  }
  
};
}

PR class_def(new ParseClass);

PR namespace_leaf(
  comments|
  hashIncludeImpl|
  hashInclude|
  hash|
  class_def| // note recursive
  typedef_statement|
  using_statement|
  enum_def|
  whitespace|
  function_decl| // inc. template
  template_function_def|
  function_def|
  attr_decl);

PR anonymous_namespace(new NamedParser<hcp_ast::AnonymousNamespace>(
  "anonymous namespace",
  parseLiteral("namespace")+
  eatWhite+
  parseOneOfChars("{")+
  eatWhite+
  (zeroOrMore*namespace_leaf)+
  parseOneOfChars("}")+
  eatWhite));

namespace
{
class ParseNamespace : public Parser
{
public:
  PR x_;
  
  ParseNamespace() throw():
    x_(parseLiteral("namespace")+
       whitespace+
       new NamedParser<hcp_ast::NamespaceName>(
         "namespace name",
         parseUntil(parseOneOfChars("(){};")))+
       parseOneOfChars("{")+
       eatWhite+
       new NamedParser<hcp_ast::NamespaceMembers>(
         "namespace members",
         zeroOrMore*(namespace_leaf|
                     anonymous_namespace|
                     PR(new SelfParser(*this))))+
       eatWhite+
       parseOneOfChars("}")+
       eatWhite) {
  }

  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw() 
  {
    return x_->parse_(at, o);
  }

  // Parser::
  virtual std::string target() const throw() {
    return "namespace";
  }
};

}

PR namespace_def(new NamedParser<hcp_ast::NamespaceDef>(
  "namespace",
  new ParseNamespace));

namespace
{
class ParseEndOfFile : public Parser
{
public:
  static std::string const expected_end_of_input;
  
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw() 
  {
    if (!at.atEnd()) {
      return ParseResult(
        Exception(ParseEndOfFile::expected_end_of_input, at, XJU_TRACED));
    }
    return ParseResult(
      std::make_pair(
        IRs(1U, new hcp_ast::EndOfFile(IRs(1U, new hcp_ast::String(at, at)))),
        at));
  }


  // Parser::
  virtual std::string target() const throw() {
    return "end of file";
  }
};
std::string const ParseEndOfFile::expected_end_of_input("expected end of input");
}

PR endOfFile(new ParseEndOfFile);

PR file(new NamedParser<hcp_ast::File>(
  "file",
  eatWhite+
  zeroOrMore*(anonymous_namespace|
              namespace_def|
              namespace_leaf)+
  endOfFile));

/*
splitting:
- anon namespace moves to .cc
- extern marks global object
- template class/fn stays in .hh
- generate include guard from nr levels, generate .cc include using same
  nr levels

*/
}
