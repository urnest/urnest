// Copyright (c) 2020 Trevor Taylor
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
  bool lisp(false);
  int i(1);
  if (argc > i && argv[i]==std::string("-e")){
    lisp=true;
    ++i;
  }
  if (argc-i != 2) {
    std::cerr << "usage: " << argv[0] << " [-e] <symbol> <from-scope>\n"
              << "e.g:\n  " << argv[0] << " impl::Fre mod1::c::Mule\n"
              << "... find completions of impl::Fre in the context "
              << "of mod1::c::Mule. Prints zero or more lines on stdout "
              << "each containing possible completion,e.g.:"
              << "  impl::Fred\n"
              << "  impl::Freda\n"
              << "... might be the output of the above example.\n"
              << "Or if -e specified, returns as an elisp (list) expression.\n"
              << "note: $TAG_LOOKUP_SERVICE_URL_FILE must locate url-file of tag-lookup-service to use - see tag-lookup-service" << std::endl;
    return 1;
  }
  try{
    auto const url(getTagLookupServiceURL());
    cxy::ORB<xju::Exception> orb("giop:tcp::");
    cxy::cref<hcp::tags::Lookup> ref(orb,url);

    int i(1);
    bool lisp(false);
    if (argc==4){
      lisp=true;
      ++i;
    }
    auto const ss(hcp::tags::splitSymbol(argv[i]));
    std::vector<hcp::tags::NamespaceName> fromScope(
      hcp::tags::splitScope(argv[i+1]));
    auto const completions(
      ref->lookupCompletions(fromScope,
                             ss.first,
                             ss.second));
    std::vector<std::string> result;
    for(auto c: completions){
      std::vector<std::string> cc;
      std::transform(c.scope_.begin(),c.scope_.end(),
                     std::back_inserter(cc),
                     xju::format::Str<decltype(c.scope_.front())>());
      cc.push_back(xju::format::str(c.name_));

      result.push_back(
        xju::format::quote(
          lisp?"\"":"",
          xju::format::join(cc.begin(),cc.end(),std::string("::"))));
    }
    if (lisp){
      std::cout << "(list "
                << xju::format::join(result.begin(),result.end(),
                                     std::string(" "))
                << ")" << std::endl;
    }
    else{
      for(auto r:result){
        std::cout << r << std::endl;
      }
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
