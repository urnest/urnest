// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <string>
#include <xju/Exception.hh>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <cxy/ORB.hh>
#include <xju/path.hh>
#include <utility>
#include <xju/format.hh>
#include "hcp/tags/Lookup.hh"
#include "hcp/tags/Lookup.cref.hh"
#include <xju/file/read.hh>
#include <xju/split.hh>
#include <hcp/tags/splitSymbol.hh>
#include <hcp/tags/splitScope.hh>


// get tag lookup service - see tag-lookup-service.cc - URL from
// file
std::string getTagLookupServiceURL() /*throw(
  xju::Exception)*/
{
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

int main(int argc, char* argv[])
{
  if (argc != 3){
    std::cerr << "usage: " << argv[0] << " <symbol> <from-scope>\n"
              << "e.g:\n  " << argv[0] << " impl::Fred mod1::c::Mule\n"
              << "... find definitions of impl::Fred in the context "
              << "of mod1::c::Mule. Prints zero or more lines on stdout "
              << "each containing <file> <line> of possible definition,e.g.:"
              << "  /x/impl/Fred.hh 33\n"
              << "  /y/mod1/impl/Fred 82\n"
              << "... might be the output of the above example.\n"
              << "note: $TAG_LOOKUP_SERVICE_URL_FILE must locate url-file of tag-lookup-service to use - see tag-lookup-service" << std::endl;
    return 1;
  }
  try{
    auto const url(getTagLookupServiceURL());
    cxy::ORB<xju::Exception> orb("giop:tcp::");
    cxy::cref<hcp::tags::Lookup> ref(orb,url);

    auto const ss(hcp::tags::splitSymbol(argv[1]));
    std::vector<hcp::tags::NamespaceName> fromScope(
      hcp::tags::splitScope(argv[2]));
    auto const foundIn(
      ref->lookupSymbol(fromScope,
                        ss.first,
                        ss.second));
    for(auto const l: foundIn.locations_){
      std::cout << xju::path::str(std::make_pair(l.directory,l.file))
                << " " << l.line << std::endl;
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














