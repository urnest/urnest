//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2014
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_NONCOPYABLE_HH_
#define XJU_NONCOPYABLE_HH_

namespace xju
{
class NonCopyable
{
private:
  // not implemented
  NonCopyable(NonCopyable const&);                               
  NonCopyable& operator=(NonCopyable const&);

public:
  NonCopyable() throw()
  {
  }
};
  
}

#endif
