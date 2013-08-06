// -*- mode: c++ ; c-file-style : "mob" ; -*-
//
// Copyright (c) 2007 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//   Wrapper to circumvent mingw32 problems finding isnan (depending on
//   headers included and their order).
//
// NOTES:
//
//    [1] we don't use a namespace because some environments
//        #define isnan
//
#ifndef _XJU_ISNAN_HH_
#define _XJU_ISNAN_HH_

bool xju_isnan(float x) throw();
bool xju_isnan(double x) throw();

#endif
