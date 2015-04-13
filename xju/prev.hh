// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_PREV_H
#define XJU_PREV_H

namespace xju
{
template<class Iterator>
Iterator prev(Iterator i)
{
  --i;
  return i;
}

}

#endif

