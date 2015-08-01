// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/countSignificantBits.hh"

namespace xju
{
namespace
{
uint64_t mask(int bits) throw()
{
  uint64_t const one(1);
  return (one<<bits)-1;
}
}

int countSignificantBits(uint64_t const x) throw()
{
  if (x==0) {
    return 0;
  }
  int h=64;
  int l=0;
  int g=32;
  uint64_t const one(1);
  
  while(h-l>1) {
    if ((mask(g)&x)==x) {
      h=g;
    }
    else {
      l=g;
    }
    g=(h+l)/2;
  }
  if ((mask(l)&x) == x) {
    return l;
  }
  return h;
}


}
