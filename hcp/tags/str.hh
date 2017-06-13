#ifndef _STR_HH
#define _STR_HH
// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <string>
#include <vector>
#include <hcp/tags/NamespaceName.hh>
#include <hcp/tags/UnqualifiedSymbol.hh>

namespace hcp
{
namespace tags
{
std::string str(std::vector<NamespaceName> const& x) throw();

std::string str(UnqualifiedSymbol const& x) throw();



}
}


#endif
