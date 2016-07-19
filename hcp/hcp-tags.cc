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
  
// std::string getClassName(hcp_ast::ClassDef const* x) throw()
// {
//   return x->className_;
// }

// void genClassStaticVarDef(
//   hcp_ast::StaticVarDef const& x,
//   OStream& h,
//   OStream& c,
//   std::vector<hcp_ast::ClassDef const*> const& scope) throw(
//     xju::Exception)
// {
//   std::vector<hcp_ast::IR>::const_iterator i(
//     hcp_ast::find1stInTree(x.items_.begin(), x.items_.end(),
//                            hcp_ast::isA_<hcp_ast::VarInitialiser>));
//   if (i != x.items_.end()) {
//     h.copy(x.begin(), (*i)->begin());
//     h << ";\n";
//   }
//   else
//   {
//     h.copy(x.begin(), x.end());
//     h << "\n";
//   }
  
//   std::vector<hcp_ast::IR>::const_iterator j(
//     std::find_if(x.items_.begin(), x.items_.end(),
//                  hcp_ast::isA_<hcp_ast::VarName>));
//   xju::assert_not_equal(j, x.items_.end());

//   std::vector<hcp_ast::IR>::const_iterator k(x.items_.begin());
//   xju::assert_(*k, hcp_ast::isA_<hcp_ast::KeywordStatic>);
//   ++k;
//   c.copy((*k)->begin(), (*j)->begin());
//   c << xju::format::join(scope.begin(),
//                          scope.end(),
//                          getClassName,
//                          "::")
//     << "::";
//   c.copy((*j)->begin(), x.end());
//   c << "\n";
// }
/*
std::map<Symbol,LineNumber> genClass(hcp_ast::ClassDef const& x) throw(
  xju::Exception)
{
  std::map<Symbol,LineNumber> result;
  result.insert(std::make(pair(x.className_,x.begin().line_)));
  
  for(hcp_ast::IRs::const_iterator i=x.items_.begin(); i!=x.items_.end(); ++i) {
    if ((*i)->isA<hcp_ast::StaticVarDef>()) {
      std::vector<hcp_ast::ClassDef const*> scope(outerClasses);
      scope.push_back(&x);
      genClassStaticVarDef((*i)->asA<hcp_ast::StaticVarDef>(), h, c, scope);
    }
    else if ((*i)->isA<hcp_ast::ClassDef>()) {
      std::vector<hcp_ast::ClassDef const*> outer(outerClasses);
      outer.push_back(&x);
      genClass((*i)->asA<hcp_ast::ClassDef>(), h, c, outer);
    }
    else {
      h.copy((*i)->begin(), (*i)->end());
    }
  }
}
*/
std::pair<Symbol,LineNumber> genFunction(hcp_ast::FunctionDef const& x) throw(
  xju::Exception)
{
  std::vector<hcp_ast::IR>::const_iterator i(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::FunctionName>));
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

std::map<Symbol,LineNumber> genNamespaceContent(hcp_ast::IRs const& x) throw(
  xju::Exception);

/*
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
*/

std::map<Symbol,LineNumber> genNamespaceContent(hcp_ast::IRs const& x) throw(
  xju::Exception)
{
  std::map<Symbol,LineNumber> result;
  
  for(hcp_ast::IRs::const_iterator i=x.begin(); i!=x.end(); ++i) {
    /* if ((*i)->isA<hcp_ast::NamespaceDef>()) {
      auto symbols(genNamespace((*i)->asA<hcp_ast::NamespaceDef>()));
      for(s : symbols) {
        result.insert(s);
      }
    }
    else if ((*i)->isA<hcp_ast::ClassDef>()) {
      auto symbols(genClass((*i)->asA<hcp_ast::ClassDef>(), h, c, 
                            std::vector<hcp_ast::ClassDef const*>()));
      for(s : symbols) {
        result.insert(s);
      }
    }
    else */ if ((*i)->isA<hcp_ast::FunctionDef>())
    {
      result.insert(genFunction((*i)->asA<hcp_ast::FunctionDef>()));
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
  s << xju::format::quote("::"+symbol.first._) << ":" << "["
    << xju::format::quote(str(symbol.second.first)) << ","
    << symbol.second.second.value() << "]";
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
