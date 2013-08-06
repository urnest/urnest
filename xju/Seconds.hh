//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2007
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SECONDS_HH_
#define XJU_SECONDS_HH_

#include "xju/Int.hh"
#include <sstream>

namespace xju
{
class SecondsTag{};

// Integral non-negative number of seconds.
typedef xju::Int<SecondsTag, unsigned int> Seconds;

inline std::ostream& operator<<(std::ostream& s, Seconds const& x) throw() {
  return s << x.value() << " seconds";
}
    
}

#endif
