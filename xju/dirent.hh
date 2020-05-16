// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_DIRENT_H
#define XJU_DIRENT_H

#include <xju/syscall.hh>
#include <dirent.h>


namespace xju
{
extern const SyscallF1<DIR*, const char*> opendir;
extern const SyscallF1<int, DIR*> closedir;

// note readdir does not follow system call error conventions so
// is not made available like above
// - returns 0 at end of entries
dirent* readdir(DIR* dir,xju::Traced const& t)
// throw SyscallFailed
  ;

}

#endif

