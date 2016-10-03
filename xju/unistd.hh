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
    extern const SyscallF0<pid_t> fork;

    // post: result.first is read end
    //       result.second is write end
    //       both file descriptors have close-on-exec set (atomically
    //       on creation); both are set non-blocking
    std::pair<int,int> pipe() throw(xju::SyscallFailed);
    
    extern const SyscallF1<int, int*> pipe;
    
    std::string getcwd() throw(xju::SyscallFailed);

    void exec(std::string const& file,
              std::vector<std::string> const& argv) throw(xju::SyscallFailed);
    
              
}


#endif
