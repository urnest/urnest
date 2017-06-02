#ifndef _XJU_IO_FD_HCP
#define _XJU_IO_FD_HCP
// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <utility>
#include "xju/NonCopyable.hh"


namespace xju
{
namespace io
{

class Fd : xju::NonCopyable
{
public:
  // takes exclusive ownership of fd (close on destroy)
  // pre: fd is valid, open file descriptor
  explicit Fd(int fd) noexcept;

  ~Fd() noexcept;

  Fd& operator=(Fd&& x) noexcept;

  Fd(Fd&& y) noexcept;

  int fd() const noexcept;

private:
  int fd_;
};
  

}
}


#endif
