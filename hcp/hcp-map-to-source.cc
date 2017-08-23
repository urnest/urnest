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

struct MapHcpTag{};
typedef hcp_ast::TaggedCompositeItem<MapHcpTag> MapHcp;
struct MapFromLineNumberTag{};
typedef hcp_ast::TaggedCompositeItem<MapFromLineNumberTag> MapFromLineNumber;
struct MapToLineNumberTag{};
typedef hcp_ast::TaggedCompositeItem<MapToLineNumberTag> MapToLineNumber;
struct MapLinePairTag{};
typedef hcp_ast::TaggedCompositeItem<MapLinePairTag> MapLinePair;

int main(int argc, char* argv[]) {
  if (argc!=3){
    std::cerr << "usage: " << argv[0] << " <file> <line>\n"
              << "  - assumes <file>.smap is hmap- or smap-file\n"
              << "    generated from .hcp eg by hcp-split\n"
              << "  - reports source file and line corresponding to \n"
              << "    line <line> in <file>, according to smap" << std::endl;
    return 1;
  }
  try{
    std::string fileName(argv[1]);
    unsigned int lineNumber(xju::stringToUInt(argv[2]));
    std::pair<xju::path::AbsolutePath,xju::path::FileName> const f(
      xju::path::split(fileName+".smap"));
    std::string const map(xju::file::read(f));

    auto parseLineNumber=hcp_parser::atLeastOne(
      hcp_parser::charInRange('0','9'));

    auto mapParser=hcp_parser::PR(new hcp_parser::NamedParser<MapHcp>(
      "hcp file name",
      hcp_parser::parseUntil(hcp_parser::parseLiteral("\n"))))+
      hcp_parser::parseLiteral("\n")+
      hcp_parser::zeroOrMore()*hcp_parser::PR(
        new hcp_parser::NamedParser<MapLinePair>(
          "line number pair",
          hcp_parser::PR(new hcp_parser::NamedParser<MapFromLineNumber>(
                           "from line number",
                           parseLineNumber))+
          hcp_parser::atLeastOne(hcp_parser::whitespaceChar())+
          hcp_parser::PR(new hcp_parser::NamedParser<MapToLineNumber>(
                           "to line number",
                           parseLineNumber))+
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
    std::map<unsigned int, unsigned int> lineMap;
    lineMap.insert(std::make_pair(0U,0U));
    auto const linePairs(
      hcp_ast::findChildrenOfType<MapLinePair>(root));
    try{
      for(auto p: linePairs){
        lineMap.insert(
          std::make_pair(
            xju::stringToUInt(
              hcp_ast::reconstruct(
                hcp_ast::findOnlyChildOfType<MapFromLineNumber>(p))),
            xju::stringToUInt(
              hcp_ast::reconstruct(
                hcp_ast::findOnlyChildOfType<MapToLineNumber>(p)))));
      }
    }
    catch(xju::Exception& e){
      std::ostringstream s;
      s << "read line pair " << lineMap.size();
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
    auto i(lineMap.upper_bound(lineNumber));
    xju::assert_not_equal(i,lineMap.begin());
    --i;
    std::cout << hcp << " " << (lineNumber-(*i).first+(*i).second) << "\n";
    return 0;
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << "map file " << argv[1] << " line " << argv[2]
      << " its .hcp source file and line";
    e.addContext(s.str(),XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 2;
  }
}
