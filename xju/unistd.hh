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
//    Descriptors for system calls defined by /usr/include/unistd.h for
//    use with logfilter::syscall (see syscall.hh).
//
//  NOTES:
//
//    [1] Alas, unix uses the name "stat" as both a function
//        and struct name, which causes all sorts of confusion
//        for gcc. This is why stat handling is so convoluted here.
//
#ifndef _XJU_UNISTD_HH_
#define _XJU_UNISTD_HH_

#ifdef __MINGW32__

#include <sys/types.h>
#include <sys/stat.h>
#include <xju/syscall.hh>


typedef struct stat btt_stat; // [1]


namespace xju
{
    typedef btt_stat Stat; // [1]
    
    extern const SyscallF3<int, int, void*, size_t> read;
    extern const SyscallF2<int, const char*, Stat*> stat_; // [1]
    extern const SyscallF3<int, int, const void*, size_t> write;
    extern const SyscallF3<off_t, int, off_t, int> lseek;
    extern const SyscallF1<int, int> close;
    extern const SyscallF2<int, const char*, mode_t> mkdir;
    extern const SyscallF1<int, const char*> rmdir;
    extern const SyscallF1<int, const char*> unlink;
}

#else


#include <sys/types.h>
#include <sys/stat.h>
#include <xju/syscall.hh>


typedef struct stat btt_stat; // [1]


namespace xju
{
    typedef btt_stat Stat; // [1]
    
    extern const SyscallF3<int, int, int, off_t> lockf;
    extern const SyscallF3<ssize_t, int, void*, size_t> read;
    extern const SyscallF2<int, const char*, Stat*> stat_; // [1]
    extern const SyscallF3<ssize_t, int, const void*, size_t> write;
    extern const SyscallF3<off_t, int, off_t, int> lseek;
    extern const SyscallF2<int, int, off_t> ftruncate;
    extern const SyscallF1<int, int> close;
    extern const SyscallF2<int, const char*, mode_t> mkdir;
    extern const SyscallF1<int, const char*> rmdir;
    extern const SyscallF1<int, const char*> unlink;
    extern const SyscallF2<char*, char*, size_t> getcwd;

    std::string getcwd_() throw(xju::SyscallFailed);
}


#endif

#endif
