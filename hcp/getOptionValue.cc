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
#include <hcp/getOptionValue.hh>

#include <sstream>

namespace hcp
{

std::string getOptionValue(std::string const& option,
                           std::vector<std::string>::const_iterator const i,
                           std::vector<std::string>::const_iterator const end)
  /*throw(xju::Exception)*/
{
  if (i==end) {
    std::ostringstream s;
    s << option << " requires an argument";
    throw xju::Exception(s.str(), XJU_TRACED);
  }
  return *i;
}

}
