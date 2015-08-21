// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "intDataLength.hh"

namespace xju
{
namespace snmp
{

size_t intDataLength(int64_t val_) throw()
{
  int dataLength=8;
  if (val_>0) {
    // for positive numbers, remove leading byte while have 9 leading 0 bits
    int64_t mask=0xff80000000000000;
    while(dataLength>1 && ((val_ & ~mask)==val_)) {
      --dataLength;
      mask = (mask >> 8) | 0xff00000000000000;
    }
  }
  else if (val_<0)
  {
    // for negative numbers, remove leading byte while have 9 leading 1 bits
    int64_t mask=0xff80000000000000;
    while(dataLength>1 && ((val_ & mask)==mask)) {
      --dataLength;
      mask = (mask >> 8) | 0xff00000000000000;
    }
  }
  else
  {
    dataLength=1; // 0 is encoded using a single 0 byte
  }
  return dataLength;
}


}
}
