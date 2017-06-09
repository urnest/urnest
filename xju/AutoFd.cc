// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/AutoFd.hh>
#include "xju/assert.hh" //impl
#include "xju/syscall.hh" //impl
#include "xju/unistd.hh" //impl


namespace xju
{
AutoFd::AutoFd(int fd) noexcept:
  fd_(fd)
{
  xju::assert_greater_equal(fd,0);
}

AutoFd::~AutoFd() noexcept
{
  if (fd_!=-1) {
    xju::syscall(xju::close,XJU_TRACED)(fd_);
  }
}

AutoFd& AutoFd::operator=(AutoFd&& x) noexcept
{
  if (this != &x) {
    if (fd_!=-1) {
      xju::syscall(xju::close,XJU_TRACED)(fd_);
    }
    fd_=x.fd_;
    x.fd_=-1;
  }
  return *this;
}

AutoFd::AutoFd(AutoFd&& y) noexcept:
  fd_(std::move(y.fd_))
{
  y.fd_=-1;
}

int AutoFd::fd() const noexcept
{
  return fd_;
}

}
