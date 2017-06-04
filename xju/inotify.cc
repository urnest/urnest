// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/inotify.hh>

#include <sys/inotify.h> //impl

namespace xju
{
const SyscallF1<int,int> inotify_init1=SyscallF1<int,int>(
  "::inotify_init",::inotify_init1);
const SyscallF3<int,int,const char*,uint32_t> inotify_add_watch=
  SyscallF3<int,int,const char*,uint32_t>(
    "::inotify_add_watch", ::inotify_add_watch);
}
