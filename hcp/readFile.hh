// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef HCP_READFILE_HH
#define HCP_READFILE_HH

#include <utility>
#include <xju/path.hh>
#include "xju/Exception.hh"

namespace hcp
{
std::string readFile(
  std::pair<xju::path::AbsolutePath,xju::path::FileName> const& file) /*throw(
    xju::Exception)*/;

}

#endif
