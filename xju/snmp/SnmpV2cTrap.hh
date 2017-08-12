// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_SNMPV2CTRAP_H
#define XJU_SNMP_SNMPV2CTRAP_H

#include "xju/snmp/Community.hh"
#include "xju/snmp/RequestId.hh"
#include "xju/snmp/Oid.hh"
#include <chrono>
#include "xju/snmp/Value.hh"
#include <iosfwd>
#include <vector>
#include <utility>
#include <memory>

namespace xju
{
namespace snmp
{

struct SnmpV2cTrap
{
  SnmpV2cTrap(
    Community community,
    RequestId id,
    Oid trapType,
    std::chrono::milliseconds timestamp,
    std::vector<std::pair<Oid, std::shared_ptr<Value const> > > vars) throw():
      community_(community),
      id_(id),
      trapType_(trapType),
      timestamp_(timestamp),
      vars_(vars) {
  }

  Community community_;
  RequestId id_;
  Oid trapType_;
  std::chrono::milliseconds timestamp_;

  std::vector<std::pair<Oid, std::shared_ptr<Value const> > > vars_;
  
  friend std::ostream& operator<<(std::ostream& s, SnmpV2cTrap const& x) 
    throw();
};

}
}

#endif
