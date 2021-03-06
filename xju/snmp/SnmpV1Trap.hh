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
#include <chrono>
#include <map>
#include <memory>
#include "xju/snmp/Value.hh"
#include <iostream>
#include <xju/seq_less.hh>

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
    std::chrono::milliseconds timestamp,
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
  std::chrono::milliseconds timestamp_;

  std::map<Oid, std::shared_ptr<Value const> > vars_;
  
  friend bool operator<(SnmpV1Trap const& x, SnmpV1Trap const& y) noexcept
  {
    if (x.community_<y.community_) return true;
    if (y.community_<x.community_) return false;
    if (x.trapType_<y.trapType_) return true;
    if (y.trapType_<x.trapType_) return false;
    if (x.origin_<y.origin_) return true;
    if (y.origin_<x.origin_) return false;
    if (x.genericType_<y.genericType_) return true;
    if (y.genericType_<x.genericType_) return false;
    if (x.specificType_<y.specificType_) return true;
    if (y.specificType_<x.specificType_) return false;
    if (x.timestamp_<y.timestamp_) return true;
    if (y.timestamp_<x.timestamp_) return false;
    return xju::seq_less(
      x.vars_.begin(),x.vars_.end(),
      y.vars_.begin(),y.vars_.end(),
      [](std::pair<Oid,std::shared_ptr<Value const> > const& x,
         std::pair<Oid,std::shared_ptr<Value const> > const& y){
        if (x.first<y.first) return true;
        if (y.first<x.first) return false;
        if (*x.second<*y.second) return true;
        if (*y.second<*x.second) return false;
        return false;
      });
  }
  friend bool operator==(SnmpV1Trap const& x, SnmpV1Trap const& y) noexcept
  {
    return !(x<y) && !(y<x);
  }
  friend bool operator<=(SnmpV1Trap const& x, SnmpV1Trap const& y) noexcept
  {
    return x<y || x==y;
  }
  friend bool operator>(SnmpV1Trap const& x, SnmpV1Trap const& y) noexcept
  {
    return y<x;
  }
  friend bool operator>=(SnmpV1Trap const& x, SnmpV1Trap const& y) noexcept
  {
    return x>y || x==y;
  }
  friend bool operator!=(SnmpV1Trap const& x, SnmpV1Trap const& y) noexcept
  {
    return !(x==y);
  }
  
  friend std::ostream& operator<<(std::ostream& s, SnmpV1Trap const& x) 
    throw();
};


}
}

#endif

