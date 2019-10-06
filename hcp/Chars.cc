// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/Chars.hh>

#include <xju/next.hh> //impl
#include <xju/format.hh>
#include <iostream>

namespace hcp
{

Chars::Chars(std::string const& pattern) noexcept:
    pattern_(std::move(pattern))
{
  auto i(pattern_.begin());
  while(i!=pattern_.end()){
    auto c=(*i);
    bits_.set(c);
    chars_.insert(c);
    ++i;
    if (i!=pattern_.end()&&
        (*i)=='-'&&
        (xju::next(i)!=pattern_.end())&&
        (*xju::next(i)>c)){
      while((++c)<(*xju::next(i))){
        bits_.set(c);
        chars_.insert(c);
      }
      ++i;
    }
  }
}

std::ostream& operator<<(std::ostream& s, Chars const& x)
{
  return s << xju::format::quote(xju::format::cEscapeString(x.pattern_));
}

}
