// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_ENCODE_H
#define XJU_SNMP_ENCODE_H

#include <vector>
#include <stdint.h>
#include "SnmpV1GetRequest.hh"

namespace xju
{
namespace snmp
{

std::vector<uint8_t> encode(SnmpV1GetRequest const& request) throw();

}
}

#endif

