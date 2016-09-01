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
//    Not implemented for Windows.
//
#ifndef _XJU_SOCKET_HH_
#define _XJU_SOCKET_HH_

#ifndef __MINGW32__

#include <sys/socket.h>
#include <xju/syscall.hh>

namespace xju
{
    extern const SyscallF3<int, int, int, int> socket;
    extern const SyscallF3<int, int, const struct sockaddr*, socklen_t> connect;
    extern const SyscallF3<int, int, const ::sockaddr*, socklen_t> bind;
    extern const SyscallF2<int, int, int> listen;
    extern const SyscallF3<int, int, ::sockaddr*, socklen_t*> accept;
    extern const SyscallF4<ssize_t, int, const void*, size_t, int> send;
    extern const SyscallF4<ssize_t, int, void*, size_t, int> recv;
    extern const SyscallF5<int, int, int, int, void*, socklen_t*> getsockopt;
    extern const SyscallF5<int, int, int, int, const void*, socklen_t>
       setsockopt;
    
}

#endif

#endif
