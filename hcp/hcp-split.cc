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
        if (hcp_ast::isA_<hcp_ast::CompositeItem>(ir)){
          auto const c(hcp_ast::asA_<hcp_ast::CompositeItem>(ir));
          copyExcluding<T,Us...>(c.items_);
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

std::string reconstructWithoutTrailingWhitespace(
  std::vector<hcp_ast::IR>::const_iterator begin,
  std::vector<hcp_ast::IR>::const_iterator end) throw()
{
  if (begin==end){
    return std::string();
  }
  --end;
  std::string result(reconstruct(hcp_ast::IRs(begin,end)));
  if (hcp_ast::isA_<hcp_ast::WhiteSpace>(*end)){
    return result;
  }
  if (hcp_ast::isA_<hcp_ast::CompositeItem>(*end)){
    auto const c(hcp_ast::asA_<hcp_ast::CompositeItem>(*end));
    return result+reconstructWithoutTrailingWhitespace(
      c.items_.begin(),
      c.items_.end());
  }
  return result+reconstruct(*end);
}

std::string reconstructTrailingWhitespace(
  std::vector<hcp_ast::IR>::const_iterator begin,
  std::vector<hcp_ast::IR>::const_iterator end) throw()
{
  if (begin==end){
    return std::string();
  }
  --end;
  if (hcp_ast::isA_<hcp_ast::WhiteSpace>(*end)){
    return reconstruct(*end);
  }
  if (hcp_ast::isA_<hcp_ast::CompositeItem>(*end)){
    auto const c(hcp_ast::asA_<hcp_ast::CompositeItem>(*end));
    return reconstructTrailingWhitespace(
      c.items_.begin(),
      c.items_.end());
  }
  return "";
}

void genClassMemberFunctionDef(
  hcp_ast::FunctionDef const& x,
  OStream& h,
  OStream& c,
  std::vector<hcp_ast::ClassDef const*> const& scope) throw(
    xju::Exception)
{
  std::vector<hcp_ast::IR>::const_iterator const i(
    std::find_if(x.items_.begin(), x.items_.end(),
                 hcp_ast::isA_<hcp_ast::FunctionImpl>));
  xju::assert_not_equal(i, x.items_.end());

  std::string proto(reconstructWithoutTrailingWhitespace(
                      x.items_.begin(), i));
  h << proto << ";";

  std::string const implTrailingWhite(
    reconstructTrailingWhitespace(i,x.items_.end()));
  h << implTrailingWhite;
  
  std::vector<hcp_ast::IR>::const_iterator k(x.items_.begin());
  if ((*k)->isA<hcp_ast::FunctionQualifiers>()) {
    k=xju::next(k);
  }
  std::vector<hcp_ast::IR>::const_iterator j(
    std::find_if(k, x.items_.end(),
                 hcp_ast::isA_<hcp_ast::FunctionName>));
  xju::assert_not_equal(j, x.items_.end());

  c.copy((*k)->begin(), (*j)->begin());
  c << xju::format::join(scope.begin(),
                         scope.end(),
                         getClassName,
                         "::")
    << "::";
  auto const l(std::find_if(j,x.items_.end(),
                            hcp_ast::isA_<hcp_ast::FunctionImpl>));
  xju::assert_not_equal(l,x.items_.end());
  c.copyExcluding<hcp_ast::VarInitialiser,hcp_ast::VirtSpecifierSeq>(
    hcp_ast::IRs(j, l));
  c.copy((*l)->begin(), x.end());
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

bool isFriendOperatorLessDecl(hcp_ast::FunctionDecl const& x,
                              std::string const& className) throw()
{
  auto const params(hcp_ast::findChildrenOfType<hcp_ast::Param>(x));
  if (params.size()!=2){
    return false;
  }
  auto const filtered{
    hcp_ast::filter(
      params,
      [&](hcp_ast::Param const& p){
        if (hcp_ast::findChildrenOfType<hcp_ast::VarNonFp>(p).size()||
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
        else if (m->isA<hcp_ast::FunctionDecl>() &&
                 isFriendOperatorLessDecl(m->asA<hcp_ast::FunctionDecl>(),
                                          x.className_)){
          h.copy(m->begin(), m->end()); //REVISIT: generate less etc
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
  std::string proto(reconstructWithoutTrailingWhitespace(
                      x.items_.begin(), i));
  h << proto << ";";

  std::string const implTrailingWhite(
    reconstructTrailingWhitespace(i,x.items_.end()));
  h << implTrailingWhite;
  
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
  auto const y(
    hcp_ast::findOnlyChildOfType<hcp_ast::VarInitialiser>(x));
  h << "extern ";
  h.copy(x.begin(), y.begin());
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
  std::vector<std::string> const& x) throw(
    xju::Exception)
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
      root.items_.front()->asA<hcp_ast::File>().items_, oh, oc);
    
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
