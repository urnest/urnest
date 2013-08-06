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
//    netdb.h syscall descriptors.
//
//    Not implemented for Windows.
//
#ifndef _XJU_NETDB_HH_
#define _XJU_NETDB_HH_

#ifndef __MINGW32__

#include <netdb.h>
#include <xju/syscall.hh>

namespace xju
{
    extern const SyscallF1<struct hostent*, const char*> gethostbyname;
}

#endif

#endif
