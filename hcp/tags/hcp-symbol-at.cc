// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <vector>
#include <iostream>
#include <utility>
#include <sstream>
#include <xju/path.hh>
#include <xju/stringToUInt.hh>
#include <xju/file/read.hh>
#include <hcp/tags/getIdentifierRefAt.hh>
#include <xju/format.hh>

int main(int argc, char* argv[])
{
  try {
    std::vector<std::string> const cmd_line{argv+1,argv+argc};
    if (cmd_line.size() != 2) {
      std::cerr << "usage: " << argv[0] 
                << " <input-file> <offset>"
                << std::endl;
      std::cerr << std::endl
                << "Prints the C++ symbol referred to at the specified offset within input file." << std::endl;
      return 1;
    }

    std::pair<xju::path::AbsolutePath, xju::path::FileName> const inputFile(
      xju::path::split(cmd_line[0]));

    std::string const x(xju::file::read(inputFile));

    size_t const offset(xju::stringToUInt(cmd_line[1]));
    
    hcp::tags::IdentifierRef const identifier(
      hcp::tags::getIdentifierRefAt(x, offset));
    std::cout << identifier << std::endl;
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
