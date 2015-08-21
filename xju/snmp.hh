// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_HH
#define XJU_SNMP_HH

#include <string>
#include "xju/Exception.hh"
#include <vector>
#include <stdint.h>
#include <memory>
#include <xju/Int.hh>
#include "xju/Tagged.hh"
#include <set>
#include <map>
#include "xju/MicroSeconds.hh"

// The intended use of this module for snmp-get is:
//
//   xju::snmp::SnmpV1GetRequest request(...);
//   std::vector<uint8_t> requestData(xju::snmp::encode(request));
//   ... send requestData to server
//   ... receive responseData as std::vector<uint8_t>
//   std::map<xju::snmp::Oid, std::shared_ptr<xju::snmp::Value> > values(
//     xju::snmp::validateResponse(
//       request,xju::snmp::decodeSnmpV1Response(responseData)));
//   ... use values
//
// ... and for snmp-set is:
//
//   xju::snmp::SnmpV1SetRequest request(...);
//   std::vector<uint8_t> requestData(xju::snmp::encode(request));
//   ... send requestData to server
//   ... receive responseData as std::vector<uint8_t>
//   xju::snmp::validateResponse(
//     request,xju::snmp::decodeSnmpV1Response(responseData));
//
// ... and for snmp-get-next is:
//   xju::snmp::SnmpV1Table t{oid1,oid2,oid3};
//   while(t.incomplete()) {
//     xju::snmp::SnmpV1GetNextRequest request(..., t.nextOids());
//     std::vector<uint8_t> requestData(xju::snmp::encode(request);
//     ... send requestData to server
//     ... receive responseData as std::vector<uint8_t>
//     t.add(xju::snmp::validateResponse(
//       request,xju::snmp::decodeSnmpV1Response(responseData));
//   }
//   for(auto i=0; i!=t.size(); ++i)
//   {
//     ... do something with t[oid1][i], t[oid2][i], t[oid3][i]
//   }
//  

#include "snmp/Oid.hh"
#include "snmp/RequestId.hh"
#include "snmp/Community.hh"
#include "snmp/IPv4Address.hh"
#include "snmp/SnmpV1GetRequest.hh"
#include "snmp/Value.hh"

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

std::vector<uint8_t> encode(SnmpV1SetRequest const& request) throw();

// validate reponse to specified request
// - note that RFC 1157 says that no values are modified if an error
//   is returned
void validateResponse(
  SnmpV1SetRequest const& request, 
  SnmpV1Response const& response) throw(
    // response.responseType_ != 0xA3
    ResponseTypeMismatch,
    // response.id_ != request.id_
    ResponseIdMismatch,
    // server does not know NoSuchName.oid_ 
    NoSuchName,
    // server forbids request.values_[BadValue.oid] as value of BadValue.oid
    BadValue,
    // ReadOnly.oid_ is read-only
    ReadOnly,
    // request was too big to process or responsd to
    // (note no values were set)
    TooBig,
    // SNMP General Error
    GenErr,
    // response malformed eg not all requested oids present in response
    xju::Exception);

struct SnmpV1GetNextRequest
{
  SnmpV1GetNextRequest(Community const& community,
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
                                  SnmpV1GetNextRequest const& x) throw();
};
  
std::vector<uint8_t> encode(SnmpV1GetNextRequest const& request) throw();

// validate reponse to specified request
// - result suitable to pass to SnmpV1Table.add()
//   note: NoSuchName error is translated to a result with all its oids
//         set to 1.3, to signal end of table (see SnmpV1Table)
std::vector<std::pair<Oid, std::shared_ptr<Value const> > > validateResponse(
  SnmpV1GetNextRequest const& request,
  SnmpV1Response const& response) throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    TooBig,
    GenErr,
    // response malformed eg not all requested oids present in response
    xju::Exception);

class SnmpV1Table
{
public:
  // pre: cols.size()>0
  // post: cols_==cols
  //       !atEnd()
  SnmpV1Table(std::set<Oid> cols) throw();
  
  std::set<Oid> const cols_;

  struct Cell
  {
    explicit Cell(std::pair<Oid, std::shared_ptr<Value const> > v) throw():
        oid_(v.first),
        value_(v.second) {
    }
    Oid oid_;
    std::shared_ptr<Value const> value_;
  };
  
  // pre: col in cols_
  std::vector<Cell> const& operator[](Oid const& col) const throw();
  
  bool atEnd() const throw() { return atEnd_; }

  // post: result.size()==cols_.size()
  std::vector<Oid> nextOids() const throw();
  
  // pre: row.size()==cols_.size()
  void add(
    std::vector<std::pair<Oid, std::shared_ptr<Value const> > > const& row)
  throw();

private:
  std::map<Oid, std::vector<Cell> > data_;
  bool atEnd_;
};

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
    IPv4Address origin,
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
  IPv4Address origin_;
  GenericType genericType_;
  SpecificType specificType_;
  xju::MicroSeconds timestamp_;

  std::map<Oid, std::shared_ptr<Value const> > vars_;
  
  friend std::ostream& operator<<(std::ostream& s, SnmpV1Trap const& x) 
    throw();
};

std::vector<uint8_t> encode(SnmpV1Trap const& trap) throw();

  
}
}

#endif
