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
#include <xju/readFile.hh>
#include <xju/format.hh>
#include "xju/functional.hh"
#include <utility>
#include <xju/stringToUInt.hh>
#include <hcp/getOptionValue.hh>
#include <map>
#include <typeinfo>
#include "xju/JoiningIterator.hh"

class Options
{
public:
  Options(bool verbose, 
          hcp_parser::Options const& parser_options) throw():
    verbose_(verbose),
    parser_options_(parser_options) {
  }
  bool verbose_;
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
  bool verbose=false;
  
  while((i != x.end()) && ((*i)[0]=='-')) {
    if ((*i)=="-v") {
      verbose=true;
      ++i;
    }
    else if ((*i)=="-t") {
      trace=true;
      ++i;
    }
    else {
      std::ostringstream s;
      s << "unknown option " << (*i)
        << " (only know -v, -t)";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
  }
  return std::make_pair(
    Options(verbose,
            hcp_parser::Options(trace,
                                hcp_parser::Cache(new hcp_parser::CacheVal()),
                                true)), 
    std::vector<std::string>(i, x.end()));
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
      return 1;
    }

    std::pair<xju::path::AbsolutePath, xju::path::FileName> const inputFile(
      xju::path::split(cmd_line.second[0]));

    size_t const offset(xju::stringToUInt(cmd_line.second[1]));
    
    std::string const x(xju::readFile(xju::path::str(inputFile)));

    Options const options(cmd_line.first);

    hcp_parser::ParseResult const r(
      hcp_parser::file()->parse(
        hcp_parser::I(x.begin(), x.begin()+offset),
        options.parser_options_));
    std::vector<std::string> scope;
    if (r.failed()) {
      if (!r.e().atEnd()) {
        throw r.e();
      }
      hcp_parser::IRs const irsAtEnd(
        r.e().getIrsAtEnd());
      for(auto i=irsAtEnd.rbegin();i!=irsAtEnd.rend();++i) {
        hcp_parser::IR const ir(*i);
        if (options.verbose_) {
          std::cerr << typeid(*ir).name() << " "
                    << xju::format::quote(
                      xju::format::cEscapeString(
                        hcp_ast::reconstruct(*ir)))
                    << std::endl;
        }
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
      }
    }
    std::cout << "::";
    std::copy(scope.begin(),
              scope.end(),
              xju::JoiningIterator<std::string,std::string>(std::cout,"::"));
    std::cout << std::endl;
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
