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
#ifndef XJU_READFILE_HH_
#define XJU_READFILE_HH_

#include <string>
#include "xju/Exception.hh"

namespace xju
{
std::string readFile(std::string fileName) throw(
  xju::Exception);
}

#endif
