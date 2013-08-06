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
//    Descriptors for system calls defined by /usr/include/fcntl.h for
//    use with xju::syscall (see syscall.hh).
//
#ifndef _XJU_FCNTL_HH_
#define _XJU_FCNTL_HH_

#include <xju/syscall.hh>
#include <fcntl.h>

namespace xju
{
    extern const SyscallF3<int, const char*, int, mode_t> open;

#ifndef __MINGW32__
    extern const SyscallF3<int, int, int, long> fcntl1;
#endif

}

#endif
