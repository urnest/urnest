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
#include <optional>
#include <vector>
#include <xju/startsWith.hh>

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
PR kw_pub() noexcept
{
  static PR result(keyword("pub"));
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

bool lookingAt(I i,std::string const& x) noexcept
{
  auto xi{x.begin()};
  for(; !i.atEnd() && (xi!=x.end()) && (*i == *xi); ++i, ++xi);
  return (xi==x.end());
}

class ParseRustBalanced : public Parser
{
public:
  PR const until_;
  bool angles_;
  
  explicit ParseRustBalanced(PR const until, bool angles) throw():
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
      if (lookingAt(end,"r"))
      {
        ParseResult const r2(rustRawStringLiteral()->parse_(end, o));
        if (!r2.failed()) {
          end=(*r2).second;
        }
        else {
          ++end;
        }
      }
      else{
        switch(*end) {
        case '\'':
        {
          ParseResult const r2(
            (parseOneOfChars("'")+c_char()+parseOneOfChars("'"))->parse_(
              end, o));
          if (!r2.failed()) {
            end=(*r2).second;
          }
          else {
            ++end;
          }
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
          ParseResult const r2(ParseRustBalanced(parseOneOfChars("}"), angles_).parse_(xju::next(end),o));
          if (r2.failed()) {
            return r2;
          }
          end=xju::next((*r2).second);
        }
        break;
        case '<':
          if (angles_) {
            ParseResult const r2(
              ParseRustBalanced(parseOneOfChars(">"), angles_).parse_(
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
            ParseRustBalanced(parseOneOfChars("]"), angles_).parse_(
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
            ParseRustBalanced(parseOneOfChars(")"), angles_).parse_(
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

PR rustBalanced(PR until, bool angles=false) throw()
{
  return PR(new ParseRustBalanced(until, angles));
}

class RecurseItem : public Parser
{
public:
  explicit RecurseItem() throw()
  {
  }

  // Parser::
  virtual ParseResult parse_(I const at, Options const& o) throw() override;

  // Parser::
  virtual std::string target() const throw() override;
};

PR recurseItem() noexcept
{
  static PR result(
    new RecurseItem());
  return result;
}

PR modDef() noexcept
{
  static PR result(
    named<ModDef>(
      "mod def",
      optional(kw_unsafe())+kw_mod()+eatWhite()+name()+eatWhite()+
      "{" + eatWhite()+
      parseUntil(recurseItem()+eatWhite(),"}")+"}"));
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
      (keyword("const") |
       keyword("async") |
       kw_unsafe() |
       (kw_extern()+eatWhite()+abi()))+eatWhite()));
  return result;
}

PR fn() noexcept
{
  static PR result(
    functionQualifiers()+eatWhite()+
    keyword("fn")+parseUntil("(")+"("+rustBalanced(")")+")"+
    parseUntil("{")+"{"+rustBalanced("}")+"}"+eatWhite());
  return result;
}
PR typeAlias() noexcept
{
  static PR result(
    keyword("type")+rustBalanced(";")+
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
    keyword("struct")+rustBalanced(structTerminator()));
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
    rustBalanced(")")+")");
  return result;
}

PR regularStruct() noexcept
{
  static PR result(
    someStructIntro()+"{"+
    rustBalanced("}")+"}");
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
    keyword("enum")+rustBalanced("{")+"{"+rustBalanced("}")+"}");
  return result;
}

PR union_() noexcept
{
  static PR result(
    keyword("union")+rustBalanced("{")+"{"+rustBalanced("}")+"}");
  return result;
}

PR constant() noexcept
{
  static PR result(
    keyword("const")+rustBalanced(";")+";");
  return result;
}

PR attr() noexcept
{
  static PR result(
    "#"+eatWhite()+"["+rustBalanced("]")+"]");
  return result;
}

PR static_() noexcept
{
  static PR result(
    keyword("static")+rustBalanced(";")+";");
  return result;
}
  
PR trait() noexcept
{
  static PR result(
    optional(kw_unsafe()+eatWhite())+keyword("trait")+rustBalanced("{")+"{"+rustBalanced("}")+"}");
  return result;
}

PR impl() noexcept
{
  static PR result(
    optional(kw_unsafe()+eatWhite())+keyword("impl")+rustBalanced("{")+"{"+rustBalanced("}")+"}");
  return result;
}

PR externBlock() noexcept
{
  static PR result(
    optional(kw_unsafe()+eatWhite())+kw_extern()+eatWhite()+abi()+rustBalanced("{")+"{"+rustBalanced("}")+"}");
  return result;
}

PR item() noexcept
{
  static PR result(
    kw_pub() |
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
    static_() |
    trait() |
    impl() |
    externBlock() |
    attr());  // note last to allow items e.g. externCrate to pick up specific attributes
  return result;
}

ParseResult RecurseItem::parse_(I const at, Options const& o) throw()
{
  return item()->parse_(at, o);
}
std::string RecurseItem::target() const throw() {
  return item()->target();
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

struct CrateDirItemTag{};
typedef hcp_ast::TaggedItem<CrateDirItemTag> CrateDirItem;

PR parseCrateDir() noexcept
{
  static PR result(
    named<CrateDirItem>(
      "crate dir",
      parseUntil("\n"))+"\n");
  return result;
}

std::vector<xju::path::AbsolutePath> readCrateDirsFile(xju::path::AbsFile const& f)
{
  try{
    std::vector<xju::path::AbsolutePath> result;
    auto const s(xju::file::read(f));
    auto const p(hcp_parser::parseString(s.begin(),s.end(),(zeroOrMore()*parseCrateDir())));
    auto const dirNames(hcp_ast::findChildrenOfType<CrateDirItem>(p));
    std::transform(dirNames.begin(), dirNames.end(),
                   std::back_inserter(result),
                   [](auto const& x){
                     return xju::path::splitdir(hcp_ast::reconstruct(x));
                   });
    return result;
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "read crate search directories (for rustc -L) one per line from " << xju::path::str(f);
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

    /*
'/home/xju/urnest/omnicxy/xju/Array.hh'
'/home/xju/urnest/xju/Array.hh'
'/home/xju/tmp/oc930/xjutv/FILES/hcp-gen.virdir_spec.17441.vir_dir/xju/Array.hh'
'/home/xju/tmp/oc930/xjutv/FILES/h/idl-gen.virdir_spec.19274.vir_dir/xju/Array.hh'
'/home/xju/gcc-9.3.0-run/include/xju/Array.hh'
=''
    */
void genModViewSpec(hcp_ast::Item cons& r, xju::path::AbsolutePath const& rDir,
                    std::ostream& mod_view_spec)
{
  for(hcp_ast::IR c: r.items()){
    auto const modDef(dynamic_cast<ModDef const*>(&c));
    auto const modDecl(dynamic_cast<ModDecl const*>(&c));
    if (modDef){
      std::string const modName(hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<Name>(*modDef)));
      genModViewSpec(*modDef, REVISIT: rDir + xju::path::DirName(modName), mod_view_spec);
    }
    if (modDecl){
      auto const explicitPath(hcp_ast::findChildrenOfType<Path>(*modDecl));
      if (explicitPath.size()){
        mod_view_spec << "'" << xju::path::split(explicitPath.front(), rDir) << "'" << "\n"
                      << "=''\n";
      }
      else{
        std::string const modName(hcp_ast::reconstruct(hcp_ast::findOnlyChildOfType<Name>(*modDef)));
        mod_view_spec
          << "'" << xju::path::str(
            xju::path::AbsFile(
              rDir, xju::path::FileName(modName+".rs"))) << "'" << "\n"
          << "'" << xju::path::str(
            xju::path::AbsFile(
              rDir+xju::path::DirName(modName), xju::path::FileName("mod.rs"))) << "'" << "\n"
          << "=''\n";
      }
    }
  }
}

int main(int argc, char* argv[])
{
  if (argc < 2) {
    std::cerr << "usage: " << argv[0]
              << "[-m mods.view_spec] [-c crates.view-spec] [-L crate-dir-ls-file] file.rs"
              << std::endl;
    return 1;
  }
  try{
    std::optional<xju::path::AbsFile> mods_view_spec_file;
    std::optional<xju::path::AbsFile> crates_view_spec_file;
    std::vector<xju::path::AbsolutePath> crates_search_directories;
    
    auto i(argv+1);
    auto const end(argv+argc);
    while (i!=end && xju::startsWith(std::string(*i), std::string("-"))){
      if (*i == std::string("-m")){
        if (++i==end){
          throw xju::Exception("expected mods.view_spec output filename after -m",XJU_TRACED);
        }
        mods_view_spec_file=xju::path::split(*i);
      }
      else if (*i == std::string("-c")){
        if (++i==end){
          throw xju::Exception("expected crates.view_spec output filename after -c",XJU_TRACED);
        }
        crates_view_spec_file=xju::path::split(*i);
      }
      else if (*i == std::string("-L")){
        if (++i==end){
          throw xju::Exception("expected crates.view_spec output filename after -c",XJU_TRACED);
        }
        crates_search_directories = readCrateDirsFile(xju::path::split(*++i));
      }
      ++i;
    }

    auto const thisFile(xju::path::split(argv[1]));
    auto const thisModDir(
      xju::path::basename(thisFile) == xju::path::FileName("mod.rs")?
      xju::path::dirname(thisFile) :
      xju::path::dirname(thisFile)+xju::path::DirName(
        xju::path::split(xju::path::basename(thisFile)).first._));

    std::string const content(xju::file::read(thisFile));
    
    auto const r(parseString(content.begin(), content.end(), parseRS()));

    std::ostringstream mod_view_spec;
    genModViewSpec(r, thisModDir, mod_view_spec);

    std::ostringstream crates_view_spec;
    genCrateViewSpec(r, crates_view_spec);
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
