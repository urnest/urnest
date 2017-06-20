// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "hcp/tags/getIdentifierRefAt.hh"

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

// result.second true means scope is "impl" scope
std::pair<std::vector<hcp::tags::NamespaceName,bool> > getScopeAtEnd(
  hcp_parser::IRs const& irs) throw()
{
  std::pair<std::vector<NamespaceName>,bool> result({},false);
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
    else if (ir->isA<hcp_ast::TemplateFunctionDef>())
    {
      return result;
    }
    else if (ir->isA<hcp_ast::AnonymousNamespace>()||
             ir->isA<hcp_ast::FunctionImpl>()
             //REVISIT: adjust parser to allow us to tell if we're
             //in the "def" part of an global or static var)
    {
      result.second=true;
      return result;
    }
  }
  return result;
}
bool isAbsolute(Identifier const& identifier) throw()
{
  return identifier.size()>=2 && identifier[0]==':' && identifier[1]==':';
}

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
      std::cout << "note: $TAG_LOOKUP_SERVICE_URI_FILE must locate uri-file of tag-lookup-service to use" << std::endl;
      return 1;
    }

    std::pair<xju::path::AbsolutePath, xju::path::FileName> const inputFile(
      xju::path::split(cmd_line.second[0]));

    size_t const offset(xju::stringToUInt(cmd_line.second[1]));
    
    std::string const x(xju::readFile(xju::path::str(inputFile)));

    Options const options(cmd_line.first);

    IdentifierRef const identifier(
      getIdentifierRefAt(x, offset, options.verbose_));

    hcp_parser::IRs const irsAtEnd(getIrsAtEnd(x,offset));

    std::pair<std::vector<NamespaceName>,bool> const scope(
      isAbsolute(identifier)?
      std::pair<std::vector<NamespaceName>,bool>({},false):
      getScopeAtEnd(irsAtEnd));
    
    // lookup in tag-lookup-service
    // ... what if not found?
    xju::path::AbsFile const tagLookupServiceURIfile(
      xju::path::split(::getenv("TAG_LOOKUP_SERVICE_URI_FILE")));
    auto const uri(xju::file::read(tagLookupServiceURIfile));
    cxy::ORB<xju::Exception> orb("giop:tcp::");
    cxy::cref<hcp::tags::Lookup> ref(orb,uri);

    hcp::tags::Locations const l(ref->lookupSymbol(fromScope,
                                                   symbolScope,
                                                   symbol));
    if (l.size()==0) {
      std::cout << "symbol " << identifier
                << " from scope " << fromScope
                << " not known to tag-lookup-service at "
                << uri
                << std::endl;
      return 3;
    }
    

    // irsAtEnd will include all #includes, so check those for
    // already mentioned

    
    // skip over initial comment block if any, then
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
