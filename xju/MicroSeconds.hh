//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2008
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_MICROSECONDS_HH_
#define XJU_MICROSECONDS_HH_

#include "xju/Int.hh"
#include <sstream>
#include "xju/format.hh"
#include <boost/cstdint.hpp>

namespace xju
{
class MicroSecondsTag{};

// Integral non-negative number of microseconds.
typedef xju::Int<MicroSecondsTag, uint64_t> MicroSeconds;

inline std::ostream& operator<<(std::ostream& s, 
                                MicroSeconds const& x) throw() {
  return s << xju::format::int_(x.value()/1000000)
           << "."
           << xju::format::int_(x.value()%1000000, 8)
           << " seconds";
}
    
}

#endif
