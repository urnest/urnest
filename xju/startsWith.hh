// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_STARTSWITH_H
#define XJU_STARTSWITH_H

#include <algorithm>
#include <iterator>

namespace xju
{
// true if x starts with y
template<class C1, class C2>
bool startsWith(C1 const& x, C2 const& y) throw() {
  return x.size()>=y.size() &&
    std::mismatch(y.begin(),y.end(),
                  x.begin()).first==y.end();
}

}

#endif

