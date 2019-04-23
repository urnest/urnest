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
#include <limits>
#include <sstream>

namespace xju
{
namespace linux
{
namespace disk_space
{

struct PercentImpl{
  explicit PercentImpl(int value) throw(
    //x not in range 0..100
    xju::Exception):
      value_(x)
  {
    if (x<0){
      std::ostringstream s;
      s << x << "% is invalid, must be >= 0%";
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    if (x>100){
      std::ostringstream s;
      s << x << "% is invalid, must be <= 100%";
      throw xju::Exception(s.str(),XJU_TRACED);
    }
  }
  int value() const noexcept { return value_; }
  Int& operator++() noexcept
  {
    ++value_;
    return *this;
  }
  
  Int operator++(int) noexcept
  {
    Int result(value_);
    ++value_;
    return result;
  }
  
  Int& operator--() noexcept
  {
    --value_;
    return *this;
  }
  
  Int operator--(int) noexcept
  {
    Int result(value_);
    --value_;
    return result;
  }
  
  Int& operator+=(Int const& y) noexcept
  {
    value_+=y.value();
    return *this;
  }
  
  Int& operator-=(Int const& y) noexcept
  {
    value_-=y.value();
    return *this;
  }

private:
  int value_;
  
  friend Int operator+(Int const& x, Int const& y)
  {
      Int result(x);
      result+=y;
      return result;
  }
  
  friend Int operator-(Int const& x, Int const& y)
  {
      Int result(x);
      result-=y;
      return result;
  }
  
  friend bool operator<(Int const& x, Int const& y) noexcept 
  {
    return x.value_ < y.value_;
  }
  friend bool operator>(Int const& x, Int const& y) noexcept 
  {
    return y < x;
  }
  friend bool operator!=(Int const& x, Int const& y) noexcept 
  {
    return y < x || x < y;
  }
  friend bool operator==(Int const& x, Int const& y) noexcept 
  {
    return !(x != y);
  }
  friend bool operator<=(Int const& x, Int const& y) noexcept 
  {
    return (x < y) || (x == y);
  }
  friend bool operator>=(Int const& x, Int const& y) noexcept 
  {
    return (x > y) || (x == y);
  }
  
};

}
}
}
namespace std
{
template<>
struct numeric_limits<xju::linux::disk_space::Percent>
{
  static xju::linux::disk_space::Percent min() noexcept {
    return xju::linux::disk_space::Percent(0);
  }
  static xju::linux::disk_space::Percent max() noexcept {
    return xju::linux::disk_space::Percent(100);
  }
};

}

namespace xju
{
namespace linux
{
namespace disk_space
{

class Percent : public xju::Int<PercentImpl,PercentImpl>
{
public:
  explicit Percent(int x) throw(
    // x not in range 0..100
    xju::Exception):
      xju::Int<PercentImpl,PercentImpl>(PercentImpl(x))
  {
  }
};

}
}
}

#endif
