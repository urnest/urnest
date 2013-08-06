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
#ifndef XJU_AUTOFD_HH_
#define XJU_AUTOFD_HH_

#include <unistd.h>
#include <boost/utility.hpp>

namespace xju
{
    struct AutoFd : boost::noncopyable
    {
        AutoFd(int fd) throw():
            value_(fd)
        {
        }
        ~AutoFd() throw()
        {
            ::close(value_);
        }
        const int value_;
    };
}

#endif
