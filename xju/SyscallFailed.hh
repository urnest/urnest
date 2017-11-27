// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SYSCALLFAILED_H
#define XJU_SYSCALLFAILED_H

#include <xju/Exception.hh>
#include <string>
#include <xju/Traced.hh>

namespace xju
{



class SyscallFailed : public Exception
{
public:
  SyscallFailed(const std::string& name,
                const int errno_,
                const Traced& location) throw():
      Exception(makeCause(name, errno_), location),
      _errno(errno_)
  {
  }
  const int _errno;
private:
  std::string makeCause(const std::string& name,
                        const int errno_) throw();
};



}

#endif




