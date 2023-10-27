// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/encodePDU.hh>
#include <algorithm>
#include <xju/snmp/OidValue.hh>
#include <xju/assert.hh>
#include <cinttypes>
#include <xju/Int.hh>
#include <unistd.h>
#include <xju/snmp/Value.hh>
#include <xju/format.hh>
#include <xju/seq_less.hh>
#include <xju/snmp/encodedLengthOfLength.hh>
#include <xju/snmp/encodeLength.hh>
#include <xju/snmp/IntValue.hh>
#include <xju/snmp/StringValue.hh>
#include <xju/snmp/Sequence.hh>

namespace xju
{
namespace snmp
{
namespace
{

uint64_t encodedLengthOfItems(
  std::vector<SnmpV2cVarResponse> const& items) throw()
{
  uint64_t result(0);
  for(auto x:items){
    result=result+x.encodedLength();
  }
  return result;
}

class V2cSequence : private xju::Int<V2cSequence,size_t>, //encoded length of items
                    public Value
{
public:
  V2cSequence(std::vector<SnmpV2cVarResponse> const& items,
              uint8_t sequenceType) throw():
      xju::Int<V2cSequence,size_t>(encodedLengthOfItems(items)),
      Value(1/*type*/+
            encodedLengthOfLength(value())+
            value()),
      items_(items),
      sequenceType_(sequenceType) {
  }
  std::vector<SnmpV2cVarResponse> const items_;
  uint8_t const sequenceType_;

  // Value::
  std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw() override
  {
    auto at=begin;
    *at++=sequenceType_;
    at=encodeLength(at,value());
    for(SnmpV2cVarResponse x: items_) {
      at=x.encodeTo(at);
    }
    xju::assert_equal(at-begin, encodedLength_);
    return at;
  }
  // Value::
  std::string str() const throw() override
  {
    return xju::format::join(items_.begin(),items_.end(),
                             [](SnmpV2cVarResponse const& x) {
                               return xju::format::str(x);
                             },", ");
  }
  // Value::
  bool less(Value const& yy) const throw() override
  {
    auto const y{dynamic_cast<V2cSequence const&>(yy)};
    return xju::seq_less(items_.begin(),items_.end(),
                         y.items_.begin(),y.items_.end());
  }
  
};


}

typedef std::shared_ptr<Value const> vp;

std::vector<uint8_t> encodePDU(
  int snmpVersion, //0 == SNMP version 1
  Community const& community,
  RequestId requestId,
  uint64_t error,
  uint64_t errorIndex,
  std::vector<std::pair<Oid, vp> > const& vars,
  uint8_t pduType // 0xA2 snmp v1 get etc
  ) throw()
{
  std::vector<vp > params;
  std::transform(vars.begin(),
                 vars.end(),
                 std::back_inserter(params),
                 [](std::pair<Oid,vp> const& var) {
                   return vp(
                     new Sequence({
                         vp(new OidValue(var.first)),
                           vp(var.second)
                           },
                       0x30));
                 });
                       
  Sequence s({
      vp(new IntValue(snmpVersion)),
      vp(new StringValue(std::vector<uint8_t>(community._.begin(),
                                              community._.end()))),
      vp(new Sequence({
            vp(new IntValue(requestId.value())),
            vp(new IntValue(error)),//error
            vp(new IntValue(errorIndex)),//errorIndex
            vp(new Sequence(params,0x30))},
        pduType))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

std::vector<uint8_t> encodePDU(
  Community const& community,
  RequestId requestId,
  uint64_t error,
  uint64_t errorIndex,
  std::vector<SnmpV2cVarResponse> const& vars,
  uint8_t pduType // 0xA2 snmp v1 get etc
  ) throw()
{
  Sequence s({
      vp(new IntValue(1)), //SNMP V2c
      vp(new StringValue(std::vector<uint8_t>(community._.begin(),
                                              community._.end()))),
      vp(new Sequence({
            vp(new IntValue(requestId.value())),
            vp(new IntValue(error)),//error
            vp(new IntValue(errorIndex)),//errorIndex
            vp(new V2cSequence(vars,0x30))},
        pduType))},
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}

}
}

