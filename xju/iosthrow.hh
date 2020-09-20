// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_IOSTHROW_H
#define XJU_IOSTHROW_H

#include <iostream>

namespace xju
{

class iosthrow
{
public:
  explicit iosthrow(std::ios& s) noexcept:
      s_(s),
      old_(s.exceptions())
  {
    s.exceptions(std::ios::failbit |
                 std::ios::badbit |
                 std::ios::eofbit);
  }
  ~iosthrow() noexcept
  {
    // clear fail bits otherwise following call to exceptions
    // will throw on fail bit, but if fail bit is set we are
    // already handling the corresponding exception - even
    // though we cannot go from exception masked to unmasked!
    s_.clear();
    s_.exceptions(old_);
  }
private:
  std::ios& s_;
  std::ios::iostate const old_;
};

}

#endif
