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

#include <vector>
#include <algorithm>
#include <sstream>
#include <xju/format.hh>
#include <xju/JoiningIterator.hh>
#include <hcp/translateException.hh>
#include <hcp/trace.hh>

namespace hcp_parser
{

PR::PR(std::string const& literal) /*throw(std::bad_alloc)*/:
    std::shared_ptr<Parser>(parseLiteral(literal))
{
}
PR::PR(const char literal[]) /*throw(std::bad_alloc)*/:
    std::shared_ptr<Parser>(parseLiteral(literal))
{
}

void Exception::addContext(Parser const& parser, I at, xju::Traced const& trace) throw()
{
  bool const isNamedParser(dynamic_cast<hcp_parser::NamedParser_ const*>(&parser));
  context_.push_back(std::make_pair(std::make_pair(
                                      std::make_pair(isNamedParser,
                                                     parser.target()), at), trace));
}

namespace
{
std::string contextReadableRepr(
  std::pair<std::pair<std::pair<bool, std::string>, I>, xju::Traced> const& c) throw()
{
  std::ostringstream s;
  s << c.first.second << ": failed to parse " << c.first.first.second;
  return s.str();
}
}
  
std::string readableRepr(Exception const& e) throw()
{
  std::ostringstream s;
  if (e.context_.size()==0) {
    s << e.at_ << ": " << e.cause_->str();
  }
  else {
    std::vector<std::string> context;
    std::transform(e.context_.rbegin(), e.context_.rend(),
                   xju::JoiningIterator<std::string, std::string>(
                     s,
                     " because\n  "),
                   std::ptr_fun(contextReadableRepr));
    s << " because\n  "
      << e.at_ << ": " << e.cause_->str();
  }
  return s.str();
}

namespace
{
class Optional;
class ParseOr;
class ParseAnd;
class ParseNot;
class ParseZeroOrMore;

class Optional : public Parser
{
public:
  PR x_;
  
  virtual ~Optional() throw() {}
  
  explicit Optional(PR x) throw():
    x_(x) {
  }
  
  // Parser::
  virtual ParseResult parse_(I const at, Options const& options) throw() 
  {
    PV result(IRs(), at);
    ParseResult const r(x_->parse(result.second, options));
    if (!r.failed()) {
      PV const x(*r);
      std::copy(x.first.begin(), x.first.end(),
                std::back_inserter(result.first));
      result.second=x.second;
    }
    return ParseResult(result);
  }

  // Parser::
  virtual std::string target() const throw();
};
class ParseZeroOrMore : public Parser
{
public:
  PR const x_;
  
  virtual ~ParseZeroOrMore() throw() {}
  
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
  
  virtual ~ParseAnd() throw() {}
  
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
      ParseResult br((*i)->parse(result.second, options));
      if (br.failed()) {
        if (options.irsAtEnd_) {
          br.addAtEndIRs(result.first);
        }
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
  
  virtual ~ParseOr() throw() {}
  

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
    if (options.trace_) {
      std::ostringstream s;
      s << "ParseOr choosing exception of " 
        << (*(*failures.rbegin()).second.context_.rbegin()).first.first.second
        << " which got to " << (*failures.rbegin()).first;
      hcp_trace::milestone(s.str(), XJU_TRACED);
    }
    return ParseResult((*failures.rbegin()).second);
  }

  // Parser::
  virtual std::string target() const throw();
};

class ParseNot : public Parser
{
  static std::shared_ptr<Exception::Cause const> const expected_parse_failure;
public:
  explicit ParseNot(PR term) throw():
    term_(term)
  {
  }
  PR term_;
  
  virtual ~ParseNot() throw() {}
  

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
std::shared_ptr<Exception::Cause const> const ParseNot::expected_parse_failure(
  new FixedCause("expected parse failure"));

std::string Optional::target() const throw()
{
  // bracket lhs/rhs if ambiguous (and/or/times)
  std::string const xt(x_->target());
  std::ostringstream s;
  s << "optional " 
    << ((dynamic_cast<ParseOr const*>(&*x_)||
         dynamic_cast<ParseAnd const*>(&*x_))?
        std::string("(")+xt+std::string(")"):
        xt);
  return s.str();
}

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

std::shared_ptr<Exception::Cause const> const end_of_input(
  new FixedCause("end of input"));

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
      std::make_pair(IRs(1U, IR(new hcp_ast::Item(at, xju::next(at)))), 
                     xju::next(at)));
  }
  virtual std::string target() const throw() {
    return "any char";
  }
};

class ParseOneOfChars : public Parser
{
public:
  std::set<char> const chars_;
  
  ~ParseOneOfChars() throw() {}
  
  
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
        Exception(
          std::shared_ptr<Exception::Cause const>(new UnexpectedChar(at, chars_)), 
          at, XJU_TRACED));
    }
    return ParseResult(
      std::make_pair(
        IRs(1U, IR(new hcp_ast::Item(at, xju::next(at)))), xju::next(at)));
  }
  // Parser::
  virtual std::string target() const throw()
  {
    std::ostringstream s;
    s << "one of chars "
      << xju::format::quote(xju::format::cEscapeString(
                              std::string(chars_.begin(),chars_.end())));
    return s.str();
  }

  class UnexpectedChar : public Exception::Cause
  {
  public:
    UnexpectedChar(I const at, std::set<char> const& chars) throw():
        at_(at),
        chars_(chars) {
    }
    ~UnexpectedChar() throw() {}
    
    std::string str() const throw()
    {
      std::ostringstream s;
      s << "'" << (*at_) << "'" << " is not one of chars "
        << xju::format::quote(xju::format::cEscapeString(
                                std::string(chars_.begin(),chars_.end())));
      return s.str();
    }
    I const at_;
    std::set<char> const chars_;
  };

};

class ParseOneOfChars2 : public Parser
{
public:
  hcp::Chars const chars_;
  
  ~ParseOneOfChars2() throw() {}
  
  
  explicit ParseOneOfChars2(hcp::Chars const chars) throw():
      chars_(std::move(chars)){
  }
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw()
  {
    if (at.atEnd()) {
      return ParseResult(EndOfInput(at, XJU_TRACED));
    }
    if (!chars_.bits().test((uint8_t)*at)) {
      return ParseResult(
        Exception(
          std::shared_ptr<Exception::Cause const>(new UnexpectedChar(at, chars_)), 
          at, XJU_TRACED));
    }
    return ParseResult(
      std::make_pair(
        IRs(1U, IR(new hcp_ast::Item(at, xju::next(at)))), xju::next(at)));
  }
  // Parser::
  virtual std::string target() const throw()
  {
    std::ostringstream s;
    s << "one of chars " << chars_;
    return s.str();
  }

  class UnexpectedChar : public Exception::Cause
  {
  public:
    UnexpectedChar(I const at, hcp::Chars const& chars) throw():
        at_(at),
        chars_(chars) {
    }
    ~UnexpectedChar() throw() {}
    
    std::string str() const throw()
    {
      std::ostringstream s;
      s << "'" << (*at_) << "'" << " is not one of chars " << chars_;
      return s.str();
    }
    I const at_;
    hcp::Chars const& chars_;
  };

};

class ParseAnyCharExcept : public Parser
{
public:
  std::set<char> const chars_;
  
  ~ParseAnyCharExcept() throw() {}
  
  
  explicit ParseAnyCharExcept(std::string const& chars) throw():
    chars_(chars.begin(), chars.end()) {
  }
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw()
  {
    if (at.atEnd()) {
      return ParseResult(EndOfInput(at, XJU_TRACED));
    }
    if (chars_.find(*at) != chars_.end()) {
      return ParseResult(
        Exception(
          std::shared_ptr<Exception::Cause const>(new UnexpectedChar(at,chars_)), 
          at, XJU_TRACED));
    }
    return ParseResult(
      std::make_pair(
        IRs(1U, IR(new hcp_ast::Item(at, xju::next(at)))), xju::next(at)));
  }
  // Parser::
  virtual std::string target() const throw()
  {
    std::ostringstream s;
    s << "any char except " 
      << xju::format::quote(xju::format::cEscapeString(
                              std::string(chars_.begin(),chars_.end())));
    return s.str();
  }

  class UnexpectedChar : public Exception::Cause
  {
  public:
    UnexpectedChar(I const at, std::set<char> const& chars) throw():
        at_(at),
        chars_(chars) {
    }
    ~UnexpectedChar() throw() {}
    
    std::string str() const throw()
    {
      std::ostringstream s;
      s << "'" << (*at_) << "'" << " is one of "
        << xju::format::quote(xju::format::cEscapeString(
                                std::string(chars_.begin(),chars_.end())));
      return s.str();
    }
    I const at_;
    std::set<char> const chars_;
  };

};

class ParseCharInRange : public Parser
{
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
        Exception(
          std::shared_ptr<Exception::Cause const>(
            new CharNotInRange(at, min_, max_)),
          at, XJU_TRACED));
    }
    return ParseResult(
      std::make_pair(
        IRs(1U, IR(new hcp_ast::Item(at, xju::next(at)))), xju::next(at)));
  }
  // Parser::
  virtual std::string target() const throw()
  {
    std::ostringstream s;
    s << "one of chars '"
      << xju::format::cEscapeChar(min_) <<"'..'"
      << xju::format::cEscapeChar(max_) <<"'";
    return s.str();
  }
  class CharNotInRange : public Exception::Cause
  {
  public:
    CharNotInRange(I const at, char const min, char const max) throw():
        at_(at),
        min_(min),
        max_(max) {
    }
    ~CharNotInRange() throw() {}
    
    std::string str() const throw()
    {
      std::ostringstream s;
      s << "'" << xju::format::cEscapeChar(*at_) << "'"
        << " is not one of chars '" 
        << xju::format::cEscapeChar(min_) <<"'..'"
        << xju::format::cEscapeChar(max_) <<"'";
      return s.str();
    }
    I const at_;
    char const min_;
    char const max_;
  };

};

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
    do{
      ParseResult r(x_->parse_(end, o));
      if (!r.failed()){
        std::shared_ptr<hcp_ast::Item> item(new hcp_ast::Item(at, end));
        return ParseResult(std::make_pair(IRs(1U, item), end));
      }
      if (end.atEnd()) {
        return ParseResult(EndOfInput(end, XJU_TRACED));
      }
      ++end;
    }
    while(true);
  }
  virtual std::string target() const throw() {
    std::ostringstream s;
    s << "up to but not including " << x_->target();
    return s.str();
  }
};

class ParseSpecificUntil : public Parser
{
public:
  PR const match_;
  PR const x_;
  
  explicit ParseSpecificUntil(PR match, PR const x) throw():
    match_(match),
    x_(x) {
  }
  
  // Parser::
  virtual ParseResult parse_(I const at, Options const& options) throw() 
  {
    PV result(IRs(), at);
    while(true) {
      ParseResult const re(x_->parse_(result.second, options));
      if (!re.failed()) {
        return ParseResult(result);
      }
      ParseResult r(match_->parse_(result.second, options));
      if (r.failed()) {
        if (options.irsAtEnd_) {
          r.addAtEndIRs(result.first);
        }
        return r;
      }
      PV const x(*r);
      std::copy(x.first.begin(), x.first.end(),
                std::back_inserter(result.first));
      result.second=x.second;
    }
  }
  virtual std::string target() const throw() {
    std::ostringstream s;
    s << match_->target() << "'s up to but not including " << x_->target();
    return s.str();
  }
};

class ParseLiteral : public Parser
{
public:
  std::string const x_;
  
  virtual ~ParseLiteral() throw() {}
  
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
          Exception(
            std::shared_ptr<Exception::Cause const>(
              new Mismatch(*x.second, *x.first)),
            x.second, XJU_TRACED));
      }
      return ParseResult(
        std::make_pair(IRs(1U, IR(new hcp_ast::Item(at, x.second))),
                       x.second));
    }
    catch(I::EndOfInput const& e) {
      return ParseResult(EndOfInput(e.at_, XJU_TRACED));
    }
  }
  
  
  virtual std::string target() const throw() {
    std::ostringstream s;
    s << (x_=="\\"?std::string("backslash"):xju::format::quote(x_));
    return s.str();
  }

  class Mismatch : public Exception::Cause
  {
  public:
    Mismatch(char const got, char const wanted) throw():
        got_(got),
        wanted_(wanted)
    {
    }
    ~Mismatch() throw() {}
    
    std::string str() const throw()
    {
      std::ostringstream s;
      
      s << "expected "
        << (wanted_=='\\'?std::string("backslash"):
            xju::format::quote("'",xju::format::cEscapeChar(wanted_)))
        << " but found "
        << (got_=='\\'?std::string("backslash"):
            xju::format::quote("'",xju::format::cEscapeChar(got_)));
      return s.str();
    }
    char const got_;
    char const wanted_;
  };

};
bool isIdentifierChar(char c) throw(){
  return ('a' <= c && c <= 'z')||
    ('A' <= c && c <= 'Z') ||
    (c=='_');
}
bool isIdentifierContChar(char c) throw(){
  return isIdentifierChar(c)||
    ('0' <= c && c <= '9');
}

class ParseIdentifier : public Parser
{
public:
  virtual ~ParseIdentifier() throw() {}
  
  
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw()
  {
    I i(at);
    if (i.atEnd()){
      return ParseResult(EndOfInput(i, XJU_TRACED));
    }
    if(!isIdentifierChar(*i)){
      return ParseResult(
        Exception(
          std::shared_ptr<Exception::Cause const>(
            new NotIdentifierChar(*i)),
          i, XJU_TRACED));
    }
    ++i;
    while(!i.atEnd()&&isIdentifierContChar(*i)){
      ++i;
    }
    return ParseResult(
      std::make_pair(IRs(1U, IR(new hcp_ast::Item(at, i))),
                     i));
  }
  
  virtual std::string target() const throw() {
    return "identifier";
  }

  class NotIdentifierChar : public Exception::Cause
  {
  public:
    explicit NotIdentifierChar(char const got) throw():
        got_(got)
    {
    }
    ~NotIdentifierChar() throw() {}
    
    std::string str() const throw()
    {
      std::ostringstream s;
      s << "'" << xju::format::cEscapeChar(got_) << "' is not an identifier char";
      return s.str();
    }
    char const got_;
  };

};

class ParseHash : public Parser
{
public:
  static std::shared_ptr<Exception::Cause const> not_at_column_1;
  
  virtual ~ParseHash() throw() {}
  
  
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
        Exception(
          std::shared_ptr<Exception::Cause>(
            new NotHash(*at)), at, XJU_TRACED));
    }
    I const nowAt(xju::next(at));
    return ParseResult(
      std::make_pair(IRs(1U, IR(new hcp_ast::Item(at, nowAt))), nowAt));
  }
  
  virtual std::string target() const throw() {
    std::ostringstream s;
    s << "'#' at start of line";
    return s.str();
  }

  class NotHash : public Exception::Cause
  {
  public:
    NotHash(char const got) throw():
        got_(got)
    {
    }
    ~NotHash() throw() {}
    
    std::string str() const throw()
    {
      std::ostringstream s;
      s << "line starts with '" << xju::format::cEscapeChar(got_)
        << "', not '#'";
      return s.str();
    }
    char const got_;
  };
    

};
std::shared_ptr<Exception::Cause const> ParseHash::not_at_column_1(
  new FixedCause("not at column 1"));

PR oneChar() throw()
{
  static PR oneChar(new ParseAnyChar);
  return oneChar;
}



PR stringEscapeSequence() throw()
{
  static PR stringEscapeSequence(
    parseLiteral("\\")+(
      parseOneOfChars("'\"?\\abfnrtv")|
      (octalDigit()+octalDigit()+octalDigit())|
      (octalDigit()+octalDigit())|
      octalDigit()|
      (parseLiteral("x")+atLeastOne(hexDigit()))));
  return stringEscapeSequence;
}


PR s_char() throw()
{
  static PR s_char(
    parseAnyCharExcept("\\\"\n")|
    stringEscapeSequence());
  return s_char;
}

PR c_char() throw()
{
  static PR s_char(
    parseAnyCharExcept("\\'\n")|
    stringEscapeSequence());
  return s_char;
}

class ParseRawStringLiteral : public Parser
{
public:
  ParseRawStringLiteral() noexcept
  : r_(parseLiteral("R\"")),
    o_((parseLiteral("u8")+r_)|
       (parseOneOfChars("LuU")+r_)|
       r_)
  {
  }
  PR r_;
  PR o_;
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw() 
  {
    try{
      auto r{o_->parse(at,o)};
      if (r.failed()){
        return r;
      }
      I i{(*r).second};
      int c{0};
      for(; (c<16) && (*i != '('); ++i,++c){
        if (std::isspace(*i) || *i == '\\' || *i == ')'){
          return ParseResult(
            Exception(
              std::shared_ptr<Exception::Cause const>(
                new InvalidDelimeterChar(i)),
              i, XJU_TRACED));
        }
      }
      if (*i != '('){
        return ParseResult(
          Exception(
            std::shared_ptr<Exception::Cause const>(
              new TooManyDelimeterChars()),
            i, XJU_TRACED));
      }
      std::string const endDelimeter{
        ")"+std::string((*r).second.x_,i.x_)+"\""};
      PR l{parseLiteral(endDelimeter)};
      auto rr{parseUntil(l)->parse(i,o)};
      if (rr.failed()){
        return ParseResult(
          Exception(
            std::shared_ptr<Exception::Cause const>(
              new EndDelimeterNotFound(endDelimeter)),
            rr.e().at_, XJU_TRACED));
      }
      auto rrr{l->parse((*rr).second,o)};
      return ParseResult(PV(IRs({IR(new hcp_ast::Item(at,(*rrr).second))}),
                            (*rrr).second));
    }
    catch(I::EndOfInput const& x){
      return ParseResult(EndOfInput(x.at_, XJU_TRACED));
    }
  }
  // Parser::
  virtual std::string target() const throw() {
    return "raw string literal";
  }

  class InvalidDelimeterChar : public Exception::Cause
  {
  public:
    InvalidDelimeterChar(I const at) throw():
        at_(at) {
    }
    ~InvalidDelimeterChar() throw() {}
    
    std::string str() const throw()
    {
      std::ostringstream s;
      s << "'" << (*at_) << "'"
        << " is not allowed in raw string literal delimeter";
      return s.str();
    }
    I const at_;
  };

  class TooManyDelimeterChars : public Exception::Cause
  {
  public:
    TooManyDelimeterChars() throw()
    {
    }
    ~TooManyDelimeterChars() throw() {}
    
    std::string str() const throw()
    {
      std::ostringstream s;
      s << "too many (more than 16) delimeter chars in raw string literal delimeter";
      return s.str();
    }
  };

  class EndDelimeterNotFound : public Exception::Cause
  {
  public:
    explicit EndDelimeterNotFound(std::string const& endDelimeter) throw():
        endDelimeter_(endDelimeter) {
    }
    ~EndDelimeterNotFound() throw() {}
    
    std::string str() const throw()
    {
      std::ostringstream s;
      s << "raw string literal end delimeter "
        << xju::format::quote(endDelimeter_)
        << " not found";
      return s.str();
    }
    std::string const endDelimeter_;
  };

};

namespace
{
bool lookingAt(I i,std::string const& x) noexcept
{
  auto xi{x.begin()};
  for(; !i.atEnd() && (xi!=x.end()) && (*i == *xi); ++i, ++xi);
  return (xi==x.end());
}

}

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
    I end(at);
    while(true) {
      ParseResult const r1(until_->parse_(end, o));
      if (!r1.failed()) {
        return ParseResult(
          std::make_pair(IRs(1U,IR(new hcp_ast::Item(at, end))),end));
      }
      if (end.atEnd()) {
        return ParseResult(EndOfInput(end, XJU_TRACED));
      }
      if (lookingAt(end,"u8R\"")||
          lookingAt(end,"uR\"")||
          lookingAt(end,"UR\"")||
          lookingAt(end,"LR\"")||
          lookingAt(end,"R\""))
      {
        ParseResult const r2(rawStringLiteral()->parse_(end, o));
        if (r2.failed()) {
          return r2;
        }
        end=(*r2).second;
      }
      else{
        switch(*end) {
        case '\'':
        {
          ParseResult const r2(
            (parseOneOfChars("'")+c_char()+parseOneOfChars("'"))->parse_(
              end, o));
          if (r2.failed()) {
            return r2;
          }
          end=(*r2).second;
        }
        break;
        case '"':
        {
          ParseResult const r2(stringLiteral()->parse_(end, o));
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
            comments()->parse_(end,o));
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
  }
  virtual std::string target() const throw() {
    std::ostringstream s;
    s << "parse text, balancing (), [], {}, <>, stringLiteral, up to but "
      << "not including " << until_->target();
    return s.str();
  }
};

class AnonParser : public NamedParser_
{
public:
  std::string const name_;
  PR x_;
  
  virtual ~AnonParser() throw() {}
  
  explicit AnonParser(std::string const& name, PR const x) throw():
    name_(name),
    x_(x) {
  }

  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw() 
  {
    ParseResult r(x_->parse(at, o));
    if (!r.failed()) {
      return r;
    }
    return r;
  }
  // Parser::
  virtual std::string target() const throw() {
    return name_;
  }
};


}

Exception EndOfInput(I at, xju::Traced const& trace) throw()
{
  return Exception(end_of_input, at, trace, true);
}

ParseResult Parser::parse(I const at, Options const& options) throw() 
{
  std::unique_ptr<hcp_trace::Scope> scope;
  if (options.trace_ && traced_) {
    std::ostringstream s;
    s << "parse " << target() << " at " << at;
    scope = std::unique_ptr<hcp_trace::Scope>(
      new hcp_trace::Scope(s.str(), XJU_TRACED));
  }
  CacheKey const k(at, this);
  CacheVal::const_iterator i((*options.cache_).find(k));
  if (i == (*options.cache_).end()) {
    ParseResult result(parse_(at, options));
    if (result.failed()) {
      result.addContext(*this, at, XJU_TRACED);
    }
    CacheVal::value_type v(k, result);
    i=(*options.cache_).insert(v).first;
  }
  else{
    if (scope.get()){
      scope->cached();
    }
  }
  if (scope.get()) {
    if ((*i).second.failed()){
      scope->fail();
    }
    else{
      scope->result(reconstruct((*((*i).second)).first));
    }
  }
  return (*i).second;
}

class NonEmptyListOf : public Parser
{
public:
  explicit NonEmptyListOf(PR x, PR moreIndicator) throw()
      : x_(x),
        moreIndicator_(moreIndicator)
  {
  }

  // Parser::
  virtual ParseResult parse_(I const at, Options const& options) throw() 
  {
    IRs irs;
    I i(at);
    PV result(IRs(), at);
    while(true){
      ParseResult x(x_->parse(i,options));
      if (x.failed()) {
        if (options.irsAtEnd_) {
          x.addAtEndIRs(irs);
        }
        return x; // failed
      }
      std::copy((*x).first.begin(),(*x).first.end(),
                std::back_inserter(irs));
      i=(*x).second;

      ParseResult const more(moreIndicator_->parse(i,options));
      if (more.failed()){
        return ParseResult(PV(irs,i)); //success (no more)
      }
      std::copy((*more).first.begin(),(*more).first.end(),
                std::back_inserter(irs));
      i=(*more).second;
    }
  }

  // Parser::
  virtual std::string target() const throw() {
    return "non-empty list";
  }
  PR const x_;
  PR const moreIndicator_;
};

PR nonEmptyListOf(PR x, PR moreIndicator) throw()
{
  return PR(new NonEmptyListOf(x,moreIndicator));
}

PR listOf(PR x, PR separator, PR terminator) throw()
{
  return anon(separator->target()+"-separated list of "+x->target()+
              " terminated by "+terminator->target(),
              terminator|
              (x+parseUntil(separator+x,terminator)+terminator));
}

PR listOf(PR opener, PR x, PR separator, PR terminator) throw()
{
  return anon(separator->target()+"-separated list of "+x->target()+
              " commencing with "+opener->target()+
              " and terminated by "+terminator->target(),
              opener+(
                terminator|
                (x+parseUntil(separator+x,terminator)+terminator)));
}

ZeroOrMore zeroOrMore() throw()
{
  static ZeroOrMore zeroOrMore;
  return zeroOrMore;
}

PR operator*(ZeroOrMore const, PR const b) throw()
{
  return PR(new ParseZeroOrMore(b));
}

PR operator+(PR a, PR b) throw()
{
  std::shared_ptr<ParseAnd> result(new ParseAnd);
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
  std::shared_ptr<ParseOr> result(new ParseOr);
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
  return std::shared_ptr<ParseNot>(new ParseNot(x));
}

PR anon(std::string const& name, PR const x) throw()
{
  return PR(new AnonParser(name,x));
}

PR atLeastOne(PR b) throw()
{
  return anon(
    "at least one occurrance of "+b->target(),
    b+zeroOrMore()*b);
}

namespace
{
class NToM : public Parser
{
public:
  size_t const n_;
  size_t const m_;
  PR const x_;
  
  virtual ~NToM() throw() {}
  
  explicit NToM(size_t const n, size_t const m, PR const x) throw():
    n_(n),
    m_(m),
    x_(x) {
  }
  
  // Parser::
  virtual ParseResult parse_(I const at, Options const& options) throw() 
  {
    PV result(IRs(), at);
    for(size_t n=0; n<m_; ++n) {
      ParseResult const r(x_->parse(result.second, options));
      if (!r.failed()) {
        PV const x(*r);
        std::copy(x.first.begin(), x.first.end(),
                  std::back_inserter(result.first));
        result.second=x.second;
      }
      else if (n>=n_) {
        return ParseResult(result);
      }
      else{
        return ParseResult(
          Exception(
            std::shared_ptr<Exception::Cause const>(new TooFew(n)),
            r.e().at_, XJU_TRACED));
      }
    }
    return ParseResult(result);
  }

  // Parser::
  virtual std::string target() const throw();

  class TooFew : public Exception::Cause
  {
  public:
    explicit TooFew(size_t const got) throw():
        got_(got) {
    }
    ~TooFew() throw() {}
    
    std::string str() const throw()
    {
      std::ostringstream s;
      s << "only got " << got_ << " occurrances";
      return s.str();
    }
    size_t const got_;
  };
  
};

std::string NToM::target() const throw() 
{
  // bracket lhs/rhs if ambiguous (and/or/times)
  std::string const xt(x_->target());
  std::ostringstream s;
  s << n_ << ".." << m_ << " occurrances of " 
    << ((dynamic_cast<ParseOr const*>(&*x_)||
         dynamic_cast<ParseAnd const*>(&*x_))?
        std::string("(")+xt+std::string(")"):
        xt);
  return s.str();
}

}

PR nToM(size_t const n, size_t const m, PR const x) throw()
{
  std::ostringstream s;
  s << n << ".." << m << " occurrances of " << x->target();
  return PR(new NToM(n,m,x));
}

PR optional(PR x) throw()
{
  return PR(new Optional(x));
}

PR parseAnyChar() throw()
{
  static PR parseAnyChar(new ParseAnyChar);
  return parseAnyChar;
}

PR parseOneOfChars(std::string const& chars) throw()
{
  return PR(new ParseOneOfChars(chars));
}

PR parseOneOfChars(hcp::Chars const& chars) throw()
{
  return PR(new ParseOneOfChars2(chars));
}

PR parseAnyCharExcept(std::string const& chars) throw()
{
  return PR(new ParseAnyCharExcept(chars));
}

PR charInRange(char const min, char const max) throw()
{
  return PR(new ParseCharInRange(min, max));
}

PR parseLiteral(std::string const& x) throw()
{
  return PR(new ParseLiteral(x));
}

PR digit() throw()
{
  static PR digit(charInRange('0','9'));
  return digit;
}

  
PR octalDigit() throw()
{
  static PR octalDigit(charInRange('0', '7'));
  return octalDigit;
}

  
PR hexDigit() throw()
{
  static PR hexDigit(charInRange('0','9')|
                     charInRange('a','f')|
                     charInRange('A','F'));
  return hexDigit;
}

PR alpha() throw()
{
  static PR result{
    charInRange('a','z')|charInRange('A','Z')};
  return result;
}
PR cr() throw()
{
  static PR result(parseLiteral("\r"));
  return result;
}
PR lf() throw()
{
  static PR result(parseLiteral("\n"));
  return result;
}
PR crlf() throw()
{
  static PR result(parseLiteral("\r\n"));
  return result;
}
PR usAsciiPrintable() throw()
{
  static PR result(charInRange(32,127));
  return result;
}

PR parseUntil(PR match, PR const x) throw()
{
  return PR(new ParseSpecificUntil(match, x));
}

PR parseUntil(PR const x) throw()
{
  return PR(new ParseUntil(x));
}

PR balanced(PR until, bool angles) throw()
{
  return PR(new ParseBalanced(until, angles));
}

PR whitespaceChar() throw()
{
  static PR whitespaceChar(parseOneOfChars(" \t\n\r"));
  return whitespaceChar;
}

PR doubleSlash() throw(){
  static PR result(parseLiteral("//"));
  return result;
}

PR slashStar() throw(){
  static PR result(parseLiteral("/*"));
  return result;
}

PR starSlash() throw(){
  static PR result(parseLiteral("*/"));
  return result;
}

PR parseHash() throw()
{
  static PR parseHash(new ParseHash);
  return parseHash;
}


PR lineComment() throw()
{
  static PR lineComment(named<hcp_ast::LineComment>(
                          "line comment",
                          doubleSlash()+
                          parseUntil(parseOneOfChars("\n"))+
                          (zeroOrMore()*whitespaceChar())));
  return lineComment;
}

  
PR blockComment() throw()
{
  static PR blockComment(named<hcp_ast::BlockComment>(
                           "block comment",
                           slashStar()+
                           parseUntil(starSlash())+
                           starSlash()+
                           (zeroOrMore()*whitespaceChar())));
  return blockComment;
}

  
PR comments() throw()
{
  static PR comments(named<hcp_ast::Comments>(
                       "comments",
                       atLeastOne(lineComment()|blockComment())));
  return comments;
}

  
// matches nothing or something
PR eatWhite() throw()
{
  static PR eatWhite(
    anon("optional whitespace",
         !(whitespaceChar()|doubleSlash()|slashStar())|
         named<hcp_ast::WhiteSpace>(
              "whitespace",
              zeroOrMore()*(whitespaceChar()|comments()))));
  return eatWhite;
}

PR nonBackslashDoubleQuote() throw()
{
  static PR nonBackslashDoubleQuote(new ParseUntil(parseOneOfChars("\\\"")));
  return nonBackslashDoubleQuote;
}

PR nonDoubleQuote() throw()
{
  static PR nonDoubleQuote(new ParseUntil(parseOneOfChars("\"")));
  return nonDoubleQuote;
}

  
PR doubleQuote() throw()
{
  static PR doubleQuote(parseOneOfChars("\""));
  return doubleQuote;
}
PR doubleOpenSquare() throw()
{
  static PR result{parseLiteral("[[")};
  return result;
}

PR lessThan() throw()
{
  static PR result(parseOneOfChars("<"));
  return result;
}
PR greaterThan() throw()
{
  static PR result(parseOneOfChars(">"));
  return result;
}

PR doubleColon() throw()
{
  static PR result(parseLiteral("::")+!parseOneOfChars(":"));
  return result;
}

PR semicolon() throw()
{
  static PR result(parseLiteral(";"));
  return result;
}

PR backslash() throw()
{
  static PR backslash(parseOneOfChars("\\"));
  return backslash;
}

PR s_chars() throw()
{
  static PR s_chars(named<hcp_ast::S_Chars>(
                      "string literal characters",
                      parseUntil(s_char(), doubleQuote())));
  return s_chars;
}


PR stringLiteral() throw()
{
  static PR stringLiteral(named<hcp_ast::StringLiteral>(
                            "string literal",
                            atLeastOne(doubleQuote()+
                                       s_chars()+
                                       doubleQuote()+eatWhite())));
  return stringLiteral;
}

PR rawStringLiteral() throw()
{
  static PR result(named<hcp_ast::StringLiteral>(
                     "raw string literal",
                     PR(
                       new ParseRawStringLiteral)));
  return result;
}

//
// to be able to split a combined .h and .cpp (ie a .hcp) file into
// .h and .cpp parts, we need to choose whether each #include goes
// into the .h or the .cpp; we use a simple convention of 
// adding //impl to the #include line, eg
//   #include <x.h> //impl
// ... which indicates #include <x.h> should go in the .cpp not the .h
//
PR hashIncludeImplMarker() throw()
{
  static PR hashIncludeImplMarker(
    parseLiteral("//")+
    (zeroOrMore()*parseOneOfChars(" \t"))+
    parseLiteral("impl")+
    (zeroOrMore()*parseOneOfChars(" \t"))+
    parseLiteral("\n"));
  return hashIncludeImplMarker;
}


PR hashIncludeCommon() throw()
{
  static PR hashIncludeCommon(
    parseHash()+
    (zeroOrMore()*parseOneOfChars(" \t"))+
    parseLiteral("include")+(zeroOrMore()*parseOneOfChars(" \t"))+
    ((lessThan()+PR(named<hcp_ast::TargetOfHashInclude>(
                      "target of #include",parseUntil(greaterThan())))+
      greaterThan())|
     (doubleQuote()+PR(named<hcp_ast::TargetOfHashInclude>(
                         "target of #include",parseUntil(doubleQuote())))+
      doubleQuote()))+
    parseUntil(parseLiteral("\n")|hashIncludeImplMarker()));
  return hashIncludeCommon;
}


PR hashInclude() throw()
{
  static PR hashInclude(named<hcp_ast::HashInclude>(
                          "#include",
                          hashIncludeCommon()+
                          parseOneOfChars("\n")+
                          eatWhite()));
  return hashInclude;
}

  
PR hashIncludeImpl() throw()
{
  static PR hashIncludeImpl(named<hcp_ast::HashIncludeImpl>(
                              "#include with //impl marker",
                              hashIncludeCommon()+
                              hashIncludeImplMarker()+
                              eatWhite()));
  return hashIncludeImpl;
}

  
PR hash() throw()
{
  static PR hash(named<hcp_ast::OtherPreprocessor>(
                   "other preprocessor directive",
                   parseHash()+
                   parseUntil(parseAnyCharExcept("\\")+parseOneOfChars("\n"))+
                   parseAnyChar()+
                   eatWhite()));
  return hash;
}

PR attributes() throw()
{
  static PR result{
    named<hcp_ast::Attributes>(
      "attributes",
      parseLiteral("[[")+balanced(parseLiteral("]"),false)+parseLiteral("]]")+
      eatWhite())};
  return result;
}
  
PR whitespace() throw()
{
  static PR whitespace(named<hcp_ast::Whitespace>(
                         "some whitespace",
                         atLeastOne(whitespaceChar())));
  return whitespace;
}

PR identifierContChar() throw()
{
  static PR identifierContChar(charInRange('a', 'z')|
                               charInRange('A', 'Z')|
                               charInRange('0', '9')|
                               parseOneOfChars("_"));
  return identifierContChar;
}


PR identifier() throw()
{
  static PR result(new ParseIdentifier());
  return result;
}

PR class_struct_union_literal() throw()
{
  static PR class_struct_union_literal(
    (parseLiteral("class")|parseLiteral("struct")|parseLiteral("union"))+
    !identifierContChar());
  return class_struct_union_literal;
}


PR scope_ref() throw(){
  static PR result(
    optional(class_struct_union_literal())+eatWhite()+
    optional(doubleColon()+eatWhite())+
    zeroOrMore()*(
      identifier()+eatWhite()+
      (!parseOneOfChars("<")|(parseOneOfChars("<")+
                              balanced(parseOneOfChars(">"), true)+
                              parseOneOfChars(">")+eatWhite()))+
      doubleColon()+eatWhite()));
  return result;
}

PR typename_keyword() throw()
{
  PR result(
    parseLiteral("typename")+!identifierContChar()+eatWhite());
  return result;
}

PR scoped_name() throw(){ //see also scoped_function_name
  static PR result(
    named<hcp_ast::ScopedName>(
      "scoped name",
      optional(typename_keyword())+
      scope_ref()+
      identifier()+eatWhite()+
      (!parseOneOfChars("<")|(parseOneOfChars("<")+
                              balanced(parseOneOfChars(">"), true)+
                              parseOneOfChars(">")+eatWhite()))));
  return result;
}

PR bracketed(PR x) throw()
{
  return parseLiteral("(")+eatWhite()+x+parseLiteral(")")+eatWhite();
}

PR typedef_keyword() throw()
{
  static PR typedef_keyword(parseLiteral("typedef")+!identifierContChar());
  return typedef_keyword;
}

PR const_keyword() throw()
{
  static PR result{
    PR(named<hcp_ast::ConstQual>(
         "const qualifier",
         parseLiteral("const")))+!identifierContChar()};
  return result;
}

PR volatile_keyword() throw()
{
  static PR result{
    PR(named<hcp_ast::VolatileQual>(
         "volatile qualifier",
         parseLiteral("volatile")))+!identifierContChar()};
  return result;
}

PR cv() throw()
{
  static PR result {
    anon("const/volatile qualifiers",
         zeroOrMore()*((const_keyword()|volatile_keyword())+
                       eatWhite()))};
  return result;
}

PR type_qual() throw()
{
  static PR result{
    anon("const/volatile/*/& type qualifier",
         (const_keyword()|volatile_keyword()|
          PR(named<hcp_ast::MoveQual>(
               "move qualifier",
               parseLiteral("&&")))|
          PR(named<hcp_ast::RefQual>(
               "ref qualifier",
               parseOneOfChars("&")))|
          PR(named<hcp_ast::PointerQual>(
               "pointer qualifier",
               parseOneOfChars("*"))))+eatWhite())
      };
  return result;
}

PR type_ref() throw()
{
  static PR result{
    named<hcp_ast::TypeRef>(
      "type reference",
      cv() +
      type_name() +
      zeroOrMore()*type_qual()+ eatWhite()+
      (!parseLiteral(".")|(
        PR(named<hcp_ast::ElipsesQual>(
             "elipses qualifier",
             parseLiteral("...")))+eatWhite())))};
  return result;
}

PR defined_type() throw(){
  static PR result(
    named<hcp_ast::DefinedType>(
      "\"defined type\"",
      identifier())+eatWhite());
  return result;
}

PR array_decl() throw()
{
  static PR array_decl(
    named<hcp_ast::ArrayDecl>(
      "array decl",
      parseOneOfChars("[")+
      balanced(parseOneOfChars("]"))+
      parseOneOfChars("]"))+
    eatWhite());
  return array_decl;
}


PR typedef_non_fp() throw()
{
  static PR typedef_statement(
    typedef_keyword()+
    whitespace()+
    type_ref()+defined_type()+(!parseLiteral("[")|
                               array_decl())+parseOneOfChars(";")+
    eatWhite());
  return typedef_statement;
}

PR using_keyword() throw()
{
  static PR using_keyword(parseLiteral("using")+!identifierContChar());
  return using_keyword;
}


PR using_statement() throw()
{
  static PR using_statement(
    named<hcp_ast::Using>(
      "using statement",
      using_keyword()+
      whitespace()+
      balanced(parseOneOfChars(";"))+
      parseOneOfChars(";")+
      eatWhite()));
  return using_statement;
}

  
PR enum_name() throw()
{
  static PR enum_name(
    named<hcp_ast::EnumName>(
      "\"enum name\"",
      identifier()));
  return enum_name;
}


PR enum_keyword() throw()
{
  static PR enum_keyword(parseLiteral("enum")+!identifierContChar());
  return enum_keyword;
}


PR scoped_enum_def() throw()
{
  static PR scoped_enum_def(
    named<hcp_ast::EnumDef>(
      "scoped enum definition",
      enum_keyword()+whitespace()+(parseLiteral("struct")|
                                   parseLiteral("class"))+
      whitespace()+
      optional(enum_name())+eatWhite()+
      ((":"+eatWhite()+type_name+eatWhite)|
       !parseLiteral(":"))+
      parseLiteral("{")+
      balanced(parseOneOfChars("}"))+
      parseOneOfChars("}")+eatWhite()+
      parseOneOfChars(";")+
      eatWhite()));
  return scoped_enum_def;
}

PR enum_def() throw()
{
  static PR enum_def(
    named<hcp_ast::EnumDef>(
      "enum definition",
      enum_keyword()+
      whitespace()+
      optional(enum_name())+eatWhite()+parseLiteral("{")+
      balanced(parseOneOfChars("}"))+
      parseOneOfChars("}")+eatWhite()+
      parseOneOfChars(";")+
      eatWhite()));
  return enum_def;
}

PR enum_decl() throw()
{
  static PR result(named<hcp_ast::EnumForwardDecl>(
                     "enum forward-declaration",
                     enum_keyword()+whitespace()+
                     optional((parseLiteral("struct")|
                               parseLiteral("class"))+
                              whitespace())+
                     enum_name()+eatWhite()+
                     parseOneOfChars(";")+
                     eatWhite()));
  return result;
}
  
PR bracketed() throw()
{
  static PR result(parseLiteral("(")+balanced(parseLiteral(")"))+parseLiteral(")")+eatWhite());
  return result;
}


PR unqualifiedTypeName() throw()
{
  static PR unqualifiedTypeName(
    identifier()+
    (!parseOneOfChars("<")|(parseOneOfChars("<")+
                            balanced(parseOneOfChars(">"), true)+
                            parseOneOfChars(">"))));
  return unqualifiedTypeName;
}

PR operator_keyword() throw()
{
  static PR operator_keyword(
    parseLiteral("operator")+!identifierContChar()+eatWhite());
  return operator_keyword;
}

PR operator_name() throw()
{
  static PR operator_name(
    anon(
      "operator name",
      scope_ref()+
      operator_keyword()+
      named<hcp_ast::OperatorName>(
        "operator name",
        parseLiteral("()")|
        parseLiteral("&&")|
        parseLiteral("||")|
        parseLiteral("<<=")|
        parseLiteral("<<")|
        parseLiteral(">>")|
        parseLiteral(">>=")|
        parseLiteral("==")|
        parseLiteral("!=")|
        parseLiteral("<=")|
        parseLiteral(">=")|
        parseLiteral("<")|
        parseLiteral(">")|
        parseLiteral("++")|
        parseLiteral("+=")|
        parseLiteral("--")|
        parseLiteral("-=")|
        parseLiteral("->*")|
        parseLiteral("->")|
        parseLiteral("+")|
        parseLiteral("-")|
        parseLiteral("|=")|
        parseLiteral("&=")|
        parseLiteral("|")|
        parseLiteral("&")|
        parseLiteral(",")|
        parseLiteral("[]")|
        parseLiteral("!")|
        parseLiteral("^")|
        parseLiteral("^=")|
        parseLiteral("%=")|
        parseLiteral("%")|
        parseLiteral("=")|
        parseLiteral("*")|
        parseLiteral("*=")|
        parseLiteral("~")|
        parseLiteral("~=")|
        parseLiteral("/")|
        parseLiteral("/=")|
        parseLiteral("new")|
        parseLiteral("delete"))+
      eatWhite()));
  return operator_name;
}

PR destructor_name() throw()
{
  static PR destructor_name(
    anon(
      "destructor name",
      scope_ref()+parseLiteral("~")+eatWhite()+identifier()));
  return destructor_name;
}

PR class_name() throw()
{
  static PR result(
    named<hcp_ast::ClassName>(
      "class name",
      optional(typename_keyword())+
      optional(doubleColon())+eatWhite()+
      unqualifiedTypeName()+
      zeroOrMore()*(
        eatWhite()+doubleColon()+eatWhite()+unqualifiedTypeName()))+
    eatWhite());
  return result;
}

PR built_in_type_name() throw()
{
  static PR result(
    atLeastOne(
      (parseLiteral("char")|
       parseLiteral("char16_t")|
       parseLiteral("char32_t")|
       parseLiteral("wchar_t")|
       parseLiteral("short")|
       parseLiteral("int")|
       parseLiteral("long")|
       parseLiteral("float")|
       parseLiteral("double")|
       parseLiteral("signed")|
       parseLiteral("unsigned"))+!identifierContChar()+eatWhite()));
  return result;
}

PR decltype_() throw()
{
  static PR result(
    parseLiteral("decltype")+
    eatWhite()+
    bracketed());
  return result;
}

PR type_name() throw()
{
  static PR result(
    anon(
      "type name",
      built_in_type_name()|
      decltype_()|
      scoped_name()));
  return result;
}

PR conversion_operator_name() throw()
{
  static PR conversion_operator_name(
    scope_ref()+
    operator_keyword()+
    type_ref());
  return conversion_operator_name;
}

  
PR keyword_static() throw()
{
  static PR keyword_static(
    named<hcp_ast::KeywordStatic>(
      "\"static\"",
      parseLiteral("static"))+!identifierContChar()+eatWhite());
  return keyword_static;
}

PR keyword_extern() throw()
{
  static PR result(
    named<hcp_ast::KeywordExtern>(
      "\"extern\"",
      parseLiteral("extern"))+!identifierContChar()+eatWhite());
  return result;
}


PR keyword_inline() throw()
{
  static PR result(
    parseLiteral("inline")+!identifierContChar()+eatWhite());
  return result;
}

PR keyword_mutable() throw()
{
  static PR result(
    parseLiteral("mutable")+!identifierContChar()+eatWhite());
  return result;
}

PR keyword_friend() throw()
{
  static PR keyword_friend(
    named<hcp_ast::KeywordFriend>(
      "\"friend\"",
      parseLiteral("friend"))+!identifierContChar()+eatWhite());
  return keyword_friend;
}

PR keyword_public() throw()
{
  static PR keyword_public(
    named<hcp_ast::KeywordPublic>(
      "\"public\"",
      parseLiteral("public"))+!identifierContChar()+eatWhite());
  return keyword_public;
}

PR keyword_private() throw()
{
  static PR keyword_private(
    named<hcp_ast::KeywordPrivate>(
      "\"private\"",
      parseLiteral("private"))+!identifierContChar()+eatWhite());
  return keyword_private;
}

PR keyword_protected() throw()
{
  static PR keyword_protected(
    named<hcp_ast::KeywordProtected>(
      "\"protected\"",
      parseLiteral("protected"))+!identifierContChar()+eatWhite());
  return keyword_protected;
}

PR keyword_virtual() throw()
{
  static PR keyword_virtual(
    named<hcp_ast::KeywordVirtual>(
      "\"virtual\"",
      parseLiteral("virtual"))+!identifierContChar()+eatWhite());
  return keyword_virtual;
}

PR keyword_explicit() throw()
{
  static PR keyword_explicit(
    named<hcp_ast::KeywordExplicit>(
      "\"explicit\"",
      parseLiteral("explicit"))+!identifierContChar()+eatWhite());
  return keyword_explicit;
}

PR keyword_override() throw()
{
  static PR result(
    named<hcp_ast::KeywordOverride>(
      "\"override\"",
      parseLiteral("override"))+!identifierContChar()+eatWhite());
  return result;
}

PR keyword_final() throw()
{
  static PR result(
    named<hcp_ast::KeywordFinal>(
      "\"final\"",
      parseLiteral("final"))+!identifierContChar()+eatWhite());
  return result;
}

PR keyword_noexcept() throw()
{
  static PR result(
    named<hcp_ast::KeywordNoexcept>(
      "\"noexcept\"",
      parseLiteral("noexcept"))+!identifierContChar()+eatWhite());
  return result;
}

PR keyword_throw() throw()
{
  static PR result(
    named<hcp_ast::KeywordThrow>(
      "\"throw\"",
      parseLiteral("throw"))+!identifierContChar()+eatWhite());
  return result;
}

std::shared_ptr<NamedParser<hcp_ast::EmptyThrow> > empty_throw() noexcept
{
  static std::shared_ptr<NamedParser<hcp_ast::EmptyThrow> > const result(
    named<hcp_ast::EmptyThrow>(
      "empty throw clause",
      keyword_throw()+eatWhite+"("+eatWhite()+")"+eatWhite()));
  return result;
}
std::shared_ptr<NamedParser<hcp_ast::ThrowList> > throw_list() noexcept
{
  static auto const result(
    named<hcp_ast::ThrowList>(
      "non-empty throw clause",
      keyword_throw()+eatWhite+
      listOf(parseLiteral("("),
             named<hcp_ast::ThrowListItem>(
               "throw clause item",
               eatWhite()+
               named<hcp_ast::ThrowListItemTypeName>(
                 "type name",
                 type_name())),
             eatWhite()+parseLiteral(","),
             eatWhite()+")")+
      named<hcp_ast::ThrowListTrailingWhite>(
        "throw clause trailing whitespace",eatWhite())));
  return result;
}
PR throw_clause() throw()
{
  static PR result(
    empty_throw()|throw_list()|
    (keyword_noexcept()+(!parseLiteral("(")|bracketed())));
  return result;
}

PR function_qualifiers() throw()
{
  static PR function_qualifiers(
    named<hcp_ast::FunctionQualifiers>(
      "function qualifiers",
      zeroOrMore()*((keyword_virtual()|
                     keyword_explicit()|
                     keyword_friend()|
                     keyword_static()|
                     keyword_inline())+eatWhite())));
  return function_qualifiers;
}

PR virt_specifier_seq() throw()
{
  static PR result(
    named<hcp_ast::VirtSpecifierSeq>(
      "virt-specifier-seq",
      (keyword_override()+eatWhite()+keyword_final())|
      (keyword_final()+eatWhite()+keyword_override())|
      keyword_override()|
      keyword_final()));
  return result;
}
PR function_post_qualifiers() throw()
{
  static PR result(
    named<hcp_ast::FunctionPostQualifiers>(
      "function post-qualifiers",
      cv()+
      (!(keyword_throw()|keyword_noexcept())|throw_clause())+
      optional(virt_specifier_seq())));
  return result;
}


PR block_open() throw()
{
  static PR result(named<hcp_ast::BlockOpen>(
                    "block open",
                    parseLiteral("{")));
  return result;
}

PR block() throw()
{
  static PR block(named<hcp_ast::Block>(
                    "block",
                    block_open()+
                    balanced(parseOneOfChars("}"))+
                    parseLiteral("}")));
  return block;
}


PR init_list() throw()
{
  static PR init_list(named<hcp_ast::InitList>(
                        "initialiser list",
                        named<hcp_ast::InitListOpen>(
                          "init list open",
                          parseLiteral(":"))+
                        balanced(parseOneOfChars("{;"))));
  return init_list;
}


PR keyword_try() throw()
{
  static PR result(
    named<hcp_ast::KeywordTry>(
      "\"try\"",
      parseLiteral("try"))+!identifierContChar()+eatWhite());
  return result;
}

PR keyword_catch() throw()
{
  static PR result(
    named<hcp_ast::KeywordCatch>(
      "\"catch\"",
      parseLiteral("catch"))+!identifierContChar()+eatWhite());
  return result;
}

PR catch_block() throw()
{
  static PR catch_block(keyword_catch()+bracketed()+block());
  return catch_block;
}

// does not eat trailing white
PR function_impl() throw()
{
  static PR function_impl(
    named<hcp_ast::FunctionImpl>(
      "function implementation",
      //block does not eat trailing white...
      (keyword_try()+init_list()+block()+eatWhite()+catch_block())|
      (keyword_try()+block()+eatWhite()+catch_block())|
      (init_list()+block())|
      block()));
  return function_impl;
}

                
PR params() throw();

PR template_keyword() throw()
{
  static PR template_keyword(
    parseLiteral("template")+!identifierContChar()+eatWhite());
  return template_keyword;
}


PR template_empty_preamble() throw()
{
  static PR template_empty_preamble(
    named<hcp_ast::TemplateEmptyPreamble>(
      "template empty preamble",
      template_keyword()+
      parseOneOfChars("<")+
      eatWhite()+
      parseOneOfChars(">")+
      eatWhite()));
  return template_empty_preamble;
}


PR template_preamble() throw()
{
  static PR template_preamble(
    named<hcp_ast::TemplatePreamble>(
      "template preamble",
      !template_empty_preamble()+(
        template_keyword()+
        parseOneOfChars("<")+
        balanced(parseOneOfChars(">"), true)+
        parseOneOfChars(">")+
        eatWhite())));
  return template_preamble;
}

PR conversion_operator_function_proto() throw()
{
  static PR result(
    function_qualifiers()+
    named<hcp_ast::FunctionName>(
      "conversion operator name",
      conversion_operator_name())+
    eatWhite()+
    (!doubleOpenSquare()|attributes())+
    params());
  return result;
}

PR typed_function_proto() throw()
{
  static PR result(
    function_qualifiers()+
    named<hcp_ast::ReturnType>(
      "return type",
      type_ref())+
    named<hcp_ast::FunctionName>(
      "function name",
      operator_name()|
      scoped_name())+
    eatWhite()+
    (!doubleOpenSquare()|attributes())+
    params());
  return result;
}

PR untyped_function_proto() throw()
{
  static PR result(
    function_qualifiers()+
    named<hcp_ast::FunctionName>(
      "function name",
      destructor_name()|
      operator_name()|
      scoped_name())+
    eatWhite()+
    (!doubleOpenSquare()|attributes())+
    params());
  return result;
}

PR function_initialiser() throw()
{
  static PR result(
    anon("function initialier",
         parseLiteral("=")+balanced(parseLiteral(";")+eatWhite())));
  return result;
}

PR post_result_type() noexcept
{
  static PR result(
    anon("post result type",
         parseLiteral("->")+eatWhite()+type_ref()));
  return result;
}

PR function_proto() throw()
{
  static PR result(
    anon(
      "function proto",
      (conversion_operator_function_proto()|
       typed_function_proto()|
       untyped_function_proto())+
      function_post_qualifiers()+
      (!parseLiteral("->")|post_result_type())+
      (!parseLiteral("=")|function_initialiser())));
  return result;
}

PR function_decl() throw()
{
  static PR function_decl(named<hcp_ast::FunctionDecl>(
                            "function declaration",
                            optional(template_empty_preamble()|
                                     template_preamble())+
                            (!doubleOpenSquare()|attributes())+
                            function_proto()+
                            (eatWhite()+parseOneOfChars(";"))+
                            eatWhite()));
  return function_decl;
}
  
PR function_def_unnamed() throw()
{
  static PR result(
    function_proto()+
    function_impl()+
    eatWhite());
  return result;
}

PR function_def() throw()
{
  static PR function_def(
    named<hcp_ast::FunctionDef>(
      "non-template function definition",
      optional(template_empty_preamble())+
      (!doubleOpenSquare()|attributes())+
      function_def_unnamed()));
  return function_def;
}

  
PR template_function_def() throw()
{
  static PR template_function_def(
    named<hcp_ast::TemplateFunctionDef>(
      "template function definition",
      atLeastOne(template_preamble())+
      (!doubleOpenSquare()|attributes())+
      function_def_unnamed()));
  return template_function_def;
}

  
PR not_class_struct_union_literal() throw()
{
  static PR not_class_struct_union_literal(
    !class_struct_union_literal());
  return not_class_struct_union_literal;
}

PR base_specifier() throw()
{
  static PR result{
    named<hcp_ast::BaseSpecifier>(
      "base specifier",
      optional((keyword_public()|
                keyword_private()|
                keyword_protected())+eatWhite())+
      optional(keyword_virtual())+eatWhite()+
      !(keyword_public()|
        keyword_private()|
        keyword_protected()|
        keyword_virtual())+
      scoped_name())};
  return result;
}
PR base_specifier_list() throw()
{
  static PR result{
    nonEmptyListOf(base_specifier(),
                   parseLiteral(",")+eatWhite())};
  return result;
}
PR class_proto() throw()
{
  static PR class_proto(
    zeroOrMore()*(template_preamble()|template_empty_preamble())+
    optional(keyword_friend())+
    class_struct_union_literal()+
    whitespace()+
    class_name()+
    (!parseLiteral(":")|
     (parseLiteral(":")+eatWhite()+base_specifier_list())));
  return class_proto;
}


PR class_decl() throw()
{
  static PR class_decl(named<hcp_ast::ClassForwardDecl>(
                         "class forward-declaration",
                         class_proto()+
                         parseOneOfChars(";")+
                         eatWhite()));
  return class_decl;
}

  
PR var_name() throw()
{
  static PR var_name(
    named<hcp_ast::VarName>(
      "var name",
      identifier())+eatWhite());
  return var_name;
}


PR var_intro() throw()
{
  static PR var_intro(
    balanced(whitespaceChar()+var_name()+
             optional(array_decl())+eatWhite()+
             parseOneOfChars("=;"))+
    whitespaceChar()+var_name()+optional(array_decl())+eatWhite());
  return var_intro;
}


PR static_var_intro() throw()
{
  static PR static_var_intro(
    keyword_static()+
    eatWhite()+
    var_intro());
  return static_var_intro;
}

PR var_initialiser_open() throw()
{
  static PR result(
    named<hcp_ast::VarInitialiserOpen>(
      "variable initialiser '='",
      parseOneOfChars("=")));
  return result;
}
    
PR var_initialiser_1() throw()
{
  static PR var_initialiser(
    named<hcp_ast::VarInitialiser>(
      "variable initialiser",
      (var_initialiser_open()+balanced(parseOneOfChars(");,")))));
  return var_initialiser;
}

PR var_initialiser_open_2() throw()
{
  PR result{
    named<hcp_ast::VarInitialiserOpen>(
      "variable initialiser '{'",
      parseOneOfChars("{"))};
  return result;
}
    
    
PR var_initialiser_2() throw()
{
  static PR result{
    named<hcp_ast::VarInitialiser>(
      "variable initialiser",
      (var_initialiser_open_2()+balanced(parseOneOfChars("}"))+
       parseOneOfChars("}")))};
  return result;
}

PR var_initialiser() throw()
{
  static PR result{
    anon("var initialiser",
         var_initialiser_1()|
         var_initialiser_2())};
  return result;
}

PR var_non_fp() throw()
{
  static PR result{
    named<hcp_ast::VarNonFp>(
      "non-function pointer var",
      (type_ref()+var_name()+
       (!doubleOpenSquare()|attributes())+
       optional(array_decl())+
       (!parseOneOfChars("={")|var_initialiser())))};
  return result;
}

PR var_fp() throw();

struct VarFpBackref : public Parser
{
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw() 
  {
    return var_fp()->parse_(at,o);
  }
  virtual std::string target() const throw() {
    return "function pointer var (backref)";
  }
};
PR var_fp_backref() throw()
{
  static PR result{new VarFpBackref};
  return result;
}
    
PR param() throw()
{
  static PR result(
    named<hcp_ast::Param>(
      "param",
      (var_non_fp()|
       var_fp_backref()|
       type_ref())+
      eatWhite()));
  return result;
}
  
PR params() throw()
{
  static PR result(
    anon(
      "params",
      parseLiteral("(")+eatWhite()+
      listOf(param(),parseLiteral(",")+eatWhite(),parseLiteral(")"))+
      eatWhite()));
  return result;
}
  
PR var_fp() throw()
{
  static PR result{
    anon("function pointer var",
         type_ref()+
         bracketed(
           scope_ref()+cv()+parseLiteral("*")+eatWhite()+cv()+
           var_name()+
           (!doubleOpenSquare()|attributes()))+
         params()+
         function_post_qualifiers()+
         (!parseOneOfChars("={")|var_initialiser())+
         eatWhite())};
  return result;
}

PR typedef_fp() throw()
{
  PR result(
    anon("typedef of function pointer",
         typedef_keyword()+
         whitespace()+
         type_ref()+
         bracketed(
           scope_ref()+cv()+parseLiteral("*")+eatWhite()+cv()+
           defined_type()+(!parseLiteral("[")|
                           array_decl()))+
         params()+
         function_post_qualifiers()+
         eatWhite()+
         parseOneOfChars(";")+eatWhite()));
  return result;
}

PR typedef_statement() throw()
{
  static PR result(
    named<hcp_ast::Typedef>(
      "typedef statement",
      typedef_fp()|
      typedef_non_fp()));
  return result;
}


PR var_def() throw()
{
  static PR result{
    optional(keyword_mutable())+
    (var_non_fp()|var_fp())+
    eatWhite()};
  return result;
}
    
PR global_var_def() throw()
{
  static PR global_var_def{
    named<hcp_ast::GlobalVarDef>(
      "global variable definition",
      (!doubleOpenSquare()|attributes())+
      var_def()+
      parseLiteral(";")+
      eatWhite())};
  return global_var_def;
}

PR static_var_def() throw()
{
  static PR static_var_def(named<hcp_ast::StaticVarDef>(
                             "static variable definition",
                             keyword_static()+
                             eatWhite()+
                             (!doubleOpenSquare()|attributes())+
                             var_def()+
                             parseLiteral(";")+
                             eatWhite()));
  return static_var_def;
}
  
PR extern_var_def() throw()
{
  static PR result(named<hcp_ast::ExternVarDef>(
                     "extern variable definition",
                     keyword_extern()+
                     eatWhite()+
                     (!doubleOpenSquare()|attributes())+
                     var_def()+
                     parseLiteral(";")+
                     eatWhite()));
  return result;
}
  
PR access_modifier() throw()
{
  static PR access_modifier(named<hcp_ast::AccessModifier>(
                              "public/private/protected: marker",
                              (parseLiteral("public")|
                               parseLiteral("private")|
                               parseLiteral("protected"))+
                              eatWhite()+
                              parseOneOfChars(":")+
                              eatWhite()));
  return access_modifier;
}


PR not_typedef_using_enum_keyword() throw()
{
  static PR not_typedef_using_enum_keyword(
    !(typedef_keyword()|using_keyword()|enum_keyword()));
  return not_typedef_using_enum_keyword;
}


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
    x_(class_proto()+
       parseOneOfChars("{")+
       eatWhite()+
       named<hcp_ast::ClassMembers>(
         "class members",
         parseUntil(comments()|
                    access_modifier()|
                    PR(new SelfParser(*this))|
                    enum_decl()|
                    class_decl()|
                    (not_class_struct_union_literal()+(
                      typedef_statement()|
                      scoped_enum_def()|
                      enum_def()|
                      using_statement()|
                      (not_typedef_using_enum_keyword()+(
                        function_decl()|
                        template_function_def()|
                        function_def()|
                        static_var_def()|
                        extern_var_def()|
                        global_var_def())))),
                    parseOneOfChars("}")))+
       parseOneOfChars("}")+
       eatWhite()+
       parseOneOfChars(";")+
       eatWhite()),
    tp_(named<hcp_ast::TemplateClassDef>(
          "template class definition",
          template_preamble()+
          x_)),
    p_(named<hcp_ast::ClassDef>(
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

PR class_def() throw()
{
  static PR class_def(new ParseClass);
  return class_def;
}

PR not_function_proto_or_template_function_proto() throw()
{
  static PR result(
    anon("not function proto or template function proto",
         !(function_proto()|(template_preamble()+function_proto()))));
  return result;
}

PR namespace_leaf() throw()
{
  static PR namespace_leaf(
    comments()|
    hashIncludeImpl()|
    hashInclude()|
    hash()|
    class_def()| // note recursive
    enum_decl()|
    class_decl()|
    (not_class_struct_union_literal()+(
      typedef_statement()|
      using_statement()|
      scoped_enum_def()|
      enum_def()|
      (not_typedef_using_enum_keyword()+(
        whitespace()| //REVISIT: remove?
        function_decl()| // inc. template
        template_function_def()|
        function_def()|
        (not_function_proto_or_template_function_proto()+
         (static_var_def()|
          extern_var_def()|
          global_var_def())))))));
  return namespace_leaf;
}


PR namespace_keyword() throw()
{
  static PR namespace_keyword(parseLiteral("namespace")+!identifierContChar());
  return namespace_keyword;
}


PR not_namespace_keyword() throw()
{
  static PR not_namespace_keyword(!namespace_keyword());
  return not_namespace_keyword;
}


PR anonymous_namespace() throw()
{
  static PR anonymous_namespace(
    named<hcp_ast::AnonymousNamespace>(
      "anonymous namespace",
      namespace_keyword()+
      eatWhite()+
      named<hcp_ast::AnonymousNamespaceOpen>(
        "anon namespace open",
        parseOneOfChars("{"))+
      eatWhite()+
      parseUntil(namespace_leaf(), parseOneOfChars("}"))+
      parseOneOfChars("}")+
      eatWhite()));
  return anonymous_namespace;
}

  
namespace
{
class ParseNamespace : public NamedParser<hcp_ast::NamespaceDef>
{
public:
  PR x_;
  
  ParseNamespace() throw():
    NamedParser<hcp_ast::NamespaceDef>(
      "namespace",(
        namespace_keyword()+
        whitespace()+
        named<hcp_ast::NamespaceName>(
          "namespace name",
          identifier())+
        eatWhite()+
        parseOneOfChars("{")+
        eatWhite()+
        named<hcp_ast::NamespaceMembers>(
          "namespace members",
          parseUntil((PR(new SelfParser(*this))|
                      anonymous_namespace()|
                      (not_namespace_keyword()+namespace_leaf()))+
                     eatWhite(),
                     parseOneOfChars("}")))+
        parseOneOfChars("}")+
        eatWhite())) {
  }
};

}

PR namespace_def() throw()
{
  static PR namespace_def(new ParseNamespace);
  return namespace_def;
}

  
namespace
{
class ParseEndOfFile : public Parser
{
public:
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw() 
  {
    if (!at.atEnd()) {
      return ParseResult(
        Exception(
          std::shared_ptr<Exception::Cause>(
            new NotEndOfInput(*at)), at, XJU_TRACED));
    }
    return ParseResult(
      std::make_pair(
        IRs(1U, IR(new hcp_ast::EndOfFile(
                     IRs(1U, IR(new hcp_ast::Item(at, at)))))),at));
  }


  // Parser::
  virtual std::string target() const throw() {
    return "end of file";
  }

  class NotEndOfInput : public Exception::Cause
  {
  public:
    NotEndOfInput(char const got) throw():
        got_(got)
    {
    }
    ~NotEndOfInput() throw() {}
    
    std::string str() const throw()
    {
      std::ostringstream s;
      s << "expected end of input, not '"
        << xju::format::cEscapeChar(got_) << "'";
      return s.str();
    }
    char const got_;
  };

};
}

PR endOfFile() throw()
{
  static PR endOfFile(new ParseEndOfFile);
  return endOfFile;
}

  
std::shared_ptr<Parser> file() throw()
{
  static PR file(named<hcp_ast::File>(
                   "file",
                   optional(comments())+
                   eatWhite()+
                   parseUntil(namespace_def()|
                              anonymous_namespace()|
                              (not_namespace_keyword()+namespace_leaf()),
                              endOfFile())+
                   endOfFile()));
  return file;
}

std::pair<IRs,I> parse(I const startOfElement,
                      std::shared_ptr<Parser> elementType,
                      bool traceToStdout,
                      bool irsAtEnd)
  /*throw(
    // post: parent unmodified
    xju::Exception)*/
{
  try {
    Options options(traceToStdout,
                    Cache(new hcp_parser::CacheVal()),
                    irsAtEnd);
    ParseResult const r(elementType->parse(startOfElement, options));
    if (r.failed()) {
      throw r.e();
    }
    PV const x(*r);
    return std::make_pair(x.first,x.second);
  }
  catch(Exception const& e) {
    throw hcp::translateException(e);
  }
}

hcp_ast::Item parseString(
  std::string::const_iterator begin,
  std::string::const_iterator end,
  std::shared_ptr<Parser> parser,
  bool traceToStdout) /*throw(
    xju::Exception)*/
{
  I const startOfElement(begin,end);
  auto const r{
    parse(startOfElement,parser,traceToStdout)};
  if (!r.second.atEnd()){
    std::ostringstream s;
    s << "only parsed until " << r.second;
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  if (!r.first.size()){
    return hcp_ast::Item(startOfElement,I(end,end));
  }
  return hcp_ast::Item(r.first);
}

}
