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
#include <xju/stat.hh>

#ifdef __MINGW32__
namespace 
{
    int chmod_(const char* f, mode_t mode)
    {
	return ::chmod(f, mode);
    }
}

namespace xju
{
    const SyscallF2<int, const char*, mode_t> chmod(
	"chmod",
	chmod_);
}
#else
namespace xju
{
    const SyscallF2<int, const char*, mode_t> chmod(
	"chmod",
	::chmod);
}
#endif
