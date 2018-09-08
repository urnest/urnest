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
#include <ostream>
#include <vector>
#include <xju/Exception.hh>
#include <utility>
#include <string>
#include <sstream>
#include <xju/path.hh>
#include <hcp/readFile.hh>
#include <xju/format.hh>
#include "xju/functional.hh"
#include <utility>
#include <xju/stringToUInt.hh>
#include <hcp/getOptionValue.hh>
#include <map>
#include <typeinfo>

class Options
{
public:
  Options(size_t offset, 
          hcp_parser::Options const& parser_options) throw():
    offset_(offset),
    parser_options_(parser_options) {
  }
  size_t offset_;
  hcp_parser::Options parser_options_;
};

// result.second are remaining arguments
std::pair<Options, std::vector<std::string> > parseCommandLine(
  std::vector<std::string> const& x) throw(
    xju::Exception)
{
  std::vector<std::string>::const_iterator i(x.begin());
  size_t offset=0;
  bool trace=false;
  bool includeAllExceptionContext=false;
  
  while((i != x.end()) && ((*i)[0]=='-')) {
    if ((*i)=="-v") {
      includeAllExceptionContext=true;
      ++i;
    }
    else if ((*i)=="-t") {
      trace=true;
      ++i;
    }
    else if ((*i)=="-o") {
      ++i;
      offset=xju::stringToUInt(hcp::getOptionValue("-o", i, x.end()));
      ++i;
    }
    else {
      std::ostringstream s;
      s << "unknown option " << (*i)
        << " (only know -v, -t, -o)";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
  }
  return std::make_pair(
    Options(
      offset,
      hcp_parser::Options(trace,
                          hcp_parser::Cache(new hcp_parser::CacheVal()),
                          false)), 
    std::vector<std::string>(i, x.end()));
}

int main(int argc, char* argv[])
{
  try {
    std::pair<Options, std::vector<std::string> > const cmd_line(
      parseCommandLine(std::vector<std::string>(argv+1, argv+argc)));

    if (cmd_line.second.size() != 1) {
      std::cout << "usage: " << argv[0] 
                << " [-v] [-t] [-o <offset>] <input-file>" << std::endl;
      std::cout << "-t, trace " << std::endl
                << "-v, verbose" << std::endl
                << "-o <offset>, report what is at offset (default 0)"
                << "\n";
      return 1;
    }

    std::pair<xju::path::AbsolutePath, xju::path::FileName> const inputFile(
      xju::path::split(cmd_line.second[0]));
    
    std::string const x(hcp::readFile(inputFile));

    Options const options(cmd_line.first);

    auto const r{hcp_parser::parse(hcp_parser::I(x.begin(), x.end()),
                                   hcp_parser::file(), 
                                   options.parser_options_.trace_)};
    hcp_parser::I at(x.begin(), x.end());
    unsigned u;
    for(u=0; u != options.offset_; ++u, ++at);
    hcp_ast::CompositeItem root{r.first};
    std::vector<hcp_ast::CompositeItem const*> context(getContextAt(at, root));
    std::vector<hcp_ast::CompositeItem const*>::const_iterator j;
    for(j=context.begin(); j!=context.end(); ++j) {
      std::cout << (*j)->begin() << ": " << typeid(**j).name() << std::endl;
    }
    std::cout << at << std::endl;
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
