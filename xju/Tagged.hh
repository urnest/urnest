//     -*- mode: c++ ; c-file-style: "xju" ; -*-
//
// Copyright (c) 2008
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_TAGGED_HH_
#define XJU_TAGGED_HH_

#include <iosfwd>

namespace xju
{
template<class T, class Tag>
class Tagged
{
public:
  T _;
  
  explicit Tagged(T const& x):
    _(x) {
  }
};
template<class T, class Tag>
std::ostream& operator<<(std::ostream& s, Tagged<T, Tag> const& x) throw()
{
  return s << x._;
}

template<class T, class Tag>
bool operator<(Tagged<T, Tag> const& x, Tagged<T, Tag> const& y) throw()
{
  return x._ < y._;
}

template<class T, class Tag>
bool operator>(Tagged<T, Tag> const& x, Tagged<T, Tag> const& y) throw()
{
  return x._ > y._;
}

template<class T, class Tag>
bool operator==(Tagged<T, Tag> const& x, Tagged<T, Tag> const& y) throw()
{
  return x._ == y._;
}

template<class T, class Tag>
bool operator<=(Tagged<T, Tag> const& x, Tagged<T, Tag> const& y) throw()
{
  return x._ <= y._;
}

template<class T, class Tag>
bool operator>=(Tagged<T, Tag> const& x, Tagged<T, Tag> const& y) throw()
{
  return x._ >= y._;
}

}


#endif
