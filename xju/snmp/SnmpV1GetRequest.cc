// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "SnmpV1GetRequest.hh"
#include <xju/format.hh>
#include <algorithm>

namespace xju
{
namespace snmp
{

std::ostream& operator<<(std::ostream& s, SnmpV1GetRequest const& x) throw()
{
  return s << "community " << x.community_._ << ", id " << x.id_.value()
           << ", oids " 
           << xju::format::join(x.oids_.begin(),x.oids_.end(),", ");
}

std::vector<uint8_t> encode(SnmpV1GetRequest const& request) throw()
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
        0xA0))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}


}
}

