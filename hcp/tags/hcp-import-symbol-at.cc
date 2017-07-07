// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/tags/importSymbolAt.hh>
#include <hcp/scopeAt.hh>
#include <vector>
#include "hcp/tags/AbsolutePath.hh"
#include "xju/Exception.hh"

// get $TAGS_HPATH directories
std::vector<xju::path::AbsolutePath> getHPath() throw(xju::Exception)
{
  try {
    auto const x(::getenv("TAGS_HPATH"));
    if (x==0){
      throw xju::Exception("TAGS_HPATH environment variable is not set",XJU_TRACED);
    }
    std::vector<xju::path::AbsFile> result;
    for(auto d: xju::split(::getenv("TAGS_HPATH"),':')){
      result.push_back(xju::path::split(d+"/.").first);
    }
    return result;
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "get $TAGS_HPATH directories";
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

  
int main(int argc, char* argv[])
{
  try {
    std::pair<Options, std::vector<std::string> > const cmd_line(
      parseCommandLine(std::vector<std::string>(argv+1, argv+argc)));

    if (cmd_line.second.size() != 2) {
      std::cerr << "usage: " << argv[0] 
                << " [-v] [-t] <input-file> <offset>" << std::endl;
      std::cerr << "-t, trace " << std::endl
                << "-v, verbose" << std::endl
                << "\n";
      std::cerr << "note: $TAG_LOOKUP_SERVICE_URL_FILE must locate url-file of tag-lookup-service to use - see tag-lookup-service" << std::endl;
      return 1;
    }

    std::pair<xju::path::AbsolutePath, xju::path::FileName> const inputFile(
      xju::path::split(cmd_line.second[0]));

    size_t const offset(xju::stringToUInt(cmd_line.second[1]));
    
    Options const options(cmd_line.first);

    std::string const x(xju::file::read(xju::path::str(inputFile)));

    xju::path::AbsFile const tagLookupServiceURLfile(
      xju::path::split(::getenv("TAG_LOOKUP_SERVICE_URL_FILE")));
    auto const url(xju::file::read(tagLookupServiceURLfile));
    cxy::ORB<xju::Exception> orb("giop:tcp::");
    cxy::cref<hcp::tags::Lookup> ref(orb,url);

    std::cout << hcp::tags::importSymbolAt(x, offset, *ref);
    
    return 0;
  }
  catch(hcp::tags::UnknownSymbol& e) {
    std::ostringstream s;
    s << xju::format::join(argv, argv+argc, " ");
    e.addContext(s.str(), XJU_TRACED);
    std::cerr << readableRepr(e);
    return 3;
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << xju::format::join(argv, argv+argc, " ");
    e.addContext(s.str(), XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 2;
  }
}
