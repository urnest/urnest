// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_ENCODEDLENGTHOFVALUE_H
#define XJU_SNMP_ENCODEDLENGTHOFVALUE_H

#include "xju/snmp/Oid.hh"
#include <vector>

namespace xju
{
namespace snmp
{

size_t encodedLengthOfValue(int64_t val_) throw();

size_t encodedLengthOfValue(std::vector<uint8_t> const& val_) throw();

size_t encodedLengthOfValue(Oid const& val_) throw();



}
}

#endif

