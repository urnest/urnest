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
#include "xju/assert.hh"

std::string getClassName(hcp_ast::ClassDef const* x) throw()
{
  return x->className_;
}

class OStream
{
public:
  // pre: lifetime(s) includes lifetime(this)
  explicit OStream(std::ostream& s, bool mapFileAndLine) throw():
      s_(s),
      line_(1),
      column_(1),
      mapFileAndLine_(mapFileAndLine)
  {
  }
  template<class T>
  void copy(T const& x) throw()
  {
    std::ostringstream s;
    s << x;
    std::string ss(s.str());
    hcp_ast::I i(ss.begin(), ss.end());
    while(!i.atEnd()) {
      ++i;
    }
    s_ << ss;
    if (i.line_ != 1) {
      line_+=i.line_-1;
      column_=i.column_;
    }
    else
    {
      column_+=i.column_-1;
    }
  }
  void copy(hcp_ast::I begin, hcp_ast::I end)
  {
    if(line_ != begin.line_) {
      if (column_!=1) {
        s_ << std::endl;
      }
      if (mapFileAndLine_) {
        s_ << "#line " << begin.line_ << std::endl;
      }
    }
    while(begin != end) {
      s_ << (*begin++);
    }
    line_=end.line_;
    column_=end.column_;
  }
  void copy(hcp_ast::IRs const& irs)
  {
    for(hcp_ast::IRs::const_iterator i=irs.begin();
        i != irs.end();
        ++i) {
      copy((*i)->begin(), (*i)->end());
    }
  }
  
private:
  std::ostream& s_;
  int line_;
  int column_;
  bool mapFileAndLine_;
};
template<class T>
OStream& operator<<(OStream& s, T const& x)
{
  s.copy(x);
  return s;
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

  std::vector<hcp_ast::IR>::const_iterator k(x.items_.begin());
  xju::assert_(*k, hcp_ast::isA_<hcp_ast::KeywordStatic>);
  ++k;
  c.copy(hcp_ast::asA_<hcp_ast::KeywordStatic>(x.items_[0]).end(),
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
  for(hcp_ast::IRs::const_iterator i=x.items_.begin(); 
      i!=x.items_.end(); 
      ++i) 
  {
    if ((*i)->isA<hcp_ast::ClassMembers>()) {
      for(auto const m: (*i)->asA<hcp_ast::ClassMembers>().items_) {
        if (m->isA<hcp_ast::FunctionDef>() &&
            !isFriendFunction(m->asA<hcp_ast::FunctionDef>())) {
          std::vector<hcp_ast::ClassDef const*> scope(outerClasses);
          scope.push_back(&x);
          genClassMemberFunctionDef(m->asA<hcp_ast::FunctionDef>(), h, c, 
                                    scope);
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

void genGlobalVar(hcp_ast::GlobalVarDef const& x,
                  OStream& h,
                  OStream& c) throw(
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

void genAnonymousNamespace(hcp_ast::AnonymousNamespace const& x,
                           OStream& h,
                           OStream& c) throw(
                             xju::Exception)
{
  c.copy(x.begin(), x.end());
}


void genNamespaceContent(hcp_ast::IRs const& x,
                         OStream& h,
                         OStream& c) throw(
                           xju::Exception);

void genNamespace(hcp_ast::NamespaceDef const& x,
                  OStream& h,
                  OStream& c) throw(
                    xju::Exception)
{
  std::vector<hcp_ast::IR>::const_iterator i(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::NamespaceMembers>));
  xju::assert_not_equal(i, x.items_.end());

  c << "namespace " << x.namespaceName_ << "\n"
    << "{\n";

  h.copy(hcp_ast::IRs(x.items_.begin(), i));
  
  genNamespaceContent((*i)->asA<hcp_ast::NamespaceMembers>().items_, h, c);
  
  h.copy(hcp_ast::IRs(xju::next(i), x.items_.end()));
  c << "}\n";
}


void genNamespaceContent(hcp_ast::IRs const& x,
                         OStream& h,
                         OStream& c) throw(
                           xju::Exception)
{
  for(hcp_ast::IRs::const_iterator i=x.begin(); i!=x.end(); ++i) {
    if ((*i)->isA<hcp_ast::AnonymousNamespace>()) {
      genAnonymousNamespace((*i)->asA<hcp_ast::AnonymousNamespace>(), h, c);
    }
    else if ((*i)->isA<hcp_ast::NamespaceDef>()) {
      genNamespace((*i)->asA<hcp_ast::NamespaceDef>(), h, c);
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
  explicit CommandLineOptions(unsigned int dir_levels, 
                              bool th,
                              bool tc,
                              std::string hpath) throw():
      dir_levels_(dir_levels),
      th_(th),
      tc_(tc),
      hpath_(hpath)
  {
  }
  unsigned int dir_levels_;
  bool th_;
  bool tc_;
  std::string hpath_;
};

// result.second are remaining arguments
std::pair<CommandLineOptions, std::vector<std::string> > parseCommandLine(
  std::vector<std::string> const& x) throw(
    xju::Exception)
{
  std::vector<std::string>::const_iterator i(x.begin());
  unsigned int dir_levels=0;
  bool th=false;
  bool tc=true;
  std::string hpath="";
  
  while((i != x.end()) && ((*i)[0]=='-')) {
    if ((*i)=="-l") {
      ++i;
      dir_levels=xju::stringToUInt(hcp::getOptionValue("-l", i, x.end()));
      ++i;
    }
    else if ((*i)=="-th") {
      th=true;
      ++i;
    }
    else if ((*i)=="-ntc") {
      tc=false;
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
    else {
      std::ostringstream s;
      s << "unknown option " << (*i)
        << " (only know -l, -th, -hpath)";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
  }
  return std::make_pair(CommandLineOptions(dir_levels, th, tc, hpath), 
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
                       unsigned int dir_levels) throw(
                         xju::Exception)
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

int main(int argc, char* argv[])
{
  try {
    std::pair<CommandLineOptions, std::vector<std::string> > const cmd_line(
      parseCommandLine(std::vector<std::string>(argv+1, argv+argc)));
    
    if (cmd_line.second.size() != 3) {
      std::cout << "usage: " << argv[0] 
                << " [-th] [-l <levels> | -hpath <path>] <input-file>"
                << " <output-header-file>"
                << " <output-cpp-file>" << std::endl;
      std::cout << "-l defaults to 0, which generates #includes without "
                << "any directory part; non-zero generates that many levels of relative path in output-cpp-file's include statement for output-header-file"
                << std::endl;
      std::cout << "-hpath overrides -l, and uses the specified path in"
                << " output-cpp-file's include statement for"
                << " output-header-file, eg -hpath x/y generates"
                << " #include <x/y/idl.hh>"
                << std::endl;
      std::cout << "-th tracks source file and line in generated header file"
                << std::endl;
      std::cout << "-ntc disables source file and line in generated cc file"
                << std::endl;
      return 1;
    }

    std::pair<xju::path::AbsolutePath, xju::path::FileName> const inputFile(
      xju::path::split(cmd_line.second[0]));
    
    std::string const x(hcp::readFile(inputFile));

    hcp_parser::I at(x.begin(), x.end());
    hcp_ast::CompositeItem root;
    at = hcp_parser::parse(root, at, hcp_parser::file());
    xju::assert_equal(root.items_.size(), 1U);

    
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
    
    fh << "#ifndef " << guard << std::endl
       << "#define " << guard << std::endl;
    if (cmd_line.first.th_) {
      fh << "#line 1 \""<<xju::path::str(inputFile)<<"\"" << std::endl;
    }
    
    xju::path::RelativePath const hhinc(
      std::vector<xju::path::DirName>(
        inputFile.first.end()-cmd_line.first.dir_levels_,
        inputFile.first.end()));

    if (cmd_line.first.hpath_.size()) {
      fc << "#include <" 
         << (cmd_line.first.hpath_+outputHH.second._)
         << ">" << std::endl;
      if (cmd_line.first.tc_) {
        fc << "#line 1 \""<<xju::path::str(inputFile)<<"\"" << std::endl;
      }
    }
    else
    {
      if (hhinc.size()) {
        fc << "#include <" 
           << xju::path::str(hhinc, outputHH.second)
           << ">" << std::endl
           << "#line 1 \""<<xju::path::str(inputFile)<<"\"" << std::endl;
      }
      else
      {
        fc << "#include \"" 
           << xju::path::str(hhinc, outputHH.second)
           << "\"" << std::endl;
        if (cmd_line.first.tc_) {
          fc << "#line 1 \""<<xju::path::str(inputFile)<<"\"" << std::endl;
        }
      }
    }
    OStream oh(fh, cmd_line.first.th_);
    OStream oc(fc, cmd_line.first.tc_);
    
    genNamespaceContent(
      root.items_.front()->asA<hcp_ast::File>().items_, oh, oc);
    
    fh << "#endif" << std::endl;
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
