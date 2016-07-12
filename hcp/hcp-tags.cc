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

class LineNumberTag{};
typedef xju::Int<LineNumberTag,unsigned int> LineNumber;

class SymbolTag{};
typedef xju::Tagged<SymbolTag> Symbol;
  
std::string getClassName(hcp_ast::ClassDef const* x) throw()
{
  return x->className_;
}

void genClassMemberFunctionDef(
  hcp_ast::FunctionDef const& x,
  OStream& h,
  OStream& c,
  std::vector<hcp_ast::ClassDef const*> const& scope) throw(
    xju::Exception)
{
  std::vector<hcp_ast::IR>::const_iterator i(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::FunctionImpl>));
  xju::assert_not_equal(i, x.items_.end());
  h.copy(x.begin(), (*i)->begin());
  h << ";";
  std::vector<hcp_ast::IR>::const_iterator w(
    xju::next(i));
  xju::assert_not_equal(w, x.items_.end());
  xju::assert_equal(hcp_ast::isA_<hcp_ast::WhiteSpace>(*w),true);
  h.copy((*w)->begin(),(*w)->end()); //copy trailing whitespace
  
  std::vector<hcp_ast::IR>::const_iterator j(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::FunctionName>));
  xju::assert_not_equal(j, x.items_.end());

  std::vector<hcp_ast::IR>::const_iterator k(x.items_.begin());
  if ((*k)->isA<hcp_ast::FunctionQualifiers>()) {
    k=xju::next(k);
  }
  c.copy((*k)->begin(), (*j)->begin());
  c << xju::format::join(scope.begin(),
                         scope.end(),
                         getClassName,
                         "::")
    << "::";
  c .copy((*j)->begin(), x.end());
  c << "\n";
}

void genClassStaticVarDef(
  hcp_ast::StaticVarDef const& x,
  OStream& h,
  OStream& c,
  std::vector<hcp_ast::ClassDef const*> const& scope) throw(
    xju::Exception)
{
  std::vector<hcp_ast::IR>::const_iterator i(
    hcp_ast::find1stInTree(x.items_.begin(), x.items_.end(),
                           hcp_ast::isA_<hcp_ast::VarInitialiser>));
  if (i != x.items_.end()) {
    h.copy(x.begin(), (*i)->begin());
    h << ";\n";
  }
  else
  {
    h.copy(x.begin(), x.end());
    h << "\n";
  }
  
  std::vector<hcp_ast::IR>::const_iterator j(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::VarName>));
  xju::assert_not_equal(j, x.items_.end());

  std::vector<hcp_ast::IR>::const_iterator k(x.items_.begin());
  xju::assert_(*k, hcp_ast::isA_<hcp_ast::KeywordStatic>);
  ++k;
  c.copy((*k)->begin(), (*j)->begin());
  c << xju::format::join(scope.begin(),
                         scope.end(),
                         getClassName,
                         "::")
    << "::";
  c.copy((*j)->begin(), x.end());
  c << "\n";
}

class isLiteral
{
public:
  isLiteral(std::string const& literal) throw():
      literal_(literal)
  {
  }
  bool operator()(hcp_ast::IR const& x) const throw()
  {
    return literal_==hcp_ast::reconstruct(*x);
  }
  std::string literal_;
};

  
bool isFriendFunction(hcp_ast::FunctionDef const& x) throw(
  xju::Exception)
{
  hcp_ast::FunctionQualifiers const& q(
    (*x.items_.begin())->asA<hcp_ast::FunctionQualifiers>());
  return (std::find_if(q.items_.begin(), q.items_.end(),
                       isLiteral("friend"))!=q.items_.end());
}

void genClass(hcp_ast::ClassDef const& x,
              OStream& h,
              OStream& c,
              std::vector<hcp_ast::ClassDef const*> const& outerClasses) throw(
                xju::Exception)
{
  for(hcp_ast::IRs::const_iterator i=x.items_.begin(); i!=x.items_.end(); ++i) {
    if ((*i)->isA<hcp_ast::FunctionDef>() &&
        !isFriendFunction((*i)->asA<hcp_ast::FunctionDef>())) {
      std::vector<hcp_ast::ClassDef const*> scope(outerClasses);
      scope.push_back(&x);
      genClassMemberFunctionDef((*i)->asA<hcp_ast::FunctionDef>(), h, c, scope);
    }
    else if ((*i)->isA<hcp_ast::StaticVarDef>()) {
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

void genFunction(hcp_ast::FunctionDef const& x,
                 OStream& h,
                 OStream& c) throw(
                   xju::Exception)
{
  std::vector<hcp_ast::IR>::const_iterator i(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::FunctionImpl>));
  xju::assert_not_equal(i, x.items_.end());
  h.copy(x.begin(), (*i)->begin());
  h << ";";
  std::vector<hcp_ast::IR>::const_iterator w(
    xju::next(i));
  xju::assert_not_equal(w, x.items_.end());
  xju::assert_equal(hcp_ast::isA_<hcp_ast::WhiteSpace>(*w),true);
  h.copy((*w)->begin(),(*w)->end()); //copy trailing whitespace
  
  std::vector<hcp_ast::IR>::const_iterator j(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::FunctionName>));
  xju::assert_not_equal(j, x.items_.end());

  std::vector<hcp_ast::IR>::const_iterator k(x.items_.begin());
  if ((*k)->isA<hcp_ast::FunctionQualifiers>()) {
    k=xju::next(k);
  }
  c.copy(x.begin(), x.end());
}

std::pair<Symbol,LineNumber> genGlobalVar(hcp_ast::GlobalVarDef const& x) throw(
  xju::Exception)
{
  std::vector<hcp_ast::IR>::const_iterator i(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::VarInitialiser>));
  xju::assert_not_equal(i, x.items_.end());
  h << "extern ";
  h.copy(x.begin(), (*i)->begin());
  h << ";\n";
  
  c.copy(x.begin(), x.end());
}

std::map<Symbol,LineNumber> genNamespaceContent(hcp_ast::IRs const& x) throw(
  xju::Exception);

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
    else if ((*i)->isA<hcp_ast::FunctionDef>())
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


int main(int argc, char* argv[])
{
  try {
    typedef std::pair<xju::path::AbsolutePath, xju::path::FileName> File;
    
    std::map<Symbol, std::pair<File,LineNumber> > result;

    for(char** arg=argv+1; arg!=argv+argc; ++arg) {
      File const inputFile(xju::path::split(*arg));
      std::string const x(xju::readFile(xju::path::str(inputFile)));
      try {
        hcp_parser::I at(x.begin(), x.end());
        at = hcp_parser::parse(root, at, hcp_parser::file);
        hcp_ast::CompositeItem root;
        xju::assert_equal(root.items_.size(), 1U);
        std::map<Symbol,LineNumber> const symbols(
          genNamespaceContent(
            root.items_.front()->asA<hcp_ast::File>().items_, result));
      }
      catch(mcom::it::Exception& e) {
        std::cerr << "Warning: " << readableRepr(e);
      }
    }
    std::cout << "{" << std::endl
              << xju::join(formatSymbols(symbols,",\n"))
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
