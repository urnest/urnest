// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef _XJU_UNIX_EPOCH_HH
#define _XJU_UNIX_EPOCH_HH

#include <chrono>
namespace xju
{

// get unix epoch
std::chrono::system_clock::time_point unix_epoch() throw();


}
#endif
