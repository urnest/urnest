// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "encodedLengthOfValue.hh"

#include <stdint.h>
#include "xju/snmp/encodedLengthOfLength.hh"
#include "xju/snmp/intDataLength.hh"
#include "xju/assert.hh"
#include "xju/snmp/oidDataLength.hh"
#include <vector>

namespace xju
{
namespace snmp
{

size_t encodedLengthOfValue(int64_t val_) throw()
{
  int const dataLength(intDataLength(val_));
  return 1/*type*/+
    encodedLengthOfLength(dataLength)+
    dataLength;
}

size_t encodedLengthOfValue(std::vector<uint8_t> const& val_) throw()
{
  // X.690 octetstring using primitive encoding
  uint64_t const dataLength(val_.size());
  return 1/*type*/+
    encodedLengthOfLength(dataLength)+
    dataLength;
}

size_t encodedLengthOfValue(Oid const& val_) throw()
{
  xju::assert_less(1U, val_.components().size());
  xju::assert_equal(val_.components()[0],1);
  xju::assert_equal(val_.components()[1],3);
  uint64_t const dataLength{oidDataLength(val_)};
  return 1/*type*/+
    encodedLengthOfLength(dataLength)+
    dataLength;
}


}
}
