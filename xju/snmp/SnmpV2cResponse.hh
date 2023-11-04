// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_SNMPV2cRESPONSE_H
#define XJU_SNMP_SNMPV2cRESPONSE_H

#include <xju/Int.hh>
#include <stdint.h>
#include <xju/snmp/Community.hh>
#include <xju/snmp/RequestId.hh>
#include <vector>
#include <utility>
#include <xju/snmp/Oid.hh>
#include <xju/snmp/Value.hh>
#include <memory>
#include <iostream>
#include <xju/Optional.hh>
#include <xju/snmp/SnmpVar.hh>

namespace xju
{
namespace snmp
{

struct SnmpV2cResponse
{
  // RFC 1157 error-status
  enum class ErrorStatus
  {
    NO_ERROR=0,
    TOO_BIG=1,
    NO_SUCH_NAME=2,
    BAD_VALUE=3,
    READ_ONLY=4,
    GEN_ERR=5,
    NO_ACCESS=6,
    WRONG_TYPE=7,
    WRONG_LENGTH=8,
    WRONG_ENCODING=9,
    WRONG_VALUE=10,
    NO_CREATION=11,
    INCONSISTENT_VALUE=12,
    RESOURCE_UNAVAILABLE=13,
    COMMIT_FAILED=14,
    UNDO_FAILED=15,
    AUTHORIZATION_ERROR=16,
    NOT_WRITABLE=17,
    INCONSISTENT_NAME=18
  };
  
  // RFC 1157 error-index
  // note error-index of first oid is 1, not 0
  class ErrorIndexTag{};
  typedef xju::Int<ErrorIndexTag,uint64_t> ErrorIndex;

  SnmpV2cResponse(
    uint8_t responseType,
    Community community,
    RequestId id,
    ErrorStatus error,
    ErrorIndex errorIndex,
    std::vector<SnmpVar> varResults) throw():
      responseType_(responseType),
      community_(community),
      id_(id),
      error_(error),
      errorIndex_(errorIndex),
      varResults_(std::move(varResults)) {
  }
  uint8_t responseType_;
  Community community_;
  RequestId id_;
  ErrorStatus error_;
  // 1-based ie bad oid is values_[errorIndex_-1]; 
  // (0 if error_ is a non-param-specific error)
  ErrorIndex errorIndex_;

  std::vector<SnmpVar> varResults_;

  friend std::ostream& operator<<(std::ostream& s, SnmpV2cResponse const& x)
    throw();
  
};

}
}

#endif
