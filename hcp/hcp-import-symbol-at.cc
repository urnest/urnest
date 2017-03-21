// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

struct IdentifierTag{};
typedef xju::Tagged<std::string, IdentifierTag> Identifier;


// get identifier at offset within text
// - offset might be at start, end or in the middle of an identifier or
//   outside an identifier
// - result includes any qualification
// post: result is a valid C++ identifier
Identifier getIdentifierAt(std::string const& text, size_t const offset)
  throw(
    // pre: no identifier at offset
    xju::Exception)
{
}

hcp_parser::IRs getIrsAtEnd(std::string const& x, size_t offset) throw(
  xju::Exception)
{
  hcp_parser::ParseResult const r(
    hcp_parser::file->parse(
      hcp_parser::I(x.begin(), x.begin()+offset),
      options.parser_options_));
  
  if (r.failed()) {
    if (!r.e().atEnd()) {
      throw r.e();
    }
    return r.e().getIrsAtEnd();
  }
  return (*r).first;
}

std::vector<std::string> getScopeAtEnd(hcp_parser::IRs const& irs) throw()
{
  std::vector<std::string> scope;
  for(auto i=irsAtEnd.rbegin();i!=irsAtEnd.rend();++i) {
    hcp_parser::IR const ir(*i);
    if (ir->isA<hcp_ast::NamespaceName>()) {
      scope.push_back(hcp_ast::reconstruct(*ir));
    }
    else if (ir->isA<hcp_ast::ClassName>())
    {
      scope.push_back(hcp_ast::reconstruct(*ir));
    }
    else if (ir->isA<hcp_ast::EnumName>())
    {
      scope.push_back(hcp_ast::reconstruct(*ir));
    }
  }
  return scope;
}
bool isAbsolute(Identifier const& identifier) throw()
{
  return identifier.size()>=2 && identifier[0]==':' && identifier[1]==':';
}

struct DepthTag{};
typedef xju::Int<DepthTag,unsigned int> Depth;
struct LocationTag{};
typedef xju::Tagged<std::string,LocationTag> Location;
  
int main(int argc, char* argv[])
{
  try {
    std::pair<Options, std::vector<std::string> > const cmd_line(
      parseCommandLine(std::vector<std::string>(argv+1, argv+argc)));

    if (cmd_line.second.size() != 2) {
      std::cout << "usage: " << argv[0] 
                << " [-v] [-t] <input-file> <offset>" << std::endl;
      std::cout << "-t, trace " << std::endl
                << "-v, verbose" << std::endl
                << "\n";
      return 1;
    }

    std::pair<xju::path::AbsolutePath, xju::path::FileName> const inputFile(
      xju::path::split(cmd_line.second[0]));

    size_t const offset(xju::stringToUInt(cmd_line.second[1]));
    
    std::string const x(xju::readFile(xju::path::str(inputFile)));

    Options const options(cmd_line.first);

    Identifier identifier(getIdentifierAt(x, offset, options.verbose_));

    hcp_parser::IRs const irsAtEnd(getIrsAtEnd(x,offset));

    std::vector<std::string> const scope(
      isAbsolute(identifier)?
      std::vector<std::string> :
      getScopeAtEnd(irsAtEnd));
    }
    
    std::ostringstream fqs;
    fqs << "::";
    std::copy(scope.begin(),
              scope.end(),
              xju::JoiningIterator<std::string,std::string>(fqs,"::"));

    std::optional<std::pair<Depth, Location> > deepestMatch;

    std::string const HCP_TAGS_FILES(::getenv(HCP_TAGS_FILES));
    for(auto i=HCP_TAGS_FILES.begin(), auto j=HCP_TAGS_FILES.begin();
        i != HCP_TAGS_FILES.end();
        j=i,i=std::find(i,HCP_TAGS_FILES.end(),':')) {
      std::string const fileName(j,i);
      std::pair<xju::path::AbsolutePath, xju::path::FileName> const tagsFile(
        xju::path::split(fileName));

      while(more) {
        Tag t(nextTag());
        if (t.endsWith(identifier)) {
          auto m(match(t-identifier,scope));
          if (m.first && (!deepestMatch.valid()||
                          m.second>deepestMatch.value().first)) {
            deepestMatch=std::make_pair(m.second,makeRelative(nextLocation(),
                                                              tagsFile));
          }
        }
      }
    }
    if (!deepestMatch.valid()) {
      std::ostringstream s;
      s << "no definition of " << identifier << " found for scope "
        << scope;
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    
            
    // irsAtEnd will include all #includes, so check those for
    // already mentioned

    // figure out whether offset is in "impl" scope:
    //   anon-namespace
    //   non-template, non-inline function def
    //   static var def
    //   ... check hcp-split for full list
    
    // otherwise, skip over initial comment block if any, then
    // insert #include <location> and add //impl if in "impl" scope
    // write updated text to fileName

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
