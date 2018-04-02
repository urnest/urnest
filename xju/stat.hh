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
//    Descriptors for system calls defined by /usr/include/sys/stat.h for
//    use with xju::syscall (see syscall.hh).
//
#ifndef _XJU_STAT_HH_
#define _XJU_STAT_HH_


#include <sys/types.h>
#include <sys/stat.h>
#include <xju/syscall.hh>
#include <sys/types.h>

namespace xju
{
    extern const SyscallF2<int, const char*, mode_t> chmod;
    extern const SyscallF2<int, const char*, struct stat*> stat;
    extern const SyscallF2<int, int, struct stat*> fstat;
}


#endif
