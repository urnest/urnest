// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef CXY_UNIXTIMEMILLI_H
#define CXY_UNIXTIMEMILLI_H

namespace cxy
{
// conversion to idl truncates
typedef std::chrono::system_clock::time_point UnixTimeMilli;
}

#endif
