// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_BITS_VOIDMETHOD1_H
#define XJU_BITS_VOIDMETHOD1_H

namespace xju
{
namespace functional
{

template<class T>
class VoidMethod : public std::unary_function<T, bool>
{
public:
  VoidMethod(void (T::*method)()) throw():
  m_method(method)
  {
  }
  bool operator()(T& x) const
  {
    return (x.*m_method)(), true;
  }
private:
  void (T::*const m_method)();
};

}
}

#endif

