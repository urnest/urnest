// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_SNMPV2cGETNEXTREQUEST_H
#define XJU_SNMP_SNMPV2cGETNEXTREQUEST_H

#include "xju/snmp/Community.hh"
#include "xju/snmp/RequestId.hh"
#include <vector>
#include <iostream>
#include "xju/snmp/Oid.hh"

namespace xju
{
namespace snmp
{

struct SnmpV2cGetNextRequest
{
  SnmpV2cGetNextRequest(Community const& community,
                        RequestId const id,
                        std::vector<Oid> const& oids) throw():
      community_(community),
      id_(id),
      oids_(oids) {
  }
  Community community_;
  RequestId id_;
  std::vector<Oid> oids_;

  friend std::ostream& operator<<(std::ostream& s, 
                                  SnmpV2cGetNextRequest const& x) throw();
};
  

}
}

#endif

