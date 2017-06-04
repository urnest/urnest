#ifndef _XJU_AUTOFD_HCP
#define _XJU_AUTOFD_HCP
// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/NonCopyable.hh"

namespace xju
{
class AutoFd : xju::NonCopyable
{
public:
  // takes exclusive ownership of fd (close on destroy)
  // pre: fd is valid, open file descriptor
  explicit AutoFd(int fd) noexcept;

  ~AutoFd() noexcept;

  AutoFd& operator=(AutoFd&& x) noexcept;

  AutoFd(AutoFd&& y) noexcept;

  int fd() const noexcept;

private:
  int fd_;
};
  

}
#endif
