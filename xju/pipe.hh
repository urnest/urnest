#ifndef _XJU_PIPE_HCP
#define _XJU_PIPE_HCP
// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <xju/io.hh>
#include <utility>
#include <memory>

#include <fcntl.h>

namespace xju
{

std::pair<std::unique_ptr<xju::io::IStream>, std::unique_ptr<xju::io::OStream> > pipe(
  bool closeReadEndOnExec,
  bool closeWriteEndOnExec) throw(std::bad_alloc,
                                  SyscallFailed);


}
#endif
