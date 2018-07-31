// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_STRIP_H
#define XJU_STRIP_H

#include <string>
#include <cctype>

namespace xju
{

inline std::string strip(std::string const& x) throw()
{
  auto i=x.begin();
  auto j=x.end();
  while(i!=j && std::isspace(*i)) ++i;
  while(j!=i && std::isspace(*(j-1))) --j;
  return std::string(i,j);
}

}

#endif
