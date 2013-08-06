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
#include <xju/socket.hh>

#ifndef __MINGW32__
namespace xju
{
    const SyscallF3<int, int, int, int> socket(
	"socket",
	::socket);
    const SyscallF3<int, int, const struct sockaddr*, socklen_t> connect(
        "connect",
        ::connect);
    const SyscallF3<int, int, const ::sockaddr*, socklen_t> bind(
	"bind",
	::bind);
    const SyscallF2<int, int, int> listen("listen", ::listen);
    const SyscallF3<int, int, ::sockaddr*, socklen_t*> accept(
	"accept",
	::accept);
    const SyscallF4<int, int, const void*, size_t, int> send(
	"send",
	::send);
    const SyscallF4<int, int, void*, size_t, int> recv(
	"recv",
	::recv);
    const SyscallF5<int, int, int, int, void*, socklen_t*> getsockopt(
	"getsockopt",
	::getsockopt);
    extern const SyscallF5<int, int, int, int, const void*, socklen_t>
    setsockopt(
	"setsockopt",
	::setsockopt);
}
#endif