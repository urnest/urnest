// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_SNMPV2cGETBULKREQUEST_H
#define XJU_SNMP_SNMPV2cGETBULKREQUEST_H

#include <xju/snmp/Community.hh>
#include <xju/snmp/RequestId.hh>
#include <vector>
#include <set>
#include <iostream>
#include <xju/snmp/Oid.hh>

namespace xju
{
namespace snmp
{

struct SnmpV2cGetBulkRequest
{
  // request to get values of "get" oids, and (up to) next n
  // values of each of "getNextN" oids.
  SnmpV2cGetBulkRequest(Community const& community,
                        RequestId const id,
                        std::set<Oid> const& get,
                        std::vector<Oid> const& getNextN,
                        unsigned int const n) throw():
      community_(community),
      id_(id),
      get_(get),
      getNextN_(getNextN),
      n_(n) {
  }
  SnmpV2cGetBulkRequest(Community const& community,
                        RequestId const id,
                        std::set<Oid> const& get) throw():
      community_(community),
      id_(id),
      get_(get),
      n_(0) {
  }
  SnmpV2cGetBulkRequest(Community const& community,
                        RequestId const id,
                        std::vector<Oid> const& getNextN,
                        unsigned int const n) throw():
      community_(community),
      id_(id),
      getNextN_(getNextN),
      n_(n) {
  }
  Community community_;
  RequestId id_;
  std::set<Oid> get_;
  std::vector<Oid> getNextN_;
  unsigned int n_;
  
  friend std::ostream& operator<<(
    std::ostream& s, 
    SnmpV2cGetBulkRequest const& x) throw();
};
  

}
}

#endif

