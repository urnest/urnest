// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef HCP_IDENTIFIER_H
#define HCP_IDENTIFIER_H

#include "xju/Tagged.hh"
#include <string>

namespace hcp
{
struct IdentifierTag{};
typedef xju::Tagged<std::string, IdentifierTag> Identifier;

}

#endif

