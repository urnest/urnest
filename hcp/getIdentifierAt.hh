// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef HCP_GETIDENTIFIERAT_H
#define HCP_GETIDENTIFIERAT_H

#include "hcp/Identifier.hh"
#include "xju/Exception.hh"

namespace hcp
{

// get identifier at offset within text
// - offset might be at start, end or in the middle of an identifier or
//   outside an identifier
// - result includes any qualification
// post: result is a valid C++ identifier
Identifier getIdentifierAt(std::string const& text,
                           std::string::size_type const offset)
  throw(
    // pre: no identifier at offset
    xju::Exception);



}

#endif

