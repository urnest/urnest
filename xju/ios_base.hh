//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 1997-2007 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    Support older standard libraries (e.g. gcc 2.95) which had
//    ios::fixed instead of ios_base::fixed etc.
//
#ifndef _XJU_IOS_BASE_HH_
#define _XJU_IOS_BASE_HH_

#if !defined(__GNUC__) || __GNUC__ >= 3

#include <ios>

namespace xju
{
    typedef std::ios_base ios_base;
    inline std::ios_base::fmtflags ios_base_fmt_normal()
    {
        return std::ios_base::fmtflags(0);
    }
}

#else

#include <streambuf.h>
namespace xju
{
    typedef std::ios ios_base;
    inline int ios_base_fmt_normal()
    {
        return 0;
    }
}

#endif

#endif
