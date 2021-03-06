// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_HOLDER_HH
#define XJU_HOLDER_HH

#include <cinttypes>
#include <cstddef>
#include <xju/MinAlign.hh>

namespace xju
{

//
// object holder for use with placement new, eg:
//   Holder<X> h;
//     :
//   X* x=new(h) X(3);
//   x->~X();
//
template<class T>
class Holder
{
public:
  union H // for correct alignment
  {
    MinAlign<T> align_;
    uint8_t f_[sizeof(T)];
  };
  H x_;

  T* operator->() noexcept             { return (T*)&x_; }
  T const* operator->() const noexcept { return (T*)&x_; }
  T& operator*() noexcept             { return *(T*)&x_; }
  T const& operator*() const noexcept { return *(T const*)&x_; }
};

}

template<class T>
void* operator new(std::size_t, xju::Holder<T>& holder) throw()
{
  return &holder.x_;
}
#endif
