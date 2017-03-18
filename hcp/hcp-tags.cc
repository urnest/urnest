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

#include <algorithm>
#include <hcp/ast.hh>
#include <hcp/parser.hh>
#include <hcp/getOptionValue.hh>
#include <ostream>
#include <vector>
#include <xju/Exception.hh>
#include <utility>
#include <string>
#include <sstream>
#include <ctype.h>
#include <xju/path.hh>
#include <xju/readFile.hh>
#include <fstream>
#include <xju/format.hh>
#include <xju/stringToUInt.hh>
#include "xju/assert.hh"
#include <xju/Int.hh>
#include "xju/Tagged.hh"

class LineNumberTag{};
typedef xju::Int<LineNumberTag,unsigned int> LineNumber;

class SymbolTag{};
typedef xju::Tagged<std::string,SymbolTag> Symbol;
  
std::string getClassName(hcp_ast::ClassDef const* x) throw()
{
  return x->className_;
}

Symbol qualify(Symbol const& x, std::string const& namespaceName) throw()
{
  return Symbol(namespaceName+"::"+x._);
}

template<class AstClass>
AstClass const& findFirst(
  std::vector<hcp_ast::IR>::const_iterator begin,
  std::vector<hcp_ast::IR>::const_iterator end) throw() 
{
  std::vector<hcp_ast::IR>::const_iterator i(
    std::find_if(begin, end, hcp_ast::isA_<AstClass>));
  if (i==end) {
    std::vector<std::string> have;
    std::transform(begin,end,
                   std::back_inserter(have),
                   [](hcp_ast::IR x) { return x->str(); });
    xju::assert_not_equal(have,have);
  }
  return (*i)->asA<AstClass>();
}

std::map<Symbol,LineNumber> genNamespaceContent(hcp_ast::IRs const& x) throw(
  xju::Exception);

std::map<Symbol,LineNumber> genClass(hcp_ast::ClassDef const& x) throw(
  xju::Exception)
{
  std::map<Symbol,LineNumber> result;
  result.insert(std::make_pair(Symbol(x.className_),
                               LineNumber(x.begin().line_)));

  auto const members(findFirst<hcp_ast::ClassMembers>(
                       x.items_.begin(),x.items_.end()));
  auto const memberSymbols(genNamespaceContent(members.items_));
  
  for(auto s: memberSymbols) {
    result.insert(std::make_pair(qualify(s.first,x.className_),s.second));
  }
  return result;
}

std::map<Symbol,LineNumber> genTemplateClass(
  hcp_ast::TemplateClassDef const& x) throw(
    xju::Exception)
{
  std::map<Symbol,LineNumber> result;
  std::string const className(hcp_ast::ClassDef::getClassName(x.items_));
  result.insert(std::make_pair(Symbol(className),
                               LineNumber(x.begin().line_)));

  auto const members(findFirst<hcp_ast::ClassMembers>(
                       x.items_.begin(),x.items_.end()));
  auto const memberSymbols(genNamespaceContent(members.items_));
  
  for(auto s: memberSymbols) {
    result.insert(std::make_pair(qualify(s.first,className),s.second));
  }
  return result;
}

std::pair<Symbol,LineNumber> genFunction(hcp_ast::FunctionDef const& x) throw(
  xju::Exception)
{
  auto y(findFirst<hcp_ast::FunctionName>(x.items_.begin(), x.items_.end()));
  Symbol symbol(reconstruct(y));
  LineNumber lineNumber(y.begin().line_);
  return std::make_pair(symbol,lineNumber);
}

std::pair<Symbol,LineNumber> genFunctionDecl(
  hcp_ast::FunctionDecl const& x) throw(
    xju::Exception)
{
  auto const y(findFirst<hcp_ast::FunctionName>(
                 x.items_.begin(), x.items_.end()));
  Symbol symbol(reconstruct(y));
  LineNumber lineNumber(y.begin().line_);
  return std::make_pair(symbol,lineNumber);
}

std::pair<Symbol,LineNumber> genTypedef(
  hcp_ast::Typedef const& x) throw(
    xju::Exception)
{
  std::vector<hcp_ast::IR>::const_iterator i(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::DefinedType>));
  xju::assert_not_equal(i, x.items_.end());
  Symbol symbol(reconstruct(**i));
  LineNumber lineNumber((**i).begin().line_);
  return std::make_pair(symbol,lineNumber);
}

std::pair<Symbol,LineNumber> genEnumDef(
  hcp_ast::EnumDef const& x) throw(
    xju::Exception)
{
  std::vector<hcp_ast::IR>::const_iterator i(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::EnumName>));
  xju::assert_not_equal(i, x.items_.end());
  Symbol symbol(reconstruct(**i));
  LineNumber lineNumber((**i).begin().line_);
  return std::make_pair(symbol,lineNumber);
}

std::pair<Symbol,LineNumber> genGlobalVar(hcp_ast::GlobalVarDef const& x) throw(
  xju::Exception)
{
  
  std::vector<hcp_ast::IR>::const_iterator i(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::VarName>));
  xju::assert_not_equal(i, x.items_.end());
  Symbol symbol(reconstruct(**i));
  LineNumber lineNumber((**i).begin().line_);
  return std::make_pair(symbol,lineNumber);
}

std::map<Symbol,LineNumber> genNamespace(hcp_ast::NamespaceDef const& x) throw(
  xju::Exception)
{
  std::vector<hcp_ast::IR>::const_iterator i(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::NamespaceMembers>));
  xju::assert_not_equal(i, x.items_.end());

  std::map<Symbol,LineNumber> content(
    genNamespaceContent((*i)->asA<hcp_ast::NamespaceMembers>().items_));
  
  std::map<Symbol,LineNumber> result;
  for(auto s: content) {
    result.insert(std::make_pair(qualify(s.first,x.namespaceName_),s.second));
  }
  return result;
}

std::map<Symbol,LineNumber> genNamespaceContent(hcp_ast::IRs const& x) throw(
  xju::Exception)
{
  std::map<Symbol,LineNumber> result;
  
  for(hcp_ast::IRs::const_iterator i=x.begin(); i!=x.end(); ++i) {
    if ((*i)->isA<hcp_ast::NamespaceDef>()) {
      auto symbols(genNamespace((*i)->asA<hcp_ast::NamespaceDef>()));
      for(auto s : symbols) {
        result.insert(s);
      }
    }
    else if ((*i)->isA<hcp_ast::TemplateClassDef>()) {
      auto const symbols(genTemplateClass((*i)->asA<hcp_ast::TemplateClassDef>()));
      for(auto s : symbols) {
        result.insert(s);
      }
    }
    else if ((*i)->isA<hcp_ast::ClassDef>()) {
      auto const symbols(genClass((*i)->asA<hcp_ast::ClassDef>()));
      for(auto s : symbols) {
        result.insert(s);
      }
    }
    else if ((*i)->isA<hcp_ast::FunctionDef>())
    {
      result.insert(genFunction((*i)->asA<hcp_ast::FunctionDef>()));
    } 
    else if ((*i)->isA<hcp_ast::FunctionDecl>())
    {
      result.insert(genFunctionDecl((*i)->asA<hcp_ast::FunctionDecl>()));
    } 
    else if ((*i)->isA<hcp_ast::Typedef>())
    {
      result.insert(genTypedef((*i)->asA<hcp_ast::Typedef>()));
    } 
    else if ((*i)->isA<hcp_ast::EnumDef>())
    {
      result.insert(genEnumDef((*i)->asA<hcp_ast::EnumDef>()));
    } 
    else if ((*i)->isA<hcp_ast::GlobalVarDef>())
    {
      result.insert(genGlobalVar((*i)->asA<hcp_ast::GlobalVarDef>()));
    }
  }
  return result;
}

typedef std::pair<xju::path::AbsolutePath, xju::path::FileName> File;

std::string formatSymbol(
  std::pair<Symbol,std::pair<File,LineNumber> > const& symbol) throw()
{
  std::ostringstream s;
  s << xju::format::quote("::"+symbol.first._) << ":" << "[{"
    << xju::format::quote("f") << ":"
    << xju::format::quote(str(symbol.second.first)) << ","
    << xju::format::quote("l") << ":"
    << symbol.second.second.value() << "}]";
  return s.str();
}

int main(int argc, char* argv[])
{
  try {
    
    std::map<Symbol, std::pair<File,LineNumber> > result;

    for(char** arg=argv+1; arg!=argv+argc; ++arg) {
      File const inputFile(xju::path::split(*arg));
      std::string const x(xju::readFile(xju::path::str(inputFile)));
      try {
        hcp_parser::I at(x.begin(), x.end());
        hcp_ast::CompositeItem root;
        at = hcp_parser::parse(root, at, hcp_parser::file);
        xju::assert_equal(root.items_.size(), 1U);
        std::map<Symbol,LineNumber> const symbols(
          genNamespaceContent(
            root.items_.front()->asA<hcp_ast::File>().items_));
        for(auto s: symbols) {
          result.insert(std::make_pair(s.first,
                                       std::make_pair(inputFile,s.second)));
        }
      }
      catch(xju::Exception& e) {
        std::ostringstream s;
        s << "parse file " << xju::path::str(inputFile);
        e.addContext(s.str(),XJU_TRACED);
        std::cerr << "Warning: " << readableRepr(e);
      }
    }
    std::cout << "{" << std::endl
              << xju::format::join(result.begin(),
                                   result.end(),
                                   formatSymbol,",\n")
              << "}" << std::endl;
    return 0;
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << xju::format::join(argv, argv+argc, " ");
    e.addContext(s.str(), XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 2;
  }
}
