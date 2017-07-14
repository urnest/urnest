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
#include <stdlib.h>
#include <xju/file/read.hh>
#include <sstream>
#include <utility>
#include <iostream>
#include <xju/stringToUInt.hh>
#include <cxy/ORB.hh>
#include "hcp/tags/Lookup.hh"
#include "hcp/tags/Lookup.cref.hh"
#include <xju/format.hh>
#include <xju/split.hh>

// get $TAGS_HPATH directories
std::vector<xju::path::AbsolutePath> getHPath() throw(xju::Exception)
{
  try {
    auto const x(::getenv("TAGS_HPATH"));
    if (x==0){
      throw xju::Exception("TAGS_HPATH environment variable is not set",XJU_TRACED);
    }
    std::vector<xju::path::AbsolutePath> result;
    for(auto d: xju::split(std::string(::getenv("TAGS_HPATH")),':')){
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

typedef std::vector<std::pair<xju::path::RelativePath,
                              xju::path::Extension> > ExtensionMappings;

struct Options {
  explicit Options(bool traceParsing) throw():
      traceParsing_(traceParsing) {
  }
  bool const traceParsing_;
};

// get tag lookup service - see tag-lookup-service.cc - URL from
// file
std::string getTagLookupServiceURL() throw(
  xju::Exception) {
  try {
    if (::getenv("TAG_LOOKUP_SERVICE_URL_FILE")==0) {
      throw xju::Exception("TAG_LOOKUP_SERVICE_URL_FILE environment variable is not set",XJU_TRACED);
    }
    xju::path::AbsFile const tagLookupServiceURLfile(
      xju::path::split(::getenv("TAG_LOOKUP_SERVICE_URL_FILE")));
    auto const url(xju::file::read(tagLookupServiceURLfile));
    return url;
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "get tag-lookup-service URL from file located by $TAG_LOOKUP_SERVICE_URL_FILE";
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

// get hcp extension mappings
ExtensionMappings getHcpExtensionMappings() throw(
  xju::Exception) {
  try {
    if (::getenv("HCP_EXTENSION_MAPPINGS")==0) {
      throw xju::Exception("HCP_EXTENSION_MAPPINGS environment variable is not set",XJU_TRACED);
    }
    try {
      ExtensionMappings result;
      for(auto const mapping: xju::split(
            std::string(::getenv("HCP_EXTENSION_MAPPINGS")),
            ':')) {
        auto const x(xju::split(mapping,'='));
        if (x.size()==0){
          std::ostringstream s;
          s << xju::format::quote(mapping)
            << " (from HCP_EXTENSION_MAPPINGS environment variable) "
            << "has no '=' (as in a/b/c=.xx)";
          throw xju::Exception(s.str(),XJU_TRACED);
        }
        else if (x.size()>2) {
          std::ostringstream s;
          s << xju::format::quote(mapping)
            << " (from HCP_EXTENSION_MAPPINGS environment variable) "
            << "has more than 1 '=' (as in a/b/c=.xx)";
          throw xju::Exception(s.str(),XJU_TRACED);
        }
        result.push_back(std::make_pair(xju::path::RelativePath(x[0]),
                                        xju::path::Extension(x[1])));
      }
      return result;
    }
    catch(xju::Exception& e) {
      std::ostringstream s;
      s << "parse $HCP_EXTENSION_MAPPINGS value "
        << xju::format::quote(::getenv("HCP_EXTENSION_MAPPINGS"));
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "get hcp extension mappings from $HCP_EXTENSION_MAPPINGS";
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}


// result.second are remaining arguments
std::pair<Options, std::vector<std::string> > parseCommandLine(
  std::vector<std::string> const& x) throw(
    xju::Exception)
{
  std::vector<std::string>::const_iterator i(x.begin());
  bool trace=false;
  ExtensionMappings extensionMappings;
  
  while((i != x.end()) && ((*i)[0]=='-')) {
    if ((*i)=="-t") {
      trace=true;
      ++i;
    }
    else {
      std::ostringstream s;
      s << "unknown option " << (*i)
        << " (only know -t)";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
  }
  return std::make_pair(
    Options(trace), 
    std::vector<std::string>(i, x.end()));
}

  
int main(int argc, char* argv[])
{
  try {
    std::pair<Options, std::vector<std::string> > const cmd_line(
      parseCommandLine(std::vector<std::string>(argv+1, argv+argc)));

    if (cmd_line.second.size() != 2) {
      std::cerr << "usage: " << argv[0] 
                << " [-t] [-m hcp-extension-mappings] <input-file> <offset>"
                << std::endl
                << "-t, trace parsing " << std::endl
                << std::endl;
      std::cerr << "note: $TAG_LOOKUP_SERVICE_URL_FILE must locate url-file of tag-lookup-service to use - see tag-lookup-service" << std::endl;
      std::cerr << "note: $TAGS_HPATH specifies search path for headers" << std::endl;
      std::cerr << "note: $HCP_EXTENSION_MAPPINGS is comma-separated extension mappings to apply where symbol is found in .hcp file, eg: " << std::endl
                << "  xju:.hh,app-aa/src:.h,:hpp" << std::endl
                << "... will map .hcp files included via xju/... to .hh and\n"
                << "    .hcp files includes via app-aa/src... to .h and all\n"
                << "    other non-absolute .hcp filenames to .hpp\n";
      std::cerr << std::endl
                << "Looks up, via Lookup service located by $TAG_LOOKUP_SERVICE_URL_FILE, the C++ symbol referred to at the specified offset within input file. Adds the appropriate include statement and prints adjusted file on stdout." << std::endl
                << "- returns 3 if symbol not found" << std::endl;
      return 1;
    }

    auto const hpath(getHPath());
    auto const hcpExtensionMappings(getHcpExtensionMappings());

    std::pair<xju::path::AbsolutePath, xju::path::FileName> const inputFile(
      xju::path::split(cmd_line.second[0]));

    size_t const offset(xju::stringToUInt(cmd_line.second[1]));
    
    Options const options(cmd_line.first);

    std::string const x(xju::file::read(inputFile));

    auto const url(getTagLookupServiceURL());
    cxy::ORB<xju::Exception> orb("giop:tcp::");
    cxy::cref<hcp::tags::Lookup> ref(orb,url);

    std::cout << hcp::tags::importSymbolAt(x, offset, *ref, hpath,
                                           hcpExtensionMappings,
                                           options.traceParsing_);
    
    return 0;
  }
  catch(hcp::tags::UnknownSymbol& e) {
    std::ostringstream s;
    s << xju::format::join(argv, argv+argc, " ");
    e.addContext(s.str(), XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
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
