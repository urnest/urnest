#ifndef _READ_HCP
#define _READ_HCP
// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <utility>
#include <xju/path.hh>
#include "xju/Exception.hh"

namespace xju
{
namespace file
{
std::string read(
  std::pair<xju::path::AbsolutePath,xju::path::FileName> const& file) throw(
    xju::Exception);



}
}
#endif
