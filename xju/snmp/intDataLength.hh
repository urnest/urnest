// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_INTDATALENGTH_H
#define XJU_SNMP_INTDATALENGTH_H

#include "xju/snmp/IntValue.hh"

namespace xju
{
namespace snmp
{

// number of bytes it takes to encode val_ value 
// ie excluding asn1 type and asn1 length fields
//
size_t intDataLength(int64_t val_) throw();

}
}

#endif

