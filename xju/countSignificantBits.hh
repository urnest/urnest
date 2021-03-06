// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_COUNTSGINIFICANTBITS_H
#define XJU_COUNTSGINIFICANTBITS_H

#include <stdint.h>


namespace xju
{

// return number of significant bits in x
// ie return highest bit number that is 1
// eg countSignificantBits(1)==1
//
int countSignificantBits(uint64_t const x) throw();

}

#endif

