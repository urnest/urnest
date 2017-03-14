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
// C++ syntax parsing, for example usage, see:
//  - hcp-parse-file.cc
//  - hcp-split.cc
//  - test-parser.cc
//
#ifndef HCP_PARSER_H_
#define HCP_PARSER_H_

#include <hcp/ast.hh>
#include "xju/Shared.hh"
#include "xju/Exception.hh"
#include "xju/next.hh"
#include <sstream>
#include <string>
#include <utility>
#include <set>
#include <algorithm>
#include <hcp/trace.hh>
#include "xju/Optional.hh"
#include <map>
#include <vector>

namespace hcp_parser
{

typedef hcp_ast::I I;
typedef hcp_ast::IR IR;
typedef std::vector<IR> IRs;
typedef std::pair<IRs, I> PV;

class Parser;
extern xju::Shared<Parser> file; // reference to whole-file parser

// The simplest parsing interface, which parses the specified
// type of C++ element (default is "whole file") assumed to
// appear at the specified startOfElement, appending ast Items
// to the specified parent.items_, and returning the position
// just after the parsed element.
// 
I parse(hcp_ast::CompositeItem& parent,
        I const startOfElement,
        xju::Shared<Parser> = file,
        bool traceToStdout = false,
        bool irsAtEnd = false)
  throw(
    // post: parent unmodified
    xju::Exception);

class Exception
{
public:
  class Cause
  {
  public:
    virtual ~Cause() throw()
    {
    }
    virtual std::string str() const throw() = 0;
  };
    
  Exception(xju::Shared<Cause const> cause, 
            I at, 
            xju::Traced const& trace,
            bool atEnd=false) throw():
    cause_(cause),
    at_(at),
    trace_(trace),
    atEnd_(atEnd) {
  }
  // gcc 4.7.2 refuses to generate the copy constructor
  Exception(Exception const& b) throw():
      cause_(b.cause_),
      at_(b.at_),
      trace_(b.trace_),
      context_(b.context_),
      atEnd_(b.atEnd_),
      irsAtEnd_(b.irsAtEnd_) {
  }

  //pre: lifetime(parser) includes lifetime(this)
  void addContext(Parser const& parser, I at, xju::Traced const& trace) throw()
  {
    context_.push_back(std::make_pair(std::make_pair(&parser, at), trace));
  }
  std::vector<std::pair<std::pair<Parser const*, I>, xju::Traced> > context_;

  xju::Shared<Cause const> const cause_;
  I const at_;
  xju::Traced const trace_;

  void addAtEndIRs(IRs const& irs) throw()
  {
    if (atEnd_) {
      std::copy(irs.rbegin(),irs.rend(),std::back_inserter(irsAtEnd_));
    }
  }
  IRs const& getIrsAtEnd() const throw()
  {
    return irsAtEnd_;
  }
  
private:
  //indicates eof caused parse failure
  bool const atEnd_;
  //if atEnd_, collects 
  std::vector<IR> irsAtEnd_;
};
std::string readableRepr(Exception const& e) throw();
  
  

class ParseResult
{
public:
  //post: failed()
  explicit ParseResult(Exception const& e) throw():
      e_(e)
  {
  }
  //post: !failed()
  explicit ParseResult(PV v) throw():
      v_(v)
  {
  }
  bool failed() const throw()
  {
    return !v_.valid();
  }
  
  //pre: !failed()
  PV const& operator*() const throw()
  {
    return v_.value();
  }
  
  //pre: failed()
  Exception const& e() const throw()
  {
    return e_.value();
  }

  //pre: failed()
  void addContext(Parser const& p, I at, const xju::Traced& trace) throw()
  {
    e_.value().addContext(p, at, trace);
  }
  void addAtEndIRs(IRs const& irs) throw()
  {
    e_.value().addAtEndIRs(irs);
  }
private:
  xju::Optional<Exception> e_;
  xju::Optional<PV> v_;
};

class Parser;
typedef std::pair<I, Parser*const> CacheKey;
typedef std::map<CacheKey, ParseResult> CacheVal;
    
typedef xju::Shared<CacheVal> Cache;
    
class Options
{
public:
  explicit Options(bool trace,
                   Cache cache,
                   bool irsAtEnd) throw():
    trace_(trace),
    cache_(cache),
    irsAtEnd_(irsAtEnd) {
  }
  Options(Options const& y) throw():
      trace_(y.trace_),
      cache_(y.cache_),
      irsAtEnd_(y.irsAtEnd_) {
  }
    
  bool trace_;
  mutable xju::Shared<std::map<std::pair<I, Parser*const>, ParseResult> > cache_;
  bool irsAtEnd_;
};

class Parser
{
public:
  virtual ~Parser() throw() {
  }
  
  // post: 
  virtual ParseResult parse_(
    I const at, 
    Options const& options) throw() = 0;

  // What the parser matches, e.g. "string literal", "typedef", "class"
  virtual std::string target() const throw() = 0;

  // post: result contains references to at or beyond, even if parsing
  //       fails (ie if result.failed())
  ParseResult parse(I const at, Options const& options) throw() 
  {
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
    return (*i).second;
  }

  template<class T>
  bool isA() const throw() {
    return dynamic_cast<T const*>(this);
  }
  // pre: isA<T>()
  template<class T>
  T& asA() throw() {
    return *dynamic_cast<T*>(this);
  }
  // pre: isA<T>()
  template<class T>
  T const& asA() const throw() {
    return *dynamic_cast<T const*>(this);
  }
};
typedef xju::Shared<Parser> PR;

class ZeroOrMore{};

PR operator*(ZeroOrMore a, PR b) throw();

extern ZeroOrMore zeroOrMore;

// result parses an a then a b
PR operator+(PR a, PR b) throw();

// result parses an a or a b
PR operator|(PR a, PR b) throw();

// result throws if x succeeds; otherwise does
// not consume anything
PR operator!(PR x) throw();

class AtLeastOne{};

PR operator*(AtLeastOne a, PR b) throw();

class NamedParser_ : public Parser
{
public:
  virtual ~NamedParser_() throw()
  {
  }
};
  
// pre: ItemType is a ast::CompositeItem
template<class ItemType>
class NamedParser : public NamedParser_
{
public:
  std::string const name_;
  PR x_;
  
  virtual ~NamedParser() throw() {
  }
  
  explicit NamedParser(std::string const& name, PR const x) throw():
    name_(name),
    x_(x) {
  }

  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw() 
  {
    std::unique_ptr<hcp_trace::Scope> scope;
    if (o.trace_) {
      std::ostringstream s;
      s << "parse " << target() << " at " << at;
      scope = std::unique_ptr<hcp_trace::Scope>(
        new hcp_trace::Scope(s.str(), XJU_TRACED));
    }
    ParseResult r(x_->parse(at, o));
    if (!r.failed()) {
      PV a(*r);
      if (!a.first.size()) {
        // composite needs an item
        a.first.push_back(IR(new hcp_ast::String(at, at)));
      }
      return ParseResult(PV(IRs(1U, new ItemType(a.first)), a.second));
    }
    if (o.trace_) {
      scope->fail();
    }
    return r;
  }
  // Parser::
  virtual std::string target() const throw() {
    return name_;
  }
};

extern AtLeastOne atLeastOne;
extern PR parseAnyChar;
PR parseOneOfChars(std::string const& chars) throw();

PR parseUntil(PR const x) throw();

PR parseUntil(PR const match, PR const until) throw();

PR parseLiteral(std::string const& x) throw();
extern PR whitespaceChar;
extern PR eatWhite;             // matches nothing
extern PR nonBackslashQuote;    // to next backslash or quote
extern PR nonQuote;
extern PR doubleQuote;
extern PR backslash;
extern PR oneChar;
extern PR stringLiteral;
extern PR comments;
extern PR hashIncludeImpl; // include preprocessor directive, with trailing "// impl" marker
extern PR hashInclude; // include preprocessor directive
extern PR hash;        // other preprocessor directive
extern PR typedef_statement;    // restriction: no anon class/struct/enum
extern PR using_statement;       // using statement
extern PR scoped_enum_def;
extern PR enum_def;
extern PR type_name; // eg x::y::Z, X<T>::size, x::Y<Z>
extern PR function_decl;
extern PR template_function_def;
extern PR function_def; // matches template, so try template_function_def first
extern PR attr_decl;
extern PR static_var_def;
extern PR class_decl;
extern PR class_def;    // template/non-template
extern PR anonymous_namespace;
extern PR namespace_def;    // matches anonymous, so try anonymous_namespace first
extern PR file;             // ensures at end of file

// Parse text, balancing (), [], {}, stringLiteral and optionally <>, 
// up to first match of until.
PR balanced(PR until, bool angles=false) throw();
}

#endif
