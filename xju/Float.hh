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
#ifndef XJU_FLOAT_H_
#define XJU_FLOAT_H_

#include "xju/Exception.hh"
#include <sstream>
#include <limits>
#include <iosfwd>
#include <xju/isnan.hh>

namespace xju
{
template<class Tag, class Impl = float, class E = xju::Exception>
class Float
{
public:
  explicit Float(Impl x) throw():
    value_(x) {
  }
  Impl value() const throw() {
    return value_;
  }
  Float& operator+=(Float const& y) /*throw(
    E)*/
  {
    if (y.value_<0) {
      return (*this)-=Float(-y.value_);
    }
    if (std::numeric_limits<Impl>::max() - value_ < y.value()) {
      std::ostringstream s;
      s << (*this) << " + " << y << " would overflow";
      throw E(s.str(), XJU_TRACED);
    }
    value_+=y.value();
    return *this;
  }
  
  Float& operator-=(Float const& y) /*throw(
    E)*/
  {
    if (y.value_<0) {
      return (*this)+=Float(-y.value_);
    }
    if (value_ < y.value() + std::numeric_limits<Impl>::min()) {
      std::ostringstream s;
      s << (*this) << " - " << y << " would underflow";
      throw E(s.str(), XJU_TRACED);
    }
    value_-=y.value();
    return *this;
  }
  
  friend Float operator+(Float const& x, Float const& y) /*throw(
    // overflow
    E)*/ 
  {
      Float result(x);
      result+=y;
      return result;
  }
  
  friend Float operator-(Float const& x, Float const& y) /*throw(
    // underflow
    E)*/ 
  {
      Float result(x);
      result-=y;
      return result;
  }
  friend bool isnan(Float const& x) throw()
  {
    return xju_isnan(x.value());
  }
  friend bool operator<(Float const& x, Float const& y) throw() 
  {
    return x.value_ < y.value_;
  }
  friend bool operator>(Float const& x, Float const& y) throw() 
  {
    return y < x;
  }
  friend bool operator!=(Float const& x, Float const& y) throw() 
  {
    return y < x || x < y;
  }
  friend bool operator==(Float const& x, Float const& y) throw() 
  {
    return !(x != y);
  }
  friend bool operator<=(Float const& x, Float const& y) throw() 
  {
    return (x < y) || (x == y);
  }
  friend bool operator>=(Float const& x, Float const& y) throw() 
  {
    return (x > y) || (x == y);
  }
private:
    Impl value_;
};
template<class Tag, class Impl, class E>
std::ostream& operator<<(std::ostream& s, Float<Tag,Impl,E> const& x) throw()
{
  return s << x.value();
}

}

#endif
