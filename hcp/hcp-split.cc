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
static const char rcsid[] = "$RCSfile: twt_cxx.el,v $ $Revision: 1.2 $";

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

std::string getClassName(hcp_ast::ClassDef const* x) throw()
{
  return x->className_;
}

void genClassMemberFunctionDef(
  hcp_ast::FunctionDef const& x,
  std::ostream& h,
  std::ostream& c,
  std::vector<hcp_ast::ClassDef const*> const& scope) throw(
    xju::Exception)
{
  std::vector<hcp_ast::IR>::const_iterator i(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::FunctionImpl>));
  xju::assert_not_equal(i, x.items_.end());
  h << std::string(x.begin().x_, (*i)->begin().x_) << ";" << std::endl;
  
  c << "\n#line " << x.begin().line_ << std::endl;
  std::vector<hcp_ast::IR>::const_iterator j(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::FunctionName>));
  xju::assert_not_equal(j, x.items_.end());

  std::vector<hcp_ast::IR>::const_iterator k(x.items_.begin());
  if ((*k)->isA<hcp_ast::FunctionQualifiers>()) {
    k=xju::next(k);
  }
  c << std::string((*k)->begin().x_, (*j)->begin().x_)
    << xju::format::join(scope.begin(),
                         scope.end(),
                         getClassName,
                         "::")
    << "::"
    << std::string((*j)->begin().x_, x.end().x_) << std::endl;
  h << "\n#line " << x.end().line_ << std::endl;
}

void genClass(hcp_ast::ClassDef const& x,
              std::ostream& h,
              std::ostream& c,
              std::vector<hcp_ast::ClassDef const*> const& outerClasses) throw(
                xju::Exception)
{
  for(hcp_ast::IRs::const_iterator i=x.items_.begin(); i!=x.items_.end(); ++i) {
    if ((*i)->isA<hcp_ast::FunctionDef>()) {
      std::vector<hcp_ast::ClassDef const*> scope(outerClasses);
      scope.push_back(&x);
      genClassMemberFunctionDef((*i)->asA<hcp_ast::FunctionDef>(), h, c, scope);
    }
    else if ((*i)->isA<hcp_ast::ClassDef>()) {
      std::vector<hcp_ast::ClassDef const*> outer(outerClasses);
      outer.push_back(&x);
      genClass((*i)->asA<hcp_ast::ClassDef>(), h, c, outer);
    }
    else {
      h << hcp_ast::reconstruct(**i);
    }
  }
}

void genAnonymousNamespace(hcp_ast::AnonymousNamespace const& x,
                           std::ostream& h,
                           std::ostream& c) throw(
                             xju::Exception)
{
  c << "\n#line " << x.begin().line_ << std::endl;
  c << hcp_ast::reconstruct(x);
}


void genNamespaceContent(hcp_ast::IRs const& x,
                         std::ostream& h,
                         std::ostream& c) throw(
                           xju::Exception);

void genNamespace(hcp_ast::NamespaceDef const& x,
                  std::ostream& h,
                  std::ostream& c) throw(
                    xju::Exception)
{
  std::vector<hcp_ast::IR>::const_iterator i(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::NamespaceMembers>));
  xju::assert_not_equal(i, x.items_.end());

  c << "\n#line " << x.begin().line_ << std::endl;
  c << "namespace " << x.namespaceName_ << std::endl
    << "{" << std::endl;

  h << hcp_ast::reconstruct(hcp_ast::IRs(x.items_.begin(), i));
  
  genNamespaceContent((*i)->asA<hcp_ast::NamespaceMembers>().items_, h, c);
  
  h << hcp_ast::reconstruct(hcp_ast::IRs(xju::next(i), x.items_.end()));
  c << "}" << std::endl;
}


void genNamespaceContent(hcp_ast::IRs const& x,
                         std::ostream& h,
                         std::ostream& c) throw(
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
      c << "\n#line " << (**i).begin().line_ << std::endl;
      c << hcp_ast::reconstruct(**i);
    }
    else {
      h << hcp_ast::reconstruct(**i);
    }
  }
}

class CommandLineOptions
{
public:
  explicit CommandLineOptions(unsigned int dir_levels) throw():
    dir_levels_(dir_levels)
  {
  }
  unsigned int dir_levels_;
};

// result.second are remaining arguments
std::pair<CommandLineOptions, std::vector<std::string> > parseCommandLine(
  std::vector<std::string> const& x) throw(
    xju::Exception)
{
  std::vector<std::string>::const_iterator i(x.begin());
  unsigned int dir_levels=0;
  
  while((i != x.end()) && ((*i)[0]=='-')) {
    if ((*i)=="-l") {
      ++i;
      dir_levels=xju::stringToUInt(hcp::getOptionValue("-l", i, x.end()));
      ++i;
    }
    else {
      std::ostringstream s;
      s << "unknown option " << (*i)
        << " (only know -l)";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
  }
  return std::make_pair(CommandLineOptions(dir_levels), 
                        std::vector<std::string>(i, x.end()));
}

char make_guard_char(char c) throw()
{
  if (c=='/') {
    c='_';
  }
  else if (c=='.') {
    c='_';
  }
  return ::toupper(c);
}

std::string make_guard(xju::path::AbsolutePath const& pathToInputFile,
                       xju::path::FileName const& inputFileName,
                       unsigned int dir_levels) throw(
                         xju::Exception)
{
  std::ostringstream s;
  s << "make include-guard from last " << dir_levels
    << " levels of " << str(pathToInputFile)
    << " and " << inputFileName;
  try {
    std::string x(
      xju::path::str(
        xju::path::RelativePath(
          std::vector<xju::path::DirName>(
            pathToInputFile.end()-dir_levels,
            pathToInputFile.end())),
        inputFileName));
    std::transform(x.begin(), x.end(),
                   x.begin(),
                   make_guard_char);
    return x;
  }
  catch(xju::Exception& e) {
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

int main(int argc, char* argv[])
{
  try {
    std::pair<CommandLineOptions, std::vector<std::string> > const cmd_line(
      parseCommandLine(std::vector<std::string>(argv+1, argv+argc)));
    
    if (cmd_line.second.size() != 3) {
      std::cout << "usage: " << argv[0] 
                << " [-l <levels>] <input-file> <output-header-file>"
                << " <output-cpp-file>" << std::endl;
      std::cout << "-l default to 0, which generates #includes without "
                << "any directory part" << std::endl;
      return 1;
    }

    std::pair<xju::path::AbsolutePath, xju::path::FileName> const inputFile(
      xju::path::split(cmd_line.second[0]));
    
    std::pair<xju::path::AbsolutePath, xju::path::FileName> const outputHH(
      xju::path::split(cmd_line.second[1]));
    std::pair<xju::path::AbsolutePath, xju::path::FileName> const outputCC(
      xju::path::split(cmd_line.second[2]));
    
    std::string const guard(make_guard(inputFile.first,
                                       inputFile.second,
                                       cmd_line.first.dir_levels_));
    
    std::string const x(xju::readFile(xju::path::str(inputFile)));

    hcp_parser::I at(x.begin(), x.end());
    hcp_ast::CompositeItem root;
    at = parse(root, at, hcp_parser::file);
    xju::assert_equal(root.items_.size(), 1U);
    
    std::ofstream fh(xju::path::str(outputHH).c_str(), 
                     std::ios_base::out|std::ios_base::trunc);
    std::ofstream fc(xju::path::str(outputCC).c_str(), 
                     std::ios_base::out|std::ios_base::trunc);
    
    fh << "#ifndef " << guard << std::endl
       << "#define " << guard << std::endl
       << "#line 1 \""<<xju::path::str(inputFile)<<"\"" << std::endl;
    fh << "\n";
    
    xju::path::RelativePath const hhinc(
      std::vector<xju::path::DirName>(
        inputFile.first.end()-cmd_line.first.dir_levels_,
        inputFile.first.end()));
    
    fc << "#include <" 
       << xju::path::str(hhinc, outputHH.second)
       << ">" << std::endl
       << "#line 1 \""<<xju::path::str(inputFile)<<"\"" << std::endl;
    
    genNamespaceContent(
      root.items_.front()->asA<hcp_ast::File>().items_, fh, fc);
    
    fh << "#endif" << std::endl;
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << xju::format::join(argv, argv+argc, " ");
    e.addContext(s.str(), XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 2;
  }
}
