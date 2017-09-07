// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <hcp/ast.hh>
#include <hcp/parser.hh>
#include <iostream>
#include <string>
#include <xju/stringToUInt.hh>
#include <xju/path.hh>
#include "xju/startsWith.hh"
#include <xju/file/read.hh>
#include <utility>
#include <map>
#include <sstream>
#include <algorithm>
#include "xju/assert.hh"
#include <xju/file/stat.hh>
#include <xju/split.hh>

struct MapHcpTag{};
typedef hcp_ast::TaggedCompositeItem<MapHcpTag> MapHcp;
struct MapFromOffsetTag{};
typedef hcp_ast::TaggedCompositeItem<MapFromOffsetTag> MapFromOffset;
struct MapToOffsetTag{};
typedef hcp_ast::TaggedCompositeItem<MapToOffsetTag> MapToOffset;
struct MapOffsetPairTag{};
typedef hcp_ast::TaggedCompositeItem<MapOffsetPairTag> MapOffsetPair;

// get $HCP_HPATH directories
std::vector<xju::path::AbsolutePath> getHPath() throw(xju::Exception)
{
  try {
    auto const x(::getenv("HCP_HPATH"));
    if (x==0){
      throw xju::Exception("HCP_HPATH environment variable is not set",XJU_TRACED);
    }
    std::vector<xju::path::AbsolutePath> result;
    for(auto d: xju::split(std::string(::getenv("HCP_HPATH")),':')){
      result.push_back(xju::path::split(d+"/.").first);
    }
    return result;
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "get $HCP_HPATH directories";
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

struct GeneratedAsTag{};
typedef hcp_ast::TaggedCompositeItem<GeneratedAsTag> GeneratedAs;

std::pair<xju::path::AbsolutePath,xju::path::FileName> const findSmap(
  std::string const& fileName) throw(
    xju::Exception)
{
  try{
    try{
      std::pair<xju::path::AbsolutePath,xju::path::FileName> const f(
        xju::path::split(fileName));
      
      auto const p(hcp_parser::parseLiteral("// hcp-split: generated as ")+
                   new hcp_parser::NamedParser<GeneratedAs>(
                     "generated as",
                     hcp_parser::parseUntil(hcp_parser::whitespaceChar())));
      hcp_ast::CompositeItem root;
      std::string const fileContent(xju::file::read(f));
      hcp_parser::parse(root,
                        hcp_parser::I(fileContent.begin(),fileContent.end()),
                        hcp_parser::parseUntil(p)+p);
      auto const generatedAs(
        hcp_ast::reconstruct(
          hcp_ast::findOnlyChildOfType<GeneratedAs>(root)));
      if (xju::startsWith(generatedAs,std::string("/"))){
        return xju::path::split(generatedAs);
      }
      auto const hpath(getHPath());
      try{
        std::vector<std::string> failures;
        for(auto const path: hpath){
          auto const candidate(
            xju::path::split(xju::path::str(path)+"/"+generatedAs+".smap"));
          try{
            xju::file::stat(candidate);
            return candidate;
          }
          catch(xju::Exception const& e){
            failures.push_back(readableRepr(e));
          }
        }
        throw xju::Exception(
          xju::format::join(failures.begin(),failures.end(), " and "),
          XJU_TRACED);
      }
      catch(xju::Exception& e){
        std::ostringstream s;
        s << "read first " << generatedAs
          << " found on path specified by HCP_HPATH environment variable";
        e.addContext(s.str(),XJU_TRACED);
        throw;
      }
    }
    catch(xju::Exception& e){
      try{
        auto const candidate(xju::path::split(fileName+".smap"));
        xju::file::stat(candidate);
        return candidate;
      }
      catch(xju::Exception& ee){
        throw xju::Exception(
          readableRepr(ee)+" and "+readableRepr(e),XJU_TRACED);
        throw;
      }
    }
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "find hcp .smap file of " << fileName;
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

int main(int argc, char* argv[]) {
  if (argc!=3){
    std::cerr << "usage: " << argv[0] << " <file> <offset>\n"
              << "  - assumes <file>.smap is hmap- or smap-file\n"
              << "    generated from .hcp eg by hcp-split\n"
              << "  - reports source file and offset corresponding to \n"
              << "    offset <offset> in <file>, according to smap\n"
              << "  - where <file> is specified without a path, seaches for"
              << "    <rel>/<file>.smap on HCP_HPATH where <rel> is the "
              << "    path specified via -hpath option to hcp-split when "
              << "    <file> was generated, assuming -G was specified"
              << std::endl;
    return 1;
  }
  try{
    std::string fileName(argv[1]);
    unsigned int offset(xju::stringToUInt(argv[2]));
    std::pair<xju::path::AbsolutePath,xju::path::FileName> const f(
      findSmap(fileName));
    std::string const map(xju::file::read(f));

    auto parseOffset=hcp_parser::atLeastOne(
      hcp_parser::charInRange('0','9'));

    auto mapParser=hcp_parser::PR(new hcp_parser::NamedParser<MapHcp>(
      "hcp file name",
      hcp_parser::parseUntil(hcp_parser::parseLiteral("\n"))))+
      hcp_parser::parseLiteral("\n")+
      hcp_parser::zeroOrMore()*hcp_parser::PR(
        new hcp_parser::NamedParser<MapOffsetPair>(
          "offset pair",
          hcp_parser::PR(new hcp_parser::NamedParser<MapFromOffset>(
                           "from offset",
                           parseOffset))+
          hcp_parser::atLeastOne(hcp_parser::whitespaceChar())+
          hcp_parser::PR(new hcp_parser::NamedParser<MapToOffset>(
                           "to offset",
                           parseOffset))+
          hcp_parser::parseLiteral("\n")));
    hcp_ast::CompositeItem root{};
    hcp_ast::I begin(map.begin(),map.end());
    hcp_ast::I end(hcp_parser::parse(root,begin,mapParser));
    if (!end.atEnd()){
      std::ostringstream s;
      s << "parsing of map did not end at end of file, it ended at "
        << (*end) << " (offset " << (end-begin) << ")";
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    std::string const hcp(
      hcp_ast::reconstruct(
        hcp_ast::findOnlyChildOfType<MapHcp>(root)));
    std::map<off_t,off_t> offsetMap;
    offsetMap.insert(std::make_pair(-1,-1));
    auto const offsetPairs(
      hcp_ast::findChildrenOfType<MapOffsetPair>(root));
    try{
      for(auto p: offsetPairs){
        offsetMap.insert(
          std::make_pair(
            xju::stringToUInt(
              hcp_ast::reconstruct(
                hcp_ast::findOnlyChildOfType<MapFromOffset>(p))),
            xju::stringToUInt(
              hcp_ast::reconstruct(
                hcp_ast::findOnlyChildOfType<MapToOffset>(p)))));
      }
    }
    catch(xju::Exception& e){
      std::ostringstream s;
      s << "read offset pair " << offsetMap.size();
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
    auto i(offsetMap.upper_bound(offset));
    xju::assert_not_equal(i,offsetMap.begin());
    --i;
    std::cout << hcp << " " << (offset-(*i).first+(*i).second) << "\n";
    return 0;
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "map file " << argv[1] << " offset " << argv[2]
      << " its .hcp source file and offset";
    e.addContext(s.str(),XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 2;
  }
}
