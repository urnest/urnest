// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "snmp.hh"

#include <sstream>
#include <xju/stringToUInt.hh>
#include <xju/format.hh>
#include "xju/countSignificantBits.hh"
#include "xju/assert.hh"
#include "xju/Optional.hh"
#include <limits.h>
#include "xju/functional.hh"

namespace xju
{
namespace snmp
{

// This implementation is based on the information in:
//   https://en.wikipedia.org/wiki/X.690#BER_encoding
//   http://www.rane.com/note161.html
//   RFC 1157 https://www.ietf.org/rfc/rfc1157.txt
//   http://www.itu.int/ITU-T/studygroups/com17/languages/X.690-0207.pdf
//


GenErr::GenErr(const xju::Traced& trace) throw():
    xju::Exception("SNMP General Error", trace)
{
}

TooBig::TooBig(const xju::Traced& trace) throw():
    xju::Exception("SNMP response would have exceeded server internal limit",
                   trace) 
{
}

InvalidParam::InvalidParam(std::string const& cause,
                           Oid const& param, const xju::Traced& trace) throw():
    xju::Exception(cause, trace),
    param_(param) 
{
}

NoSuchName::NoSuchName(Oid const& param, const xju::Traced& trace) throw():
    InvalidParam("server has no object with oid "+param.toString(),
                 param,
                 trace)
{
}

BadValue::BadValue(Oid const& param, const xju::Traced& trace) throw():
    InvalidParam("value of "+param.toString()+" is invalid",
                 param,
                 trace)
{
}
ReadOnly::ReadOnly(Oid const& param, const xju::Traced& trace) throw():
    InvalidParam("object oid "+param.toString()+" is read-only",
                 param,
                 trace)
{
}
ResponseTypeMismatch::ResponseTypeMismatch(
  uint8_t const got,
  uint8_t const expected,
  xju::Traced const& trace) throw():
    xju::Exception("expected response of type "+xju::format::hex(expected)+
                   " but got response of type "+xju::format::hex(got),
                   trace),
    got_(got),
    expected_(expected) 
{
}
ResponseIdMismatch::ResponseIdMismatch(RequestId const got,
                                       RequestId const expected,
                                       xju::Traced const& trace) throw():
    xju::Exception("expected response with id "+xju::format::str(expected)+
                   " but got response of id "+xju::format::str(got),
                   trace),
    got_(got),
    expected_(expected) 
{
}

std::ostream& operator<<(std::ostream& s, 
                         SnmpV1SetRequest const& x) throw() {
  return s << "community " << x.community_._ << ", id " << x.id_.value()
           << ", values " 
           << xju::format::join(
             x.values_.begin(),x.values_.end(),
             [](std::pair<Oid const, std::shared_ptr<Value const> > x) {
               std::ostringstream s;
               s << x.first << ": " << (*x.second);
               return s.str();
             },
             ", ");
}

void validateResponse(
  SnmpV1SetRequest const& request, 
  SnmpV1Response const& response) throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    NoSuchName,
    BadValue,
    ReadOnly,
    TooBig,
    GenErr,
    xju::Exception)
{
  try {
    if (request.id_ != response.id_) {
      throw ResponseIdMismatch(response.id_,request.id_,XJU_TRACED);
    }
    if (response.responseType_!=0xA2) {
      throw ResponseTypeMismatch(response.responseType_,0xA2,XJU_TRACED);
    }
    switch(response.error_) {
    case 0: break;
    case 0x01: throw TooBig(XJU_TRACED);
    case 0x02:
    {
      if (response.errorIndex_==SnmpV1Response::ErrorIndex(0)) {
        std::ostringstream s;
        s << "response specifies NoSuchName (0x02) error but error index does "
          << "not identify which oid was not found (index must be > 0)";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      auto const i(response.errorIndex_.value()-1);
      if (i >= response.values_.size()) {
        std::ostringstream s;
        s << "response specifies NoSuchName (0x02) error but error index"
          << " is beyond last oid in response";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      if (request.values_.find(response.values_[i].first)==request.values_.end()) {
        std::ostringstream s;
        s << "response error index indicates that oid "
          << response.values_[i].first << " was not found, but that oid "
          << "was not requested?";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      throw NoSuchName(response.values_[i].first,XJU_TRACED);
    }
    case 0x03:
    {
      if (response.errorIndex_==SnmpV1Response::ErrorIndex(0)) {
        std::ostringstream s;
        s << "response specifies BadValue (0x03) error but error index does "
          << "not identify which oid was not found (index must be > 0)";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      auto const i(response.errorIndex_.value()-1);
      if (i >= response.values_.size()) {
        std::ostringstream s;
        s << "response specifies BadValue (0x03) error but error index"
          << " is beyond last oid in response";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      if (request.values_.find(response.values_[i].first)==request.values_.end()) {
        std::ostringstream s;
        s << " specifies BadValue (0x03) error and response error index "
          << "indicates that oid "
          << response.values_[i].first 
          << " was not found, but that oid was not requested to be set?";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      throw BadValue(response.values_[i].first,XJU_TRACED);
    }
    case 0x04:
    {
      if (response.errorIndex_==SnmpV1Response::ErrorIndex(0)) {
        std::ostringstream s;
        s << "response specifies ReadOnly (0x04) error but error index does "
          << "not identify which oid was not found (index must be > 0)";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      auto const i(response.errorIndex_.value()-1);
      if (i >= response.values_.size()) {
        std::ostringstream s;
        s << "response specifies ReadOnly (0x04) error but error index"
          << " is beyond last oid in response";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      if (request.values_.find(response.values_[i].first)==request.values_.end()) {
        std::ostringstream s;
        s << "response error index indicates that oid "
          << response.values_[i].first << " was not found, but that oid "
          << "was not requested?";
        throw xju::Exception(s.str(),XJU_TRACED);
      }
      throw ReadOnly(response.values_[i].first,XJU_TRACED);
    }
    case 0x05:
      throw GenErr(XJU_TRACED);
    default:
    {
      std::ostringstream s;
      s << "response has unknown error status " << response.error_;
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    }
    std::set<Oid> requestOids;
    std::transform(request.values_.begin(),
                   request.values_.end(),
                   std::inserter(requestOids,requestOids.end()),
                   xju::functional::First());
    std::set<Oid> responseOids;
    std::transform(response.values_.begin(),
                   response.values_.end(),
                   std::inserter(responseOids,responseOids.end()),
                   xju::functional::First());
    std::set<Oid> missing;
    std::set_difference(requestOids.begin(),requestOids.end(),
                        responseOids.begin(),responseOids.end(),
                        std::inserter(missing,missing.end()));
    std::set<Oid> extra;
    std::set_difference(responseOids.begin(),responseOids.end(),
                        requestOids.begin(),requestOids.end(),
                        std::inserter(extra,extra.end()));
    if (missing.size()||extra.size()) {
      std::vector<std::string> errors;
      if (missing.size()) {
        errors.push_back(
          "response did not return oids "+
          xju::format::join(missing.begin(),missing.end(),", "));
      }
      if (extra.size()) {
        errors.push_back(
          "response returned unrequested oids "+
          xju::format::join(extra.begin(),extra.end(),", "));
      }
      throw xju::Exception(
        xju::format::join(errors.begin(),errors.end()," and "),XJU_TRACED);
    }
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "validate response " << response << " to SnmpV1SetRequest " << request;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

std::ostream& operator<<(std::ostream& s, 
                         SnmpV1GetNextRequest const& x) throw()
{
  return s << "community " << x.community_._ << ", id " << x.id_.value()
           << ", oids " 
           << xju::format::join(x.oids_.begin(),x.oids_.end(),", ");
}

std::vector<uint8_t> encode(SnmpV1GetNextRequest const& request) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(request.oids_.begin(),
                 request.oids_.end(),
                 std::back_inserter(params),
                 [](Oid const& oid) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(oid)),
                         vp(new NullValue)},
                       0x30));
                 });
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1
      vp(new StringValue(request.community_._)),
      vp(new Sequence({
            vp(new IntValue(request.id_.value())),
            vp(new IntValue(0)),//error
            vp(new IntValue(0)),//errorIndex
            vp(new Sequence(params,0x30))},
        0xA1))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<std::pair<Oid, std::shared_ptr<Value const> > > validateResponse(
  SnmpV1GetNextRequest const& request,
  SnmpV1Response const& response) throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    TooBig,
    GenErr,
    xju::Exception)
{
  try {
    if (request.id_ != response.id_) {
      throw ResponseIdMismatch(response.id_,request.id_,XJU_TRACED);
    }
    if (response.responseType_!=0xA2) {
      throw ResponseTypeMismatch(response.responseType_,0xA2,XJU_TRACED);
    }
    switch(response.error_) {
    case 0: break;
    case 0x01: throw TooBig(XJU_TRACED);
    case 0x02:
    {
      // translate NoSuchName to suit SnmpV1Table.add()
      // note that RFC 1157 leaves an ambiguity where traversal
      // reaches the end of the MIB - there are no subsequent
      // OIDs to return, so we end up with NoSuchName, but we can't
      // distinguish this from "requested oid not known to server"
      std::vector<std::pair<Oid, std::shared_ptr<Value const> > > result;
      std::transform(request.oids_.begin(),
                     request.oids_.end(),
                     std::inserter(result,result.end()),
                     [](Oid const& x) throw() {
                       return std::make_pair(
                         Oid(".1.3"),
                         std::shared_ptr<Value const>(
                           new NullValue));
                     });
      return result;
    }
    case 0x05:
      throw GenErr(XJU_TRACED);
    default:
    {
      std::ostringstream s;
      s << "response has unknown error status " << response.error_;
      throw xju::Exception(s.str(),XJU_TRACED);
    }
    }
    if (response.values_.size()<request.oids_.size()) {
      throw xju::Exception("response has less values than request",XJU_TRACED);
    }
    else if (response.values_.size()>request.oids_.size()) {
      throw xju::Exception("response has more values than request",XJU_TRACED);
    }
    return response.values_;
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "validate response " << response << " to SnmpV1GetNextRequest " 
      << request;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}


SnmpV1Table::SnmpV1Table(std::set<Oid> cols) throw():
cols_(cols),
              atEnd_(false)
{
  std::transform(cols.begin(),cols.end(),
                 std::inserter(data_,data_.end()),
                 [](Oid const& x) throw() {
                   return std::make_pair(x, std::vector<Cell>());
                 });
}

std::vector<SnmpV1Table::Cell> const& SnmpV1Table::operator[](Oid const& col) const throw()
{
  auto const i(data_.find(col));
  xju::assert_not_equal(i,data_.end());
  return (*i).second;
}

std::vector<Oid> SnmpV1Table::nextOids() const throw()
{
  if ((*data_.begin()).second.size()==0) {
    return std::vector<Oid>(cols_.begin(),cols_.end());
  }
  std::vector<Oid> result;
  for(auto x: data_) {
    result.push_back((*x.second.rbegin()).oid_);
  }
  xju::assert_equal(result.size(),cols_.size());
  return result;
}

void SnmpV1Table::add(
  std::vector<std::pair<Oid, std::shared_ptr<Value const> > > const& row)
throw()
{
  xju::assert_equal(row.size(),cols_.size());
  auto i = cols_.begin();
  for(auto x: row) {
    if (!(*i++).contains(x.first)) {
      atEnd_=true;
      return;
    }
  }
  i = cols_.begin();
  for(auto x: row) {
    data_[*i++].push_back(Cell(x));
  }
}

std::vector<uint8_t> encode(SnmpV1Trap const& trap) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(trap.vars_.begin(),
                 trap.vars_.end(),
                 std::back_inserter(params),
                 [](std::pair<Oid const,vp> const& x) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(x.first)),
                         x.second},
                       0x30));
                 });
  Sequence s({
      vp(new IntValue(0)), // SNMP version 1
      vp(new StringValue(trap.community_._)),
      vp(new Sequence({
            vp(new OidValue(trap.trapType_)),
            vp(new IPv4AddressValue(trap.origin_)),//error
            vp(new IntValue((int)trap.genericType_)),
            vp(new IntValue(trap.specificType_.value())),
            vp(new TimeTicksValue(trap.timestamp_)),
            vp(new Sequence(params,0x30))},
          0xA4))}, // SNMP Trap
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

}
}
