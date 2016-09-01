// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_SNMPV1GETREQUEST_H
#define XJU_SNMP_SNMPV1GETREQUEST_H

#include "Community.hh"
#include "RequestId.hh"
#include "Oid.hh"
#include <set>
#include <iostream>
#include <vector>

namespace xju
{
namespace snmp
{

struct SnmpV1GetRequest
{
  SnmpV1GetRequest(Community const& community,
                   RequestId const id,
                   std::set<Oid> const& oids) throw():
      community_(community),
      id_(id),
      oids_(oids) {
  }
  Community community_;
  RequestId id_;
  std::set<Oid> oids_;

  friend std::ostream& operator<<(std::ostream& s, 
                                  SnmpV1GetRequest const& x) throw();

};
  
}
}

#endif

