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
#include <xju/unistd.hh>

#ifdef __MINGW32__

#include <io.h>


namespace xju
{
    namespace
    {
        int mkdir_wrapper(const char* n, mode_t)
        {
            return ::mkdir(n);
        }
    }
    
    const SyscallF3<int, int, void*, size_t> read(
	"read",
	::read);
    
    const SyscallF2<int, const char*, Stat*> stat_(
	"stat",
	::stat);

    const SyscallF3<int, int, const void*, size_t> write(
	"write",
	::write);

    const SyscallF1<int, int> close(
	"close",
	::close);

    const SyscallF3<off_t, int, off_t, int> lseek(
	"lseek",
	::lseek);

    const SyscallF2<int, const char*, mode_t> mkdir(
        "mkdir",
        mkdir_wrapper);

    const SyscallF1<int, const char*> rmdir(
        "rmdir",
        ::rmdir);

    const SyscallF1<int, const char*> unlink(
        "unlink",
        ::unlink);
}


#else

#ifdef __Lynx__
#include <lockf.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

namespace xju
{
#ifndef __CYGWIN__
    const SyscallF3<int, int, int, off_t> lockf(
	"lockf",
	::lockf);
#endif
    const SyscallF3<ssize_t, int, void*, size_t> read(
	"read",
	::read);
    
    const SyscallF2<int, const char*, Stat*> stat_(
	"stat",
	::stat);

    const SyscallF3<ssize_t, int, const void*, size_t> write(
	"write",
	::write);

    const SyscallF1<int, int> close(
	"close",
	::close);

    const SyscallF3<off_t, int, off_t, int> lseek(
	"lseek",
	::lseek);

    const SyscallF2<int, int, off_t> ftruncate(
	"ftruncate",
	::ftruncate);

    const SyscallF2<int, const char*, mode_t> mkdir(
        "mkdir",
        ::mkdir);

    const SyscallF1<int, const char*> rmdir(
        "rmdir",
        ::rmdir);

    const SyscallF1<int, const char*> unlink(
        "unlink",
        ::unlink);

    const SyscallF2<char*, char*, size_t> getcwd(
        "getcwd",
        ::getcwd);

    const SyscallF0<pid_t> fork(
        "fork",
        ::fork);

    std::string getcwd_() throw(xju::SyscallFailed)
    {
        char* x = syscall(getcwd, XJU_TRACED, false, (char*)0)(0, 0);
        std::string result(x);
        ::free(x);
        return result;
    }

    void execvp(std::string const& file,
                std::vector<std::string> const& argv) throw(
                    xju::SyscallFailed) {
        char* f(::strdup(file.c_str()));
        std::vector<char*> a;
        for(auto arg: argv) {
            a.push_back(::strdup(arg.c_str()));
        }
        a.push_back(0);
        ::execvp(f,&a[0]);
        ::free(f);
        for(auto s: a) {
            ::free(s);
        }
        throw xju::SyscallFailed("execvp",errno,XJU_TRACED);
    }
}


#endif
