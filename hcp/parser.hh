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

namespace hcp_parser
{
typedef hcp_ast::I I;
typedef hcp_ast::IR IR;
typedef std::vector<IR> IRs;
typedef std::pair<IRs, I> PV;

class ParseResult
{
public:
    explicit ParseResult(xju::Exception const& e) throw():
        e_(e)
    {
    }
    explicit ParseResult(PV v) throw():
        v_(v)
    {
    }
    PV operator*() const throw(
        xju::Exception)
    {
        if (e_.valid()) {
            throw e_.value();
        }
        return v_.value();
    }
    xju::Optional<xju::Exception> e_;
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
                   bool includeAllExceptionContext,
                   Cache cache) throw():
    trace_(trace),
    includeAllExceptionContext_(includeAllExceptionContext),
    cache_(cache) {
  }
  Options(Options const& y) throw():
      trace_(y.trace_),
      includeAllExceptionContext_(y.includeAllExceptionContext_),
      cache_(y.cache_) {
  }
    
  bool trace_;
  bool includeAllExceptionContext_;
  mutable xju::Shared<std::map<std::pair<I, Parser*const>, ParseResult> > cache_;
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

  ParseResult parse(I const at, Options const& options) throw() 
  {
    CacheKey const k(at, this);
    CacheVal::const_iterator i((*options.cache_).find(k));
    if (i == (*options.cache_).end()) {
        ParseResult result(parse_(at, options));
        if (result.e_.valid()) {
            std::ostringstream s;
            s << "parse " << target() << " at " << at;
            result.e_.value().addContext(s.str(), XJU_TRACED);
        }
        CacheVal::value_type v(k, result);
        i=(*options.cache_).insert(v).first;
    }
    return (*i).second;
  }

  I parse(hcp_ast::CompositeItem& parent, I const at, Options const& options)
    throw(
      // post: parent unmodified
      xju::Exception)
  {
    ParseResult const r(parse(at, options));
    PV const x(*r);
    std::copy(x.first.begin(), 
              x.first.end(), 
              std::back_inserter(parent.items_));
    return x.second;
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

// pre: ItemType is a CompositeItem
template<class ItemType>
class NamedParser : public Parser
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
    std::auto_ptr<hcp_trace::Scope> scope;
    if (o.trace_) {
      std::ostringstream s;
      s << "parse " << target() << " at " << at;
      scope = std::auto_ptr<hcp_trace::Scope>(
        new hcp_trace::Scope(s.str(), XJU_TRACED));
    }
    ParseResult r(x_->parse(at, o));
    if (r.v_.valid()) {
      PV& a(r.v_.value());
      if (!a.first.size()) {
        // composite needs an item
        a.first.push_back(IR(new hcp_ast::String(at, at)));
      }
      return ParseResult(PV(IRs(1U, new ItemType(a.first)), a.second));
    }
    else {
      if (o.includeAllExceptionContext_) {
        return r;
      }
      xju::Exception const& e(r.e_.value());
      return ParseResult(
        xju::Exception(e.cause().first, e.cause().second));
    }
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
extern PR enum_def;
extern PR function_decl;
extern PR template_function_def;
extern PR function_def; // matches template, so try template_function_def first
extern PR attr_decl;
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
