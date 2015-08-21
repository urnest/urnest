// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_SNMPV1RESPONSE_H
#define XJU_SNMP_SNMPV1RESPONSE_H

#include <xju/Int.hh>
#include <stdint.h>
#include <xju/snmp/Community.hh>
#include <xju/snmp/RequestId.hh>
#include <vector>
#include <utility>
#include "xju/snmp/Oid.hh"
#include "xju/snmp/Value.hh"
#include <memory>
#include <iostream>

namespace xju
{
namespace snmp
{

struct SnmpV1Response
{
  // RFC 1157 error-status
  enum class ErrorStatus
  {
    NO_ERROR,
    TOO_BIG,
    NO_SUCH_NAME,
    BAD_VALUE,
    READ_ONLY,
    GEN_ERR
  };
  
  // RFC 1157 error-index
  // note error-index of first oid is 1, not 0
  class ErrorIndexTag{};
  typedef xju::Int<ErrorIndexTag,uint64_t> ErrorIndex;

  SnmpV1Response(
    uint8_t responseType,
    Community community,
    RequestId id,
    ErrorStatus error,
    ErrorIndex errorIndex,
    std::vector<std::pair<Oid, std::shared_ptr<Value const> > > values)
      throw():
      responseType_(responseType),
      community_(community),
      id_(id),
      error_(error),
      errorIndex_(errorIndex),
      values_(values) {
  }
  uint8_t responseType_;
  Community community_;
  RequestId id_;
  ErrorStatus error_;
  // 1-based ie bad oid is values_[errorIndex_-1]; 
  // (0 if error_ is a non-param-specific error)
  ErrorIndex errorIndex_;
  
  std::vector<std::pair<Oid, std::shared_ptr<Value const> > > values_;

  friend std::ostream& operator<<(std::ostream& s, SnmpV1Response const& x)
    throw();
  
};


}
}

#endif

