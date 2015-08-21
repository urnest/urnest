// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_SNMPV1SETREQUEST_H
#define XJU_SNMP_SNMPV1SETREQUEST_H

#include "xju/snmp/Community.hh"
#include "xju/snmp/RequestId.hh"
#include <map>
#include <memory>
#include "xju/snmp/Value.hh"
#include <iostream>
#include "xju/snmp/Oid.hh"

namespace xju
{
namespace snmp
{

struct SnmpV1SetRequest
{
  SnmpV1SetRequest(
    Community community,
    RequestId id,
    std::map<Oid, std::shared_ptr<Value const> > values) throw():
      community_(community),
      id_(id),
      values_(values) {
  }
  Community community_;
  RequestId id_;
  std::map<Oid, std::shared_ptr<Value const> > values_;

  friend std::ostream& operator<<(std::ostream& s, 
                                  SnmpV1SetRequest const& x) throw();
};


}
}

#endif

