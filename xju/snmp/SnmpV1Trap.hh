// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_SNMPV1TRAP_H
#define XJU_SNMP_SNMPV1TRAP_H

#include <xju/Int.hh>
#include "xju/snmp/Community.hh"
#include "xju/snmp/Oid.hh"
#include "xju/ip/v4/Address.hh"
#include "xju/MicroSeconds.hh"
#include <map>
#include <memory>
#include "xju/snmp/Value.hh"
#include <iostream>

namespace xju
{
namespace snmp
{

struct SnmpV1Trap
{
  enum class GenericType
  {
    COLDSTART,
    WARMSTART,
    LINKDOWN,
    LINKUP,
    AUTHENTICATIONFAILURE,
    EGPNEIGHBORLOSS,
    ENTERPRISESPECIFIC
  };
    
  struct SpecificTypeTag{};
  typedef xju::Int<SpecificTypeTag,uint32_t> SpecificType;
  
  SnmpV1Trap(
    Community community,
    Oid trapType,
    xju::ip::v4::Address origin,
    GenericType genericType,
    SpecificType specificType,
    xju::MicroSeconds timestamp,
    std::map<Oid, std::shared_ptr<Value const> > vars) throw():
      community_(community),
      trapType_(trapType),
      origin_(origin),
      genericType_(genericType),
      specificType_(specificType),
      timestamp_(timestamp),
      vars_(vars) {
  }

  Community community_;
  Oid trapType_;
  xju::ip::v4::Address origin_;
  GenericType genericType_;
  SpecificType specificType_;
  xju::MicroSeconds timestamp_;

  std::map<Oid, std::shared_ptr<Value const> > vars_;
  
  friend std::ostream& operator<<(std::ostream& s, SnmpV1Trap const& x) 
    throw();
};


}
}

#endif

