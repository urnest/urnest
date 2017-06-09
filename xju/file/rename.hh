#ifndef _RENAME_HCP
#define _RENAME_HCP
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

#include "xju/stdio.hh"

namespace xju
{
namespace file
{
// rename file to newName
void rename(
  std::pair<xju::path::AbsolutePath,xju::path::FileName> const& file,
  std::pair<xju::path::AbsolutePath,xju::path::FileName> const& newName) throw(
    xju::Exception);


// rename directory to newName
void rename(
  std::pair<xju::path::AbsolutePath,xju::path::DirName> const& directory,
  std::pair<xju::path::AbsolutePath,xju::path::DirName> const& newName) throw(
    xju::Exception);


}
}
#endif
