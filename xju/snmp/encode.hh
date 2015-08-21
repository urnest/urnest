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


namespace xju
{
namespace snmp
{

class SnmpV1GetRequest;
class SnmpV1SetRequest;
class SnmpV1GetNextRequest;
class SnmpV1Trap;

std::vector<uint8_t> encode(SnmpV1GetRequest const& request) throw();
std::vector<uint8_t> encode(SnmpV1SetRequest const& request) throw();
std::vector<uint8_t> encode(SnmpV1GetNextRequest const& request) throw();
std::vector<uint8_t> encode(SnmpV1Trap const& trap) throw();

}
}

#endif
