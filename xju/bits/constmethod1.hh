// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_BITS_CONSTMETHOD1_H
#define XJU_BITS_CONSTMETHOD1_H

#include <functional>

namespace xju
{
namespace functional
{

template<class T, class R>
class ConstMethod : public std::unary_function<T, R>
{
public:
  ConstMethod(R (T::*method)() const) throw():
  m_method(method)
  {
  }
  R operator()(const T& x) const
  {
    return (x.*m_method)();
  }
private:
  R (T::*const m_method)() const;
};

}
}

#endif

