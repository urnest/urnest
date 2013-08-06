// -*- mode: c++ ; c-file-style : "mob" ; -*-
//
// Copyright (c) 1997-2007 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/isnan.hh>

#include <math.h>

bool xju_isnan(float x) throw()
{
  return isnan(x);
}
bool xju_isnan(double x) throw()
{
  return isnan(x);
}

