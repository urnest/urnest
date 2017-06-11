#ifndef _SPLITSYMBOL_HH
#define _SPLITSYMBOL_HH
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
#include <vector>
#include <hcp/tags/NamespaceName.hh>
#include <hcp/tags/UnqualifiedSymbol.hh>
#include <string>
namespace hcp
{
namespace tags
{
std::pair<std::vector<NamespaceName>,UnqualifiedSymbol> splitSymbol(
  std::string const& symbol) throw();



}
}


#endif
