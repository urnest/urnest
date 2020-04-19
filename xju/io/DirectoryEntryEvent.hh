// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <iostream>
#include <sys/inotify.h>
#include <cinttypes>

namespace xju
{
namespace io
{

enum class DirectoryEntryEvent : uint32_t {
  METADATA_CHANGED=IN_ATTRIB,
  WRITER_CLOSED=IN_CLOSE_WRITE,
  ENTRY_ADDED=IN_CREATE|IN_MOVED_TO,
  ENTRY_REMOVED=IN_DELETE|IN_MOVED_FROM
};

std::ostream& operator<<(std::ostream& s, DirectoryEntryEvent const& x);
    
}
}


