// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_BITS_METHOD2_H
#define XJU_BITS_METHOD2_H

namespace xju
{
namespace functional
{

template<class T, class R, class P>
class Method2 : public std::binary_function<T, P, R>
{
public:
  Method2(R (T::*method)(P p)) throw():
  m_method(method)
  {
  }
  R operator()(T& x, P p) const
  {
    return (x.*m_method)(p);
  }
private:
  R (T::*const m_method)(P p);
};

}
}

#endif

