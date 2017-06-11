#ifndef _XJU_FILE_WRITE_HH
#define _XJU_FILE_WRITE_HH
// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/path.hh>
#include "xju/Exception.hh"

namespace xju
{
namespace file
{

// write content to file, creating file with specified mode if necessary
// - replaces existing content
// - actual mode is mode & ~umask & 0777 - see open manpage
void write(
  std::pair<xju::path::AbsolutePath,xju::path::FileName> const& file,
  void const* content,
  size_t const contentSize,
  mode_t const mode) throw(
    xju::Exception);


void write(
  std::pair<xju::path::AbsolutePath,xju::path::FileName> const& file,
  std::string const& content,
  mode_t const mode) throw(
    xju::Exception);


}
}
#endif
