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
#include "xju/JoiningIterator.hh"
#include <hcp/getIrsAtEnd.hh>
#include <hcp/scopeAt.hh>

class Options
{
public:
  Options(bool verbose, 
          bool traceParsing) throw():
    verbose_(verbose),
    traceParsing_(traceParsing) {
  }
  bool verbose_;
  bool traceParsing_;
};

  
// result.second are remaining arguments
std::pair<Options, std::vector<std::string> > parseCommandLine(
  std::vector<std::string> const& x) /*throw(
    xju::Exception)*/
{
  std::vector<std::string>::const_iterator i(x.begin());
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
    Options(verbose,trace), 
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
      std::cout << "-t, trace parsing" << std::endl
                << "-v, verbose output" << std::endl
                << "\n";
      std::cout << "reports C++ scope at specified offset, either like:"
                << std::endl
                << "  header: x::y::z" << std::endl
                << "    ... if in \"header\" scope" << std::endl
                << "  impl: x::y::z" << std::endl
                << "    ... if in \"impl\" scope" << std::endl
                << "\n";
      return 1;
    }

    std::pair<xju::path::AbsolutePath, xju::path::FileName> const inputFile(
      xju::path::split(cmd_line.second[0]));

    size_t const offset(xju::stringToUInt(cmd_line.second[1]));
    
    std::string const x(hcp::readFile(inputFile));

    Options const options(cmd_line.first);

    hcp_parser::IRs const irsAtEnd(
      hcp::getIrsAtEnd(x,offset,options.traceParsing_));

    if (options.verbose_) {
      std::for_each(irsAtEnd.rbegin(),
                    irsAtEnd.rend(),
                    [](hcp_parser::IR const x){
                      std::cout << typeid(*x).name() << std::endl;
                    });
    }

    std::pair<std::vector<std::string>,bool> const scope(
      hcp::scopeAt(irsAtEnd));
    std::cout << (scope.second?"impl":"header") << " ::";
    std::copy(scope.first.begin(),
              scope.first.end(),
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
