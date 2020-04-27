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

int main(int argc, char* argv[])
{
  try {
    std::pair<CommandLineOptions, std::vector<std::string> > const cmd_line(
      parseCommandLine(std::vector<std::string>(argv+1, argv+argc)));
    
    if (cmd_line.second.size() < 3) {
      std::cerr << "usage: " << argv[0] 
                << " [-w] <input-file>"
      std::cerr << "-w write back to file instead of to stdout"
                << std::endl;
      std::cerr << "  replace explicit throw clauses with commented out"
                << "  list; replace empty throw clauses with noexcept"
                << std::endl;
      return 1;
    }

    std::pair<xju::path::AbsolutePath, xju::path::FileName> const inputFile(
      xju::path::split(cmd_line.second[0]));
    
    std::string const x(hcp::readFile(inputFile));

    auto const root{
      hcp_parser::parseString(x.begin(),x.end(), hcp_parser::file())};

    std::string const adjusted(replaceThrowClauses(
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
    
    std::ostringstream output;
    
    auto const throwLists(
      hcp_ast::findChildrenOfType<hcp_ast::ThrowList>(root));
    auto i(root.begin().x_);
    for(auto tl: throwLists){
      output << std::string(i,tl.begin().x_);
      auto const throwListItems(
        hcp_ast::findChildrenOfType<hcp_ast::ThrowListItem>(tl.get()));
      for(auto tli: throwListItems){
        auto const n(
          hcp_ast::findOnlyChildOfType<hcp_ast::ThrowListItemTypeName>(tli));
        output << std::string(tli.begin().x_,n.begin().x_);
        output << "//" << hcp_ast::reconstruct(n);
      }
      auto const tw(
        hcp_ast::findOnlyChildOfType<hcp_ast::ThrowListTrailingWhite>(tl));
      output << hcp_ast::reconstruct(tw);
      i=tl.end().x_;
    }
    os << std::string(i.x_,root.end().x_);
    if (cmd_line.writeBack_)
    {
      std::ofstream fc(xju::path::str(inputFile).c_str(), 
                       std::ios_base::out|std::ios_base::trunc);
      if (fc.fail()){
        std::ostringstream s;
        s << "open " << xju::path::str(inputFile) << " for write failed";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      fc << output.str();
      if (fc.fail()){
        std::ostringstream s;
        s << "write to " << xju::path::str(inputFile) << " failed";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
    }
    else{
      std::cout << output.str();
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
