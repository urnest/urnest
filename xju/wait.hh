// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_WAIT_HH
#define XJU_WAIT_HH

#include "xju/syscall.hh"
#include <sys/types.h>

namespace xju
{
extern xju::SyscallF3<pid_t,pid_t,int*,int> const waitpid;
}

#endif

