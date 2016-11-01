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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

namespace xju
{
    const SyscallF3<int, int, int, off_t> lockf(
	"lockf",
	::lockf);
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

    const SyscallF0<pid_t> fork(
        "fork",
        ::fork);

    const SyscallF2<int,int,int> dup2(
        "dup2",
        ::dup2);

    std::pair<int,int> pipe_() throw(xju::SyscallFailed)
    {
        int fds[2];
        syscall("pipe2",::pipe2, XJU_TRACED, true, -1)(
            fds,O_NONBLOCK|O_CLOEXEC);
        return std::make_pair(fds[0],fds[1]);
    }
    
    std::string getcwd() throw(std::bad_alloc,xju::SyscallFailed)
    {
        try {
            char* x = ::getcwd(0, 0);
            if (x==0) {
                throw std::bad_alloc();
            }
            try {
                std::string result(x);
                ::free(x);
                return result;
            }
            catch(...) {
                ::free(x);
            }
        }
        catch(xju::SyscallFailed& e) {
            e.addContext("get current working directory",XJU_TRACED);
            throw;
        }
    }

    void exec(std::string const& file,
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

