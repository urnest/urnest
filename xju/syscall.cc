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
#include <xju/syscall.hh>


#include <sstream>
#include <errno.h>


namespace xju
{
    std::string SyscallFailed::SyscallFailed::makeCause(
	const std::string& name,
	const int errno_) throw()
    {
	std::ostringstream s;
	s << name << " system call failed, errno = " << errno_;
	return s.str();
    }
}
