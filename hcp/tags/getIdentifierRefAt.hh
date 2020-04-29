// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef HCP_TAGS_GETIDENTIFIERREFAT_H
#define HCP_TAGS_GETIDENTIFIERREFAT_H

#include "hcp/tags/IdentifierRef.hh"
#include "xju/Exception.hh"

namespace hcp
{
namespace tags
{
// get identifier reference at offset within text
// - offset might be at start, end or in the middle of an identifier ref or
//   outside an identifier ref
// - result includes any qualification
// post: result is a valid C++ identifier ref
IdentifierRef getIdentifierRefAt(std::string const& text,
                                 std::string::size_type const offset)
  /*throw(
    // pre: no identifier ref at offset
    xju::Exception)*/;

}
}

#endif
