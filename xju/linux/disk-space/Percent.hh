// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_LINUX_DISK_SPACE_PERCENT_H
#define XJU_LINUX_DISK_SPACE_PERCENT_H

#include <xju/Int.hh>

namespace xju
{
namespace linux
{
namespace disk_space
{

struct PercentTag{};
typedef xju::Int<PercentTag> Percent;

}
}
}

#endif

