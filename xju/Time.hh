//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2007 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    Wrapper for gettimeofday(2).
//
//    Rationale:
//
//       - basic comparisons, difference etc allow use for timeouts
//       - no "formatting" or "parsing" as it is too contentious (inividual
//         users - e.g. log file timestamp class - are in much better
//         position to provide appropraite formatting and parsing).
//
#ifndef _XJU_TIME_HH_
#define _XJU_TIME_HH_

#include <chrono>

namespace xju
{
    typedef std::chrono::system_clock::time_point Time;
}


#endif
