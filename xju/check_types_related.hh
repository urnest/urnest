// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_CHECK_TYPES_RELATED_H
#define XJU_CHECK_TYPES_RELATED_H

namespace xju
{
// usage:
//  check_types_related<T, U>();
//  ... does not compile unless T is a U or U is a T
//
template<class T, class U>
void check_types_related();

template<class T, class U>
struct CheckTypesRelated
{
  CheckTypesRelated(T* t, T* u) // U is a T
  {
  }
  CheckTypesRelated(U* t, U* u) // T is a U
  {
  }
};

template<class T>
struct CheckTypesRelated<T, T>
{
  CheckTypesRelated(T* t, T* u)
  {
  }
};

template<class T, class U>
void check_types_related()
{
  CheckTypesRelated<T, U>((T*)0, (U*)0);
}

}

#endif
