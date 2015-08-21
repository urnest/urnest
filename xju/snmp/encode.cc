// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "encode.hh"

#include <xju/countSignificantBits.hh>

namespace xju
{
namespace snmp
{

namespace
{

uint64_t encodedLengthOfItems(
  std::vector<std::shared_ptr<Value const> > const& items) throw()
{
  return std::accumulate(
    items.begin(),
    items.end(),
    uint64_t{0},
    [](uint64_t t, std::shared_ptr<Value const> x){
      return t+x->encodedLength();});
}

class Sequence : private Int<Sequence,size_t>, //encoded length of items
                 public Value
{
public:
  Sequence(std::vector<std::shared_ptr<Value const> > const& items,
           uint8_t sequenceType) throw():
      Int<Sequence,size_t>(encodedLengthOfItems(items)),
      Value(1/*type*/+
            encodedLengthOfLength(value())+
            value()),
      items_(items),
      sequenceType_(sequenceType) {
  }
  std::vector<std::shared_ptr<Value const> > const items_;
  uint8_t const sequenceType_;

  // Value::
  std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw() override
  {
    auto at=begin;
    *at++=sequenceType_;
    at=encodeLength(at,value());
    for(std::shared_ptr<Value const> x: items_) {
      at=x->encodeTo(at);
    }
    xju::assert_equal(at-begin, encodedLength_);
    return at;
  }
  // Value::
  std::string str() const throw() override
  {
    return xju::format::join(items_.begin(),items_.end(),
                             [](std::shared_ptr<Value const> x) {
                               return x->str();
                             },", ");
  }
  
};

}

std::vector<uint8_t> encode(SnmpV1SetRequest const& request) throw()
{
  typedef std::shared_ptr<Value const> vp;
  
  std::vector<vp > params;
  std::transform(request.values_.begin(),
                 request.values_.end(),
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
      vp(new StringValue(request.community_._)),
      vp(new Sequence({
            vp(new IntValue(request.id_.value())),
            vp(new IntValue(0)),//error
            vp(new IntValue(0)),//errorIndex
            vp(new Sequence(params,0x30))},
          0xA3))}, // SNMP Get
    0x30);
  std::vector<uint8_t> result(s.encodedLength());
  xju::assert_equal(s.encodeTo(result.begin()),result.end());
  return result;
}


}
}

