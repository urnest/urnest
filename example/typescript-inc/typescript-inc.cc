// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <string>
#include <sstream>
#include <iostream>
#include <hcp/ast.hh>
#include <hcp/parser.hh>
#include <xju/file/read.hh>
#include <example/typescript-inc/parsers.hh>

std::string usage(std::string program){
  std::ostringstream s;
  s << "usage: " << program << " <file>\n"
    << "  print typescript import files of argv[1] one per line on stdout";
  return s.str();
}

using namespace example::typescript_inc;

int main(int argc, char* argv[])
{
  if (argc != 2){
    std::cerr << usage(argv[0]) << std::endl;
    return 1;
  }
  
  auto const s(xju::file::read(xju::path::split(argv[1])));
  auto const cst(parseString(s.begin(), s.end(),
                             parseUntil(hcp_parser::endOfFile()+
                                        import()|statementOrDef())));
  for(auto const imported: hcp_ast::findChildrenOfType<Imported>(cst)){
    std::cout << reconstruct(imported) << std::endl;
  }
  return 0;
}
