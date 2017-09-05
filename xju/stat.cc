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

namespace xju
{
    const SyscallF2<int, const char*, mode_t> chmod={
	"chmod",
	::chmod};
    const SyscallF2<int, const char*, struct stat*> stat={
	"stat",
	::stat};
}
