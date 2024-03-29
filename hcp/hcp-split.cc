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
#include <hcp/readFile.hh>
#include <fstream>
#include <xju/format.hh>
#include <xju/stringToUInt.hh>
#include <xju/assert.hh>
#include <map>
#include <xju/startsWith.hh>
#include <sys/types.h>
#include <xju/strip.hh>
#include <xju/prev.hh>

std::string getClassName(hcp_ast::ClassDef const* x) throw()
{
  return x->className_;
}

class OStream
{
public:
  // pre: lifetime(s) includes lifetime(this)
  explicit OStream(std::ostream& s) throw():
      s_(s),
      outputOffset_(0)
  {
  }
  void copy(std::string const& x) throw()
  {
    s_ << x;
    outputOffset_+=x.size();
  }
  template<class T>
  void copy(T const& x) throw()
  {
    std::ostringstream s;
    s << x;
    std::string ss(s.str());
    copy(ss);
  }
  void copy(hcp_ast::I begin, hcp_ast::I end)
  {
    if (mappedOffset()!=begin.offset()){
      sourceOffsetMap_.insert(std::make_pair(outputOffset_,begin.offset()));
    }
    xju::assert_equal(mappedOffset(),begin.offset());
    copy(std::string(begin.x_,end.x_));
  }
  void copy(hcp_ast::IRs const& irs)
  {
    for(hcp_ast::IRs::const_iterator i=irs.begin();
        i != irs.end();
        ++i) {
      copy((*i)->begin(), (*i)->end());
    }
  }
  template<class T, class ...Us>
  void copyExcluding(hcp_ast::IRs const& irs)
  {
    for(auto ir: irs){
      if (!hcp_ast::IsAnyOf<T,Us...>(ir)){
        if (ir->items().size()){
          copyExcluding<T,Us...>(ir->items());
        }
        else{
          copy(ir->begin(),ir->end());
        }
      }
    }
  }
  std::map<off_t,off_t> const& getSourceOffsetMap() const throw(){
    return sourceOffsetMap_;
  }
private:
  std::ostream& s_;
  off_t outputOffset_;
  std::map<off_t,off_t> sourceOffsetMap_; //outputOffset->sourceOffset
  off_t mappedOffset() const throw(){
    auto i(sourceOffsetMap_.upper_bound(outputOffset_));
    if (i==sourceOffsetMap_.begin()){
      return outputOffset_;
    }
    --i;
    return outputOffset_-(*i).first+(*i).second;
  }
};
template<class T>
OStream& operator<<(OStream& s, T const& x)
{
  s.copy(x);
  return s;
}

hcp_ast::I findStartOfTrailingWhitespace(hcp_ast::IR const x) throw()
{
  if (hcp_ast::isA_<hcp_ast::WhiteSpace>(x)){
    return x->begin();
  }
  if (x->items().size()){
    return findStartOfTrailingWhitespace(*xju::prev(x->items().end()));
  }
  return x->end();
}

void genClassMemberFunctionDef(
  hcp_ast::FunctionDef const& x,
  OStream& h,
  OStream& c,
  std::vector<hcp_ast::ClassDef const*> const& scope) /*throw(
    xju::Exception)*/
{
  std::vector<hcp_ast::IR>::const_iterator const i(
    std::find_if(x.items().begin(), x.items().end(),
                 hcp_ast::isA_<hcp_ast::FunctionImpl>));
  xju::assert_not_equal(i, x.items().end());
  xju::assert_not_equal(i, x.items().begin());

  hcp_ast::I const startOfProtoTrailingWhitespace(
    findStartOfTrailingWhitespace(*xju::prev(i)));
  
  hcp_ast::I const startOfImplTrailingWhitespace(
    findStartOfTrailingWhitespace(*i));
  
  h.copy(x.begin(),startOfProtoTrailingWhitespace);
  h << ";";
  h.copy(startOfImplTrailingWhitespace,x.end());
  
  std::vector<hcp_ast::IR>::const_iterator k(x.items().begin());
  if ((*k)->isA<hcp_ast::FunctionQualifiers>()) {
    k=xju::next(k);
  }
  std::vector<hcp_ast::IR>::const_iterator j(
    std::find_if(k, x.items().end(),
                 hcp_ast::isA_<hcp_ast::FunctionName>));
  xju::assert_not_equal(j, x.items().end());

  c.copy((*k)->begin(), (*j)->begin());
  c << xju::format::join(scope.begin(),
                         scope.end(),
                         getClassName,
                         "::")
    << "::";
  auto const l(std::find_if(j,x.items().end(),
                            hcp_ast::isA_<hcp_ast::FunctionImpl>));
  xju::assert_not_equal(l,x.items().end());
  c.copyExcluding<hcp_ast::VarInitialiser,hcp_ast::VirtSpecifierSeq>(
    hcp_ast::IRs(j, l));
  c.copy((*l)->begin(), x.end());
  c << "\n";
}

void genClassStaticVarDef(
  hcp_ast::StaticVarDef const& x,
  OStream& h,
  OStream& c,
  std::vector<hcp_ast::ClassDef const*> const& scope) /*throw(
    xju::Exception)*/
{
  auto i(hcp_ast::findChildrenOfType<hcp_ast::VarInitialiser>(x));
  if (i.size()){
    h.copy(x.begin(), i[0].get().begin());
    h << ";\n";
  }
  else
  {
    h.copy(x.begin(), x.end());
    h << "\n";
  }

  auto name(hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(x));

  std::vector<hcp_ast::IR>::const_iterator k(x.items().begin());
  xju::assert_(*k, hcp_ast::isA_<hcp_ast::KeywordStatic>);
  ++k;
  c.copy(hcp_ast::asA_<hcp_ast::KeywordStatic>(x.items().front()).end(),
         name.begin());
  c << xju::format::join(scope.begin(),
                         scope.end(),
                         getClassName,
                         "::")
    << "::";
  c.copy(name.begin(), x.end());
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

  
bool isInlineFunction(hcp_ast::FunctionDef const& x) /*throw(
  xju::Exception)*/
{
  auto const qualifiers{
    hcp_ast::findChildrenOfType<hcp_ast::FunctionQualifiers>(x)};
  if (!qualifiers.size()){
    return false;
  }
  return (std::find_if(
            qualifiers.front().get().items().begin(),
            qualifiers.front().get().items().end(),
            isLiteral("inline"))!=qualifiers.front().get().items().end());
}

bool isFriendFunction(hcp_ast::FunctionDef const& x) /*throw(
  xju::Exception)*/
{
  auto const qualifiers{
    hcp_ast::findChildrenOfType<hcp_ast::FunctionQualifiers>(x)};
  if (!qualifiers.size()){
    return false;
  }
  return (std::find_if(
            qualifiers.front().get().items().begin(),
            qualifiers.front().get().items().end(),
            isLiteral("friend"))!=qualifiers.front().get().items().end());
}

bool isFriendFunction(hcp_ast::FunctionDecl const& x) /*throw(
  xju::Exception)*/
{
  auto const qualifiers{
    hcp_ast::findChildrenOfType<hcp_ast::FunctionQualifiers>(x)};
  if (!qualifiers.size()){
    return false;
  }
  return (std::find_if(
            qualifiers.front().get().items().begin(),
            qualifiers.front().get().items().end(),
            isLiteral("friend"))!=qualifiers.front().get().items().end());
}

bool isFriendOperatorLessDecl(hcp_ast::FunctionDecl const& x,
                              std::string const& className) throw()
{
  if (hcp_ast::findChildrenOfType<hcp_ast::TemplatePreamble>(x).size()){
    return false;
  }
  if (!isFriendFunction(x)){
    return false;
  }
  auto const returnType{hcp_ast::findChildrenOfType<hcp_ast::ReturnType>(x)};
  if (!returnType.size()||
      xju::strip(hcp_ast::reconstruct(returnType.front()))!="bool"){
    return false;
  }
  auto const functionName{
    hcp_ast::findChildrenOfType<hcp_ast::FunctionName>(x).front()};
  auto const operatorName{
    hcp_ast::findChildrenOfType<hcp_ast::OperatorName>(x)};
  if (!operatorName.size()||
      hcp_ast::reconstruct(operatorName.front())!="<"){
    return false;
  }
  auto const params(hcp_ast::findChildrenOfType<hcp_ast::Param>(x));
  if (params.size()!=2){
    return false;
  }
  auto const filtered{
    hcp_ast::filter(
      params,
      [&](hcp_ast::Param const& p){
        if (!hcp_ast::findChildrenOfType<hcp_ast::VarNonFp>(p).size()||
            hcp_ast::findChildrenOfType<hcp_ast::MoveQual>(p).size()||
            hcp_ast::findChildrenOfType<hcp_ast::PointerQual>(p).size()||
            hcp_ast::findChildrenOfType<hcp_ast::ElipsesQual>(p).size()||
            hcp_ast::findChildrenOfType<hcp_ast::ArrayDecl>(p).size()){
          return false;
        }
        if (hcp_ast::findChildrenOfType<hcp_ast::RefQual>(p).size()&&
            !hcp_ast::findChildrenOfType<hcp_ast::ConstQual>(p).size()){
          return false;
        }
        auto const scopedName{
          hcp_ast::findChildrenOfType<hcp_ast::ScopedName>(p)};
        if (scopedName.size()!=1){
          return false;
        }
        if (xju::strip(className)!=xju::strip(hcp_ast::reconstruct(
                                                scopedName[0]))){
          return false;
        }
        return true;
      })};
  if (filtered.size()!=2){
    return false;
  }
  return true;
}

std::vector<std::string> getClassMemberVarNames(hcp_ast::ClassDef const& x)
  noexcept
{
  auto const globalVars{
    hcp_ast::findChildrenOfType<hcp_ast::GlobalVarDef>(x)};
  std::vector<std::string> result;
  for(auto c : globalVars){
    result.push_back(
      hcp_ast::reconstruct(
        hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(c)));
  }
  return result;
}

void genClass(hcp_ast::ClassDef const& x,
              OStream& h,
              OStream& c,
              std::vector<hcp_ast::ClassDef const*> const& outerClasses) /*throw(
                xju::Exception)*/
{
  for(hcp_ast::IRs::const_iterator i=x.items().begin(); 
      i!=x.items().end(); 
      ++i) 
  {
    if ((*i)->isA<hcp_ast::ClassMembers>()) {
      for(auto const m: (*i)->asA<hcp_ast::ClassMembers>().items()) {
        if (m->isA<hcp_ast::FunctionDef>()) {
          if (isInlineFunction(m->asA<hcp_ast::FunctionDef>())) {
            h.copy(m->begin(),m->end());
          }
          else if (isFriendFunction(m->asA<hcp_ast::FunctionDef>())) {
            //REVISIT: split
            h.copy(m->begin(), m->end());
          }
          else {
            std::vector<hcp_ast::ClassDef const*> scope(outerClasses);
            scope.push_back(&x);
            genClassMemberFunctionDef(m->asA<hcp_ast::FunctionDef>(), h, c, 
                                      scope);
          }
        }
        else if (m->isA<hcp_ast::StaticVarDef>()) {
          std::vector<hcp_ast::ClassDef const*> scope(outerClasses);
          scope.push_back(&x);
          genClassStaticVarDef(m->asA<hcp_ast::StaticVarDef>(), h, c, scope);
        }
        else if (m->isA<hcp_ast::ClassDef>()) {
          std::vector<hcp_ast::ClassDef const*> outer(outerClasses);
          outer.push_back(&x);
          genClass(m->asA<hcp_ast::ClassDef>(), h, c, outer);
        }
        else if (m->isA<hcp_ast::FunctionDecl>() &&
                 isFriendOperatorLessDecl(m->asA<hcp_ast::FunctionDecl>(),
                                          x.className_)){
          std::vector<std::string> const memberVarNames{
            getClassMemberVarNames(x)};
          h.copy(m->begin(), m->end());
          h << "friend bool operator>(" << getClassName(&x) << " const& x, "
            << getClassName(&x) << " const& y) noexcept;\n";

          h << "friend bool operator!=(" << getClassName(&x) << " const& x, "
            << getClassName(&x) << " const& y) noexcept;\n";

          h << "friend bool operator==(" << getClassName(&x) << " const& x, "
            << getClassName(&x) << " const& y) noexcept;\n";

          h << "friend bool operator<=(" << getClassName(&x) << " const& x, "
            << getClassName(&x) << " const& y) noexcept;\n";

          h << "friend bool operator>=(" << getClassName(&x) << " const& x, "
            << getClassName(&x) << " const& y) noexcept;\n";

          std::vector<hcp_ast::ClassDef const*> scope(outerClasses);
          scope.push_back(&x);
          auto const scopedName{
            xju::format::join(scope.begin(),
                              scope.end(),
                              getClassName,
                              "::")};
          c << "bool operator<(" << scopedName << " const& x, "
            << scopedName << " const& y) noexcept\n{\n";
          for(auto v:memberVarNames){
            c << "  if (x." << v << " < y." << v << ") return true;\n";
            c << "  if (y." << v << " < x." << v << ") return false;\n";
          }
          c << "  return false;\n"
            << "}\n";

          c << "bool operator>(" << scopedName << " const& x, "
            << scopedName << " const& y) noexcept\n{\n"
            << "  return y<x;\n"
            << "}\n";

          c << "bool operator!=(" << scopedName << " const& x, "
            << scopedName << " const& y) noexcept\n{\n"
            << "  return y<x||x<y;\n"
            << "}\n";

          c << "bool operator==(" << scopedName << " const& x, "
            << scopedName << " const& y) noexcept\n{\n"
            << "  return !(y<x||x<y);\n"
            << "}\n";

          c << "bool operator<=(" << scopedName << " const& x, "
            << scopedName << " const& y) noexcept\n{\n"
            << "  return !(x>y);\n"
            << "}\n";

          c << "bool operator>=(" << scopedName << " const& x, "
            << scopedName << " const& y) noexcept\n{\n"
            << "  return !(x<y);\n"
            << "}\n";
        }
        else {
          h.copy(m->begin(), m->end());
        }
      }
    }
    else {
      h.copy((*i)->begin(), (*i)->end());
    }
  }
}

std::vector<std::string> getTemplateClassMemberVarNames(
  hcp_ast::TemplateClassDef const& x) noexcept
{
  auto const globalVars{
    hcp_ast::findChildrenOfType<hcp_ast::GlobalVarDef>(x)};
  std::vector<std::string> result;
  for(auto c : globalVars){
    result.push_back(
      hcp_ast::reconstruct(
        hcp_ast::findOnlyChildOfType<hcp_ast::VarName>(c)));
  }
  return result;
}

void genTemplateClass(hcp_ast::TemplateClassDef const& x,
                      OStream& h,
                      OStream& c) /*throw(
                xju::Exception)*/
{
  for(hcp_ast::IRs::const_iterator i=x.items().begin(); 
      i!=x.items().end(); 
      ++i) 
  {
    if ((*i)->isA<hcp_ast::ClassMembers>()) {
      for(auto const m: (*i)->asA<hcp_ast::ClassMembers>().items()) {
        if (m->isA<hcp_ast::FunctionDecl>() &&
            isFriendOperatorLessDecl(m->asA<hcp_ast::FunctionDecl>(),
                                     x.className_)){
          std::vector<std::string> const memberVarNames{
            getTemplateClassMemberVarNames(x)};

          h << "friend bool operator<(" << x.className_ << " const& x, "
            << x.className_ << " const& y) noexcept\n{\n";
          for(auto v:memberVarNames){
            h << "  if (x." << v << " < y." << v << ") return true;\n";
            h << "  if (y." << v << " < x." << v << ") return false;\n";
          }
          h << "  return false;\n"
            << "}\n";

          h << "friend bool operator>(" << x.className_ << " const& x, "
            << x.className_ << " const& y) noexcept {\n"
            << "  return y<x;\n"
            << "}\n";

          h << "friend bool operator!=(" << x.className_ << " const& x, "
            << x.className_ << " const& y) noexcept {\n"
            << "  return y<x||x<y;\n"
            << "}\n";

          h << "friend bool operator==(" << x.className_ << " const& x, "
            << x.className_ << " const& y) noexcept {\n"
            << "  return !(y<x||x<y);\n"
            << "}\n";

          h << "friend bool operator<=(" << x.className_ << " const& x, "
            << x.className_ << " const& y) noexcept {\n"
            << "  return !(x>y);\n"
            << "}\n";


          h << "friend bool operator>=(" << x.className_ << " const& x, "
            << x.className_ << " const& y) noexcept {\n"
            << "  return !(x<y);\n"
            << "}\n";

        }
        else {
          h.copy(m->begin(), m->end());
        }
      }
    }
    else {
      h.copy((*i)->begin(), (*i)->end());
    }
  }
}

void genFunction(hcp_ast::FunctionDef const& x,
                 OStream& h,
                 OStream& c) /*throw(
                   xju::Exception)*/
{
  if (isInlineFunction(x)) {
    h.copy(x.begin(),x.end());
  }
  else{
    std::vector<hcp_ast::IR>::const_iterator i(
      std::find_if(x.items().begin(), x.items().end(),
                   hcp_ast::isA_<hcp_ast::FunctionImpl>));
    xju::assert_not_equal(i, x.items().end());
    xju::assert_not_equal(i, x.items().begin());
    
    hcp_ast::I const startOfProtoTrailingWhitespace(
      findStartOfTrailingWhitespace(*xju::prev(i)));
    
    hcp_ast::I const startOfImplTrailingWhitespace(
      findStartOfTrailingWhitespace(*i));
    
    h.copy(x.begin(),startOfProtoTrailingWhitespace);
    h << ";";
    h.copy(startOfImplTrailingWhitespace,x.end());
    
    std::vector<hcp_ast::IR>::const_iterator j(
      std::find_if(x.items().begin(), x.items().end(),
                   hcp_ast::isA_<hcp_ast::FunctionName>));
    xju::assert_not_equal(j, x.items().end());
    
    auto const l(std::find_if(j,x.items().end(),
                              hcp_ast::isA_<hcp_ast::FunctionImpl>));
    xju::assert_not_equal(l,x.items().end());
    c.copy(x.begin(),(*j)->begin());
    c.copyExcluding<hcp_ast::VarInitialiser>(hcp_ast::IRs(j, l));
    c.copy((*l)->begin(), x.end());
    c << "\n";
  }
}

void genGlobalVar(hcp_ast::GlobalVarDef const& x,
                  OStream& h,
                  OStream& c) /*throw(
                    xju::Exception)*/
{
  auto const y(
    hcp_ast::findOnlyChildOfType<hcp_ast::VarInitialiser>(x));
  h << "extern ";
  h.copy(x.begin(), y.begin());
  h << ";\n";
  
  c.copy(x.begin(), x.end());
}

void genAnonymousNamespace(hcp_ast::AnonymousNamespace const& x,
                           OStream& h,
                           OStream& c) /*throw(
                             xju::Exception)*/
{
  c.copy(x.begin(), x.end());
}


void genNamespaceContent(hcp_ast::IRs const& x,
                         OStream& h,
                         OStream& c) /*throw(
                           xju::Exception)*/;

void genNamespace(hcp_ast::NamespaceDef const& x,
                  OStream& h,
                  OStream& c) /*throw(
                    xju::Exception)*/
{
  std::vector<hcp_ast::IR>::const_iterator i(
    std::find_if(x.items().begin(), x.items().end(),
                 hcp_ast::isA_<hcp_ast::NamespaceMembers>));
  xju::assert_not_equal(i, x.items().end());

  c << "namespace " << x.namespaceName_ << "\n"
    << "{\n";

  h.copy(hcp_ast::IRs(x.items().begin(), i));
  
  genNamespaceContent((*i)->asA<hcp_ast::NamespaceMembers>().items(), h, c);
  
  h.copy(hcp_ast::IRs(xju::next(i), x.items().end()));
  c << "}\n";
}


void genNamespaceContent(hcp_ast::IRs const& x,
                         OStream& h,
                         OStream& c) /*throw(
                           xju::Exception)*/
{
  for(hcp_ast::IRs::const_iterator i=x.begin(); i!=x.end(); ++i) {
    if ((*i)->isA<hcp_ast::AnonymousNamespace>()) {
      genAnonymousNamespace((*i)->asA<hcp_ast::AnonymousNamespace>(), h, c);
    }
    else if ((*i)->isA<hcp_ast::NamespaceDef>()) {
      genNamespace((*i)->asA<hcp_ast::NamespaceDef>(), h, c);
    }
    else if ((*i)->isA<hcp_ast::TemplateClassDef>()) {
      genTemplateClass((*i)->asA<hcp_ast::TemplateClassDef>(), h, c);
    }
    else if ((*i)->isA<hcp_ast::ClassDef>()) {
      genClass((*i)->asA<hcp_ast::ClassDef>(), h, c, 
               std::vector<hcp_ast::ClassDef const*>());
    }
    else if ((*i)->isA<hcp_ast::HashIncludeImpl>())
    {
      c.copy((*i)->begin(), (*i)->end());
    }
    else if ((*i)->isA<hcp_ast::FunctionDef>())
    {
      genFunction((*i)->asA<hcp_ast::FunctionDef>(), h, c);
    }
    else if ((*i)->isA<hcp_ast::GlobalVarDef>())
    {
      genGlobalVar((*i)->asA<hcp_ast::GlobalVarDef>(), h, c);
    }
    else {
      h.copy((*i)->begin(), (*i)->end());
    }
  }
}

class CommandLineOptions
{
public:
  explicit CommandLineOptions(unsigned int const dir_levels, 
                              std::string const hpath,
                              bool const includeGeneratedBy) throw():
      dir_levels_(dir_levels),
      hpath_(hpath),
      includeGeneratedBy_(includeGeneratedBy)
  {
  }
  unsigned int dir_levels_;
  std::string hpath_;
  bool includeGeneratedBy_;
};

// result.second are remaining arguments
std::pair<CommandLineOptions, std::vector<std::string> > parseCommandLine(
  std::vector<std::string> const& x) /*throw(
    xju::Exception)*/
{
  std::vector<std::string>::const_iterator i(x.begin());
  unsigned int dir_levels=0;
  std::string hpath="";
  bool includeGeneratedBy=false;
  
  while((i != x.end()) && ((*i)[0]=='-')) {
    if ((*i)=="-l") {
      ++i;
      dir_levels=xju::stringToUInt(hcp::getOptionValue("-l", i, x.end()));
      ++i;
    }
    else if ((*i)=="-hpath") {
      ++i;
      hpath=hcp::getOptionValue("-hpath", i, x.end());
      ++i;
      if (hpath.size()==0 || (*hpath.rend())!='/') {
        hpath=hpath+"/";
      }
    }
    else if ((*i)=="-G") {
      includeGeneratedBy=true;
      ++i;
    }
    else {
      std::ostringstream s;
      s << "unknown option " << (*i)
        << " (only know -l, -th, -hpath)";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
  }
  return std::make_pair(
    CommandLineOptions(dir_levels, hpath, includeGeneratedBy), 
    std::vector<std::string>(i, x.end()));
}

char make_guard_char(char c) throw()
{
  if (('a'<=c && c<='z') ||
      ('A'<=c && c<='Z') ||
      ('0'<=c && c<='9') ||
      (c=='_')) {
    return ::toupper(c);
  }
  return '_';
}

std::string make_guard(xju::path::AbsolutePath const& pathToInputFile,
                       xju::path::FileName const& outputHHFileName,
                       unsigned int dir_levels) /*throw(
                         xju::Exception)*/
{
  std::ostringstream s;
  s << "make include-guard from last " << dir_levels
    << " levels of " << str(pathToInputFile)
    << " and " << outputHHFileName;
  try {
    std::string x(
      xju::path::str(
        xju::path::RelativePath(
          std::vector<xju::path::DirName>(
            pathToInputFile.end()-dir_levels,
            pathToInputFile.end())),
        outputHHFileName));
    std::transform(x.begin(), x.end(),
                   x.begin(),
                   make_guard_char);
    return "_"+x;
  }
  catch(xju::Exception& e) {
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}
std::string make_guard(std::string x) throw()
{
  std::transform(x.begin(), x.end(),
                 x.begin(),
                 make_guard_char);
  return "_"+x;
}

void writeOffsetMap(std::ostream& s,
                  std::string const& inputFileName,
                  std::map<off_t,off_t> const& offsetMap) throw(){
  s << inputFileName << std::endl;
  for(auto const x:offsetMap){
    s << x.first << " " << x.second << std::endl;
  }
}

std::string chooseHpath(std::string const& hpath,
                        xju::path::RelativePath const& hhinc) throw(){
  if (hpath.size()==0){
    if (hhinc.size()==0){
      return "";
    }
    return xju::path::str(hhinc)+"/";
  }
  if (xju::startsWith(hpath,std::string("/"))){
    return xju::path::str(std::make_pair(xju::path::absolute_dirname(hpath),
                                         xju::path::basename(hpath)))+"/";
  }
  return xju::path::str(std::make_pair(xju::path::relative_dirname(hpath),
                                       xju::path::basename(hpath)))+"/";
}

int main(int argc, char* argv[])
{
  try {
    std::pair<CommandLineOptions, std::vector<std::string> > const cmd_line(
      parseCommandLine(std::vector<std::string>(argv+1, argv+argc)));
    
    if (cmd_line.second.size() < 3) {
      std::cerr << "usage: " << argv[0] 
                << " [-G] [-l <levels> | -hpath <path>] <input-file>"
                << " <output-header-file>"
                << " <output-cpp-file>"
                << " [header-offset-map-file] [cpp-offset-map]" << std::endl;
      std::cerr << "-l defaults to 0, which generates #includes without "
                << "any directory part; non-zero generates that many levels of relative path in output-cpp-file's include statement for output-header-file"
                << std::endl;
      std::cerr << "-hpath overrides -l, and uses the specified path in"
                << " output-cpp-file's include statement for"
                << " output-header-file, eg -hpath x/y generates"
                << " #include <x/y/idl.hh>"
                << std::endl;
      std::cerr << "-G includes generated-by comments" << std::endl;
      return 1;
    }

    std::pair<xju::path::AbsolutePath, xju::path::FileName> const inputFile(
      xju::path::split(cmd_line.second[0]));
    
    std::string const x(hcp::readFile(inputFile));

    auto const root{
      hcp_parser::parseString(x.begin(),x.end(), hcp_parser::file())};
    
    std::pair<xju::path::AbsolutePath, xju::path::FileName> const outputHH(
      xju::path::split(cmd_line.second[1]));
    std::pair<xju::path::AbsolutePath, xju::path::FileName> const outputCC(
      xju::path::split(cmd_line.second[2]));
    
    std::string const guard(
      cmd_line.first.hpath_.size()?
        make_guard(cmd_line.first.hpath_+outputHH.second._)
      : 
        make_guard(inputFile.first,
                   outputHH.second,
                   cmd_line.first.dir_levels_));
    
    std::ofstream fh(xju::path::str(outputHH).c_str(), 
                     std::ios_base::out|std::ios_base::trunc);
    std::ofstream fc(xju::path::str(outputCC).c_str(), 
                     std::ios_base::out|std::ios_base::trunc);
    
    OStream oh(fh);
    OStream oc(fc);

    xju::path::RelativePath const hhinc(
      std::vector<xju::path::DirName>(
        inputFile.first.end()-cmd_line.first.dir_levels_,
        inputFile.first.end()));

    std::string hpath(chooseHpath(
                        cmd_line.first.hpath_,
                        hhinc));

    std::string const hhGeneratedBy(
      "// hcp-split: generated as "+
      hpath+xju::path::str(outputHH.second)+ " from "+
      xju::format::str(inputFile.second));
    std::string const ccGeneratedBy(
      "// hcp-split: generated as "+
      hpath+xju::path::str(outputCC.second)+ " from "+
      xju::format::str(inputFile.second));
    
    oh << "#ifndef " << guard << "\n"
       << "#define " << guard << "\n";
    if (cmd_line.first.includeGeneratedBy_){
      oh << hhGeneratedBy << "\n";
    }
    if (cmd_line.first.hpath_.size()) {
      oc << "#include <" 
         << (cmd_line.first.hpath_+outputHH.second._)
         << ">" << "\n";
      if (cmd_line.first.includeGeneratedBy_){
        oc << ccGeneratedBy << "\n";
      }
    }
    else
    {
      if (hhinc.size()) {
        oc << "#include <" 
           << xju::path::str(hhinc, outputHH.second)
           << ">" << "\n";
        if (cmd_line.first.includeGeneratedBy_){
          oc << ccGeneratedBy << "\n";
        }
      }
      else
      {
        oc << "#include \"" 
           << outputHH.second
           << "\"" << "\n";
        if (cmd_line.first.includeGeneratedBy_){
          oc << ccGeneratedBy << "\n";
        }
      }
    }
    genNamespaceContent(
      root.items().front()->asA<hcp_ast::File>().items(), oh, oc);
    
    oh << "#endif" << "\n";

    if (cmd_line.second.size()>3){
      std::ofstream fh(cmd_line.second[3], 
                       std::ios_base::out|std::ios_base::trunc);
      writeOffsetMap(fh,xju::path::str(inputFile),oh.getSourceOffsetMap());
    }
        
    if (cmd_line.second.size()>4){
      std::ofstream fc(cmd_line.second[4], 
                       std::ios_base::out|std::ios_base::trunc);
      writeOffsetMap(fc,xju::path::str(inputFile),oc.getSourceOffsetMap());
    }

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
