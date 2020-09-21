//
// Copyright (c) 2004
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_OPTIONAL_HH
#define XJU_OPTIONAL_HH

#include <memory>
#include <xju/Holder.hh>
#include <xju/assert.hh>

namespace xju
{
template<class T>
class Optional
{
public:
  Optional() noexcept:
  x_(0)
  {
  }
  Optional(T const& x):
      x_(new(h_) T(x))
  {
  }
  Optional(T&& x):
      x_(new(h_) T(x))
  {
  }
  Optional(const Optional<T>& x):
      x_(x.valid()?new(h_) T(*x):0)
  {
  }
  ~Optional() noexcept
  {
    clear();
  }
  Optional(Optional<T>&& x):
      x_(x.valid()?new(h_) T(std::move(*x)):0)
  {
    x.clear();
  }
  bool valid() const noexcept
  {
    return x_!=0;
  }
  T& value() noexcept
  {
    xju::assert_not_equal(x_,(T*)0);
    return *x_;
  }
  const T& value() const noexcept
  {
    xju::assert_not_equal(x_,(T*)0);
    return *x_;
  }
  const T* get() const noexcept
  {
    return x_;
  }
  T* get() noexcept
  {
    return x_;
  }
  const T& operator*() const noexcept
  {
    return *x_;
  }
  T& operator*() noexcept
  {
    return *x_;
  }
  
  void clear() noexcept
  {
    if (x_){
      x_->~T();
    }
    x_=0;
  }
  void reset() noexcept
  {
    clear();
  }
  Optional& operator=(const T& x)
  {
    clear();
    x_=new(h_) T(x);
    return *this;
  }
  Optional& operator=(const Optional<T>& x)
  {
    if (this != &x)
    {
      clear();
      if (x.get()){
        (*this)=*x;
      }
    }
    return *this;
  }
  Optional& operator=(Optional<T>&& x)
  {
    if (this != &x)
    {
      clear();
      if (x.get()){
        (*this)=std::move(*x);
        x.clear();
      }
    }
    return *this;
  }
  
private:
  Holder<T> h_;
  T* x_;
};
template<class T>
bool operator<(const Optional<T>& x, const Optional<T>& y) 
{
  if (!x.valid())
  {
    return y.valid();
  }
  return y.valid() && (x.value() < y.value());
}
template<class T>
bool operator>(const Optional<T>& x, const Optional<T>& y)
{
  return y < x;
}
template<class T>
bool operator!=(const Optional<T>& x, const Optional<T>& y)
{
  return x < y || x > y;
}
template<class T>
bool operator==(const Optional<T>& x, const Optional<T>& y)
{
  return !(x != y);
}
template<class T>
bool operator>=(const Optional<T>& x, const Optional<T>& y)
{
  return !(x < y);
}
template<class T>
bool operator<=(const Optional<T>& x, const Optional<T>& y)
{
  return !(x > y);
}
}

#endif
