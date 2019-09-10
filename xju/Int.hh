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

#include <xju/Exception.hh>
#include <sstream>
#include <limits>
#include <iosfwd>

namespace xju
{
template<class Tag, class Impl = int, class E = xju::Exception>
class Int
{
public:
  explicit Int(Impl x) throw():
    value_(x) {
  }
  Impl value() const throw() {
    return value_;
  }

  Int& operator++() throw(
    E)
  {
    if (std::numeric_limits<Impl>::max()==value_) {
      std::ostringstream s;
      s << "++" << (*this) << " would overflow";
      throw E(s.str(), XJU_TRACED);
    }
    ++value_;
    return *this;
  }
  
  Int operator++(int) throw(
    E)
  {
    if (std::numeric_limits<Impl>::max()==value_) {
      std::ostringstream s;
      s << "++" << (*this) << " would overflow";
      throw E(s.str(), XJU_TRACED);
    }
    Int const result(*this);
    ++value_;
    return result;
  }
  
  Int& operator--() throw(
    E)
  {
    if (std::numeric_limits<Impl>::min()==value_) {
      std::ostringstream s;
      s << "--" << (*this) << " would underflow";
      throw E(s.str(), XJU_TRACED);
    }
    --value_;
    return *this;
  }
  
  Int operator--(int) throw(
    E)
  {
    if (std::numeric_limits<Impl>::min()==value_) {
      std::ostringstream s;
      s << "--" << (*this) << " would underflow";
      throw E(s.str(), XJU_TRACED);
    }
    Int result(value_);
    --value_;
    return result;
  }
  
  Int& operator+=(Int const& y) throw(
    E)
  {
    if (y.value_<0) {
      return (*this)-=Int(-y.value_);
    }
    if (std::numeric_limits<Impl>::max() - value_ < y.value()) {
      std::ostringstream s;
      s << (*this) << " + " << y << " would overflow";
      throw E(s.str(), XJU_TRACED);
    }
    value_+=y.value();
    return *this;
  }
  
  Int& operator-=(Int const& y) throw(
    E)
  {
    if (y.value_<0) {
      return (*this)+=Int(-y.value_);
    }
    if (value_ < y.value() + std::numeric_limits<Impl>::min()) {
      std::ostringstream s;
      s << (*this) << " - " << y << " would underflow";
      throw E(s.str(), XJU_TRACED);
    }
    value_-=y.value();
    return *this;
  }
  
  friend Int operator+(Int const& x, Int const& y) throw(
    // overflow
    E) 
  {
      Int result(x);
      result+=y;
      return result;
  }
  
  friend Int operator-(Int const& x, Int const& y) throw(
    // underflow
    E) 
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
template<class Tag, class Impl, class E>
std::ostream& operator<<(std::ostream& s, Int<Tag,Impl,E> const& x) throw()
{
  return s << x.value();
}

}

#endif
