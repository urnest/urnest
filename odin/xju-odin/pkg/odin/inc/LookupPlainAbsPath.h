// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef _LOOKUPPLAINABSPATH_H
#define _LOOKUPPLAINABSPATH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "GMC.h"
#include "Type.hh"

  tp_FilHdr LookupPlainAbsPath(char const*const path);
  void RetAbsPaths();
  
#ifdef __cplusplus
}
#endif

#endif

