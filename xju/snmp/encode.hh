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
#include <xju/snmp/ContextEngineID.hh>
#include <xju/snmp/ContextName.hh>


namespace xju
{
namespace snmp
{

class SnmpV1GetRequest;
class SnmpV1GetNextRequest;
class SnmpV1SetRequest;
class SnmpV1Trap;
class SnmpV1Response;

std::vector<uint8_t> encode(SnmpV1GetRequest const& request) throw();
std::vector<uint8_t> encode(SnmpV1SetRequest const& request) throw();
std::vector<uint8_t> encode(SnmpV1GetNextRequest const& request) throw();
std::vector<uint8_t> encode(SnmpV1Trap const& trap) throw();
std::vector<uint8_t> encode(SnmpV1Response const& trap) throw();

class SnmpV2cGetRequest;
class SnmpV2cGetNextRequest;
class SnmpV2cGetBulkRequest;
class SnmpV2cSetRequest;
class SnmpV2cTrap;
class SnmpV2cResponse;

std::vector<uint8_t> encode(SnmpV2cGetRequest const& request) throw();
std::vector<uint8_t> encode(SnmpV2cGetNextRequest const& request) throw();
std::vector<uint8_t> encode(SnmpV2cSetRequest const& request) throw();
std::vector<uint8_t> encode(SnmpV2cTrap const& trap) throw();
std::vector<uint8_t> encode(SnmpV2cGetBulkRequest const& request) throw();
std::vector<uint8_t> encode(SnmpV2cResponse const& trap) throw();

class SnmpV3Message;

std::vector<uint8_t> encode(SnmpV3Message const& message) throw();

class SnmpV3ScopedPDU;

std::vector<uint8_t> encode(SnmpV3ScopedPDU pdu) throw();

}
}

#endif
