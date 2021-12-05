// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <hcp/ast.hh>
#include <hcp/parser.hh>
#include <xju/file/read.hh>
#include <iostream>
#include <xju/path.hh>
#include <xju/Exception.hh>

struct ModDefTag {};
typedef hcp_ast::TaggedItem<ModDefTag> ModDef;

struct ModDeclTag {};
typedef hcp_ast::TaggedItem<ModDeclTag> ModDecl;

struct ExternCrateTag {};
typedef hcp_ast::TaggedItem<ExternCrateTag> ExternCrate;

struct NameTag {};
typedef hcp_ast::TaggedItem<NameTag> Name;

struct PathTag {};
typedef hcp_ast::TaggedItem<PathTag> Path;

using namespace hcp_parser;

PR keyword(std::string const& x)
{
  PR result(x+!identifierContChar());
  return result;
}
PR kw_path() noexcept
{
  static PR result(keyword("path"));
  return result;
}
PR kw_mod() noexcept
{
  static PR result(keyword("mod"));
  return result;
}
PR kw_extern() noexcept
{
  static PR result(keyword("extern"));
  return result;
}
PR kw_crate() noexcept
{
  static PR result(keyword("crate"));
  return result;
}
PR kw_unsafe() noexcept
{
  static PR result(keyword("unsafe"));
  return result;
}
  
PR name() noexcept
{
  static PR result(
    named<Name>(
      "name",
      identifier()));
  return result;
}

PR pathAttr() noexcept
{
  static PR result(
    "#"+eatWhite()+"["+eatWhite()+kw_path()+eatWhite()+"="+eatWhite()+doubleQuote() +
    named<Path>(
      "path",
      parseUntil(doubleQuote())) +
    doubleQuote()+eatWhite()+"]");
  return result;
}

PR modDecl() noexcept
{
  static PR result(
    named<ModDecl>(
      "mod decl",
      optional(pathAttr())+eatWhite()+
      optional(kw_unsafe())+kw_mod()+eatWhite()+name()+eatWhite()+";"));
  return result;
}

PR modDef() noexcept
{
  static PR result(
    named<ModDef>(
      "mod def",
      optional(kw_unsafe())+kw_mod()+name()+eatWhite()+
      "{" + balanced("}",false)+ "}")); // REVISIT: want nested modules too via item
  return result;
}

PR externCrate() noexcept
{
  static PR result(
    named<ExternCrate>(
      "extern crate",
      kw_extern()+eatWhite()+kw_crate()+eatWhite()+
      name()+parseUntil(parseLiteral(";"))+";"));
  return result;
}

PR use() noexcept
{
  static PR result(
    keyword("use")+parseUntil(semicolon())+
    semicolon());
  return result;
}

// r"fred"
// r###"fred"###
class ParseRustRawStringLiteral : public Parser
{
public:
  ParseRustRawStringLiteral() noexcept
  : r_(parseLiteral("r"))
  {
  }
  PR r_;
  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw() 
  {
    try{
      auto r{r_->parse(at,o)};
      if (r.failed()){
        return r;
      }
      I i{(*r).second};
      int c{0};
      for(; *i != '"'; ++i,++c){
        if (*i != '#'){
          return ParseResult(
            Exception(
              std::shared_ptr<Exception::Cause const>(
                new InvalidDelimeterChar(i)),
              i, XJU_TRACED));
        }
      }
      std::string const endDelimeter(
        "\""+std::string(c, '#'));
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

PR rustRawStringLiteral() noexcept
{
  static PR result(named<hcp_ast::StringLiteral>(
                     "raw string literal",
                     PR(new ParseRustRawStringLiteral)));
  return result;
}

PR abi() noexcept
{
  static PR result(
    stringLiteral()|
    rustRawStringLiteral());
  return result;
}
PR functionQualifiers() noexcept
{
  static PR result(
    zeroOrMore()*(
      keyword("const") |
      keyword("async") |
      kw_unsafe() |
      (kw_extern()+eatWhite()+abi())));
  return result;
}

PR fn() noexcept
{
  static PR result(
    functionQualifiers()+eatWhite()+
    keyword("fn")+parseUntil("(")+"("+balanced(")")+")"+
    parseUntil("{")+"{"+balanced("}")+"}"+eatWhite());
  return result;
}
PR typeAlias() noexcept
{
  static PR result(
    keyword("type")+balanced(";")+
    ";");
  return result;
}

PR structTerminator()
{
  static PR result(
    parseOneOfChars("({;"));
  return result;
}
PR someStructIntro()
{
  static PR result(
    keyword("struct")+balanced(structTerminator()));
  return result;
}
PR unitStruct() noexcept
{
  static PR result(
    someStructIntro()+";");
  return result;
}

PR tupleStruct() noexcept
{
  static PR result(
    someStructIntro()+"("+
    balanced(")")+")");
  return result;
}

PR regularStruct() noexcept
{
  static PR result(
    someStructIntro()+"{"+
    balanced("}")+"}");
  return result;
}

PR struct_() noexcept
{
  static PR result(
    unitStruct()|
    tupleStruct()|
    regularStruct());
  return result;
}

PR enum_() noexcept
{
  static PR result(
    keyword("enum")+balanced("{")+"{"+balanced("}")+"}");
  return result;
}

PR union_() noexcept
{
  static PR result(
    keyword("union")+balanced("{")+"{"+balanced("}")+"}");
  return result;
}

PR constant() noexcept
{
  static PR result(
    keyword("const")+balanced(";")+";");
  return result;
}

PR attr() noexcept
{
  static PR result(
    "#"+eatWhite()+"["+balanced("]")+"]");
  return result;
}

PR item() noexcept
{
  static PR result(
    
    modDecl() |
    modDef() |
    externCrate() |
    use() |
    fn() |
    typeAlias() |
    struct_() |
    enum_()  |
    union_() |
    constant() |
/*
    static_() |
    trait() |
    impl() |
    externBlock() |
 */
    attr());  // note last to allow items e.g. externCrate to pick up specific attributes
  return result;
}
PR utf8BOM() noexcept
{
  static PR result(
    parseLiteral("\xEF\xBB\xBF"));
  return result;
}
PR shebang() noexcept
{
  static PR result(
    parseLiteral("#!")+parseUntil("\n"));
  return result;
}
PR parseRS() noexcept
{
  static PR result(hcp_parser::optional(utf8BOM())+
                   hcp_parser::optional(shebang())+
                   eatWhite()+
                   parseUntil(item()+eatWhite(), endOfFile()));
  return result;
}

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " file.rs" << std::endl;
    return 1;
  }
  try{
    std::string const content(xju::file::read(xju::path::split(argv[1])));
    
    auto const r(parseString(content.begin(), content.end(), parseRS()));
    return 0;
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "scan " << argv[1] << " for module and crate dependencies";
    e.addContext(s.str(),XJU_TRACED);
    std::cerr << xju::readableRepr(e) << std::endl;
    return 1;
  }
}
