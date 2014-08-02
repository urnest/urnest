// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_BITS_VOIDCONSTMETHOD2_H
#define XJU_BITS_VOIDCONSTMETHOD2_H

#include <functional>

namespace xju
{
namespace functional
{

template<class T, class P>
class VoidConstMethod2 : public std::binary_function<T, P, bool>
{
public:
  VoidConstMethod2(bool (T::*method)(P) const) throw():
  m_method(method)
  {
  }
  bool operator()(const T& x, P p) const
  {
    return (x.*m_method)(p), true;
  }
private:
  void (T::*const m_method)(P) const;
};

}
}

#endif

