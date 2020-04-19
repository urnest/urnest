// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/io/DirectoryEntryEvent.hh>
#include <xju/format.hh>

namespace xju
{
namespace io
{

std::ostream& operator<<(std::ostream& s, DirectoryEntryEvent const& x)
{
  switch(x){
  case DirectoryEntryEvent::METADATA_CHANGED:return s<<"entry metadata changed";
  case DirectoryEntryEvent::WRITER_CLOSED: return s<<"entry written to";
  case DirectoryEntryEvent::ENTRY_ADDED: return s<<"entry added";
  case DirectoryEntryEvent::ENTRY_REMOVED: return s<< "entry removed";
  }
  return s << xju::format::str((uint32_t)x);
}


}
}

