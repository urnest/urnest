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

class Options
{
public:
  Options(size_t offset, 
          std::string const& target, 
          bool const dump,
          hcp_parser::Options const& parser_options) throw():
    offset_(offset),
    target_(target),
    dump_(dump),
    parser_options_(parser_options) {
  }
  size_t offset_;
  std::string target_;
  bool dump_;
  hcp_parser::Options parser_options_;
};

// result.second are remaining arguments
std::pair<Options, std::vector<std::string> > parseCommandLine(
  std::vector<std::string> const& x) throw(
    xju::Exception)
{
  std::vector<std::string>::const_iterator i(x.begin());
  size_t offset=0;
  std::string target="file";
  bool trace=false;
  bool includeAllExceptionContext=false;
  bool dump=false;
  
  while((i != x.end()) && ((*i)[0]=='-')) {
    if ((*i)=="-v") {
      includeAllExceptionContext=true;
      ++i;
    }
    else if ((*i)=="-t") {
      trace=true;
      ++i;
    }
    else if ((*i)=="-d") {
      dump=true;
      ++i;
    }
    else if ((*i)=="-o") {
      ++i;
      offset=xju::stringToUInt(hcp::getOptionValue("-o", i, x.end()));
      ++i;
    }
    else if ((*i)=="-p") {
      ++i;
      target=hcp::getOptionValue("-p", i, x.end());
      ++i;
    }
    else {
      std::ostringstream s;
      s << "unknown option " << (*i)
        << " (only know -v, -t, -o, -p)";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
  }
  return std::make_pair(
    Options(
      offset,
      target,
      dump,
      hcp_parser::Options(trace,
                          hcp_parser::Cache(new hcp_parser::CacheVal()))), 
    std::vector<std::string>(i, x.end()));
}

int main(int argc, char* argv[])
{
  try {
    std::pair<Options, std::vector<std::string> > const cmd_line(
      parseCommandLine(std::vector<std::string>(argv+1, argv+argc)));

    std::map<std::string, hcp_parser::PR> parsers;
    parsers.insert(std::make_pair("string literal", hcp_parser::stringLiteral));
    parsers.insert(std::make_pair("comments", hcp_parser::comments));
    parsers.insert(std::make_pair("typedef", hcp_parser::typedef_statement));
    parsers.insert(std::make_pair("using", hcp_parser::using_statement));
    parsers.insert(std::make_pair("enum definition", hcp_parser::enum_def));
    parsers.insert(std::make_pair("function decl", hcp_parser::function_decl));
    parsers.insert(std::make_pair("template function def", hcp_parser::template_function_def));
    parsers.insert(std::make_pair("function def", hcp_parser::function_def));
    parsers.insert(std::make_pair("attr decl", hcp_parser::attr_decl));
    parsers.insert(std::make_pair("class decl", hcp_parser::class_decl));
    parsers.insert(std::make_pair("class def", hcp_parser::class_def));
    parsers.insert(std::make_pair("anonymous namespace", hcp_parser::anonymous_namespace));
    parsers.insert(std::make_pair("namespace def", hcp_parser::namespace_def));
    parsers.insert(std::make_pair("file", hcp_parser::file));

    if (cmd_line.second.size() != 1) {
      std::cout << "usage: " << argv[0] 
                << " [-v] [-t] [-o <offset>] [-p <type>] [-d] <input-file>" 
                << std::endl;
      std::cout << "-t, trace " << std::endl
                << "-v, verbose" << std::endl
                << "-o <offset>, start parsing at offset (default 0)\n"
                << "-d, dump parsed item(s)"
                << "-p <type>, attempt to parse <type>, one of:\n"
                << xju::format::join(parsers.begin(), parsers.end(),
                                     xju::functional::first,
                                     ", ") << "\n";
      return 1;
    }

    std::pair<xju::path::AbsolutePath, xju::path::FileName> const inputFile(
      xju::path::split(cmd_line.second[0]));
    
    std::string const x(xju::readFile(xju::path::str(inputFile)));

    Options const options(cmd_line.first);

    std::map<std::string, hcp_parser::PR>::iterator const i(
      parsers.find(options.target_));
    if (i == parsers.end()) {
      std::ostringstream s;
      s << "unknown target type " << options.target_
        << " (only know "
        << xju::format::join(parsers.begin(), parsers.end(),
                             xju::functional::first,
                             ", ")
        << ")";
      throw xju::Exception(s.str(), XJU_TRACED);
    }

    hcp_parser::I at(x.begin(), x.end());
    size_t u;
    for(u=0; u != options.offset_; ++u, ++at);
    hcp_ast::CompositeItem root;
    at = hcp_parser::parse(root, at, (*i).second, 
                           options.parser_options_.trace_);
    if (options.dump_) {
      std::cout << root << std::endl;
    }
    std::cout << "end at " << at << std::endl;
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << xju::format::join(argv, argv+argc, " ");
    e.addContext(s.str(), XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 2;
  }
}
