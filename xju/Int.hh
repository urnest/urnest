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
#ifndef XJU_INT_H_
#define XJU_INT_H_

#include "xju/Exception.hh"
#include <sstream>
#include <limits>

namespace xju
{
template<class Tag, class Impl = int>
class Int
{
public:
  explicit Int(Impl x) throw():
    value_(x) {
  }
  Impl value() const throw() {
    return value_;
  }
  Int& operator+=(Int const& y) throw(
    xju::Exception)
  {
    if (std::numeric_limits<Impl>::max() - value_ < y.value()) {
      std::ostringstream s;
      s << (*this) << " + " << y << " would overflow";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    value_+=y.value();
    return *this;
  }
  
  Int& operator-=(Int const& y) throw(
    xju::Exception)
  {
    if (value_ < y.value() + std::numeric_limits<Impl>::min()) {
      std::ostringstream s;
      s << (*this) << " - " << y << " would underflow";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    value_-=y.value();
    return *this;
  }
  
  friend Int operator+(Int const& x, Int const& y) throw(
    // overflow
    xju::Exception) 
  {
      Int result(x);
      result+=y;
      return result;
  }
  
  friend Int operator-(Int const& x, Int const& y) throw(
    // underflow
    xju::Exception) 
  {
      Int result(x);
      result-=y;
      return result;
  }
  
  friend bool operator<(Int const& x, Int const& y) throw() 
  {
    return x.value_ < y.value_;
  }
  friend bool operator>(Int const& x, Int const& y) throw() 
  {
    return y < x;
  }
  friend bool operator!=(Int const& x, Int const& y) throw() 
  {
    return y < x || x < y;
  }
  friend bool operator==(Int const& x, Int const& y) throw() 
  {
    return !(x != y);
  }
  friend bool operator<=(Int const& x, Int const& y) throw() 
  {
    return (x < y) || (x == y);
  }
  friend bool operator>=(Int const& x, Int const& y) throw() 
  {
    return (x > y) || (x == y);
  }
private:
    Impl value_;
};
}

#endif
