// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <utility>
#include <vector>
#include "xju/path.hh"
#include <string>
#include "xju/Exception.hh"
#include <hcp/getOptionValue.hh>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "xju/format.hh"
#include <cxy/ORB.hh>
#include "hcp/tags/TagLookupService.hh"
#include "hcp/tags/Lookup.hh"
#include "hcp/tags/Lookup.sref.hh"
#include <xju/file/write.hh>
#include <xju/file/rename.hh>
#include <xju/format.hh>
#include "xju/Optional.hh"

char const usage[]="-u url-file tags-file [tags-file...]\n"
  "  provides hcp::tags::Lookup interface - see Lookup.idl - to tags files, via CORBA url written to url-file";

// result.first is uri-file
// result.second are remaining arguments
std::pair<xju::path::AbsFile, std::vector<xju::path::AbsFile> > parseCommandLine(
  std::vector<std::string> const& x) throw(
    xju::Exception)
{
  std::vector<std::string>::const_iterator i(x.begin());
  xju::Optional<std::string> uriFile;
  std::vector<xju::path::AbsFile> tagsFiles;
  
  while((i != x.end()) && ((*i)[0]=='-')) {
    if ((*i)=="-u") {
      ++i;
      uriFile=hcp::getOptionValue("-u", i, x.end());
      ++i;
    }
    else {
      std::ostringstream s;
      s << "unknown option " << (*i)
        << " (only know -u)";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
  }
  if (!uriFile.valid()) {
    std::ostringstream s;
    s << "-u option is mandatory";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  if (i==x.end()) {
    std::ostringstream s;
    s << "specify at least one tags file";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  std::transform(i,x.end(),
                 std::back_inserter(tagsFiles),
                 [](std::string const& tagsFile) {
                   return xju::path::split(tagsFile);
                 });
  return std::make_pair(xju::path::split(uriFile.value()), tagsFiles);
}


int main(int argc, char* argv[])
{
  try {
    auto const args(
      parseCommandLine(std::vector<std::string>(argv+1,argv+argc)));
    xju::path::AbsFile const uriFile(args.first);
    for(int port=8723; port!=9723; ++port) {
      try {
        std::string const orbEndPoint="giop:tcp:localhost:"+
          xju::format::str(port);
        cxy::ORB<xju::Exception> orb(orbEndPoint);
        hcp::tags::TagLookupService s(args.second);
        cxy::sref<hcp::tags::Lookup> sref(orb,"TagLookupService",s);
        auto const tmpFile(xju::path::split(xju::path::str(uriFile)+".new"));
        xju::file::write(tmpFile,
                         "corbaloc:iiop:localhost:"+xju::format::str(port)+
                         "/TagLookupService",
                         0666);
        xju::file::rename(tmpFile,uriFile);
        s.run();
        return 0;
      }
      catch(cxy::PortInUse&) {
      }
    }
    std::ostringstream s;
    s << "apparently none of tcp ports 8723..9723 were available";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  catch(xju::Exception& e) {
    e.addContext(xju::format::join(argv,argv+argc," "),XJU_TRACED);
    std::cerr << "ERROR: " << readableRepr(e) << std::endl;
    return 1;
  }
}
