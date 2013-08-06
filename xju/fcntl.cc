//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2003
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/fcntl.hh>

#ifdef __MINGW32__
#include <io.h>
#endif

namespace
{
    int open_(const char* pathname, int flags, mode_t mode)
    {
	return ::open(pathname, flags, mode);
    }
#ifndef __MINGW32__
    int fcntl1_(int fd, int cmd, long arg)
    {
        return ::fcntl(fd, cmd, arg);
    }
#endif
}

namespace xju
{
    const SyscallF3<int, const char*, int, mode_t> open(
	"open",
	open_);

#ifndef __MINGW32__
    const SyscallF3<int, int, int, long> fcntl1("fcntl", fcntl1_);
#endif
}
