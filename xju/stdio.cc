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
#include <xju/stdio.hh>

#ifdef __MINGW32__
#include <unistd.h>
#endif

namespace xju
{
#ifdef __MINGW32__
    namespace detail
    {
        //
        // On windows, rename fails with errno = 17 if the "to" file
        // exists (on Unix "to" is replaced).
        //
        // REVISIT: this implementation does not meet rename's atomicity
        // guarentees.
        //
        int rename_(const char* from, const char* to)
        {
            int status(::rename(from, to));
            if (status == -1 && 
		(errno == EEXIST || errno == EACCES))
            {
                int ss(::unlink(to));
                status = ::rename(from, to);
            }
	    if (status != 0)
	    {
		return -1;
	    }
            return status;
        }
    }
    const SyscallF2<int, const char*, const char*> rename(
	"rename",
	detail::rename_);

#else
    const SyscallF2<int, const char*, const char*> rename(
	"rename",
	::rename);
#endif

}
