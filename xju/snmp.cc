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

namespace xju
{
namespace snmp
{

// This implementation is based on the information in:
//   https://en.wikipedia.org/wiki/X.690#BER_encoding
//   http://www.rane.com/note161.html
//   RFC 1157
//   http://www.itu.int/ITU-T/studygroups/com17/languages/X.690-0207.pdf
//

namespace
{
// parse dotted assumed to be an SNMP OID in dotted notation, eg .1.3.6.1.4.364
std::vector<uint32_t> parseDottedOid(std::string const& dotted) throw(
  xju::Exception)
{
  try {
    if (dotted.size()==0 || (*dotted.begin())!= '.') {
      std::ostringstream s;
      s << xju::format::quote(dotted) << " does not start with '.'";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    std::vector<uint32_t> components;
    std::string::const_iterator i(dotted.begin());
    while(i!=dotted.end()) {
      ++i;
      std::string::const_iterator j=std::find(i, dotted.end(), '.');
      std::string const octet(i,j);
      try {
        unsigned int o(xju::stringToUInt(octet));
        components.push_back(o);
        i=j;
      }
      catch(xju::Exception& e) {
        std::ostringstream s;
        s << "parse component " << xju::format::quote(octet)
          << " (at offset " << (i-dotted.begin()) << ")";
        e.addContext(s.str(), XJU_TRACED);
        throw;
      }
    }
    return components;
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "parse " << xju::format::quote(dotted) 
      << " assumed to be an SNMP OID in dotted notation, eg .1.3.6.1.4.364";
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

}

Oid::Oid(std::string const& dotted) throw(
  xju::Exception):
    components_(parseDottedOid(dotted))
{
}

std::string Oid::toString() const throw()
{
  std::ostringstream s;
  for(std::vector<uint32_t>::const_iterator i=components_.begin();
      i != components_.end();
      ++i) {
    s << "." << (*i);
  }
  return s.str();
}

Oid operator+(Oid const& a, Oid const& b) throw()
{
  Oid result(a);
  std::copy(b.components_.begin(), b.components_.end(),
            std::back_inserter(result.components_));
  return result;
}

namespace
{

uint64_t encodedLengthOfLength(uint64_t length) throw()
{
  if (length < 128) {
    // X.690 Definite Short Form length encoding 
    // (single byte, top bit 0, length in lowest 7 bits)
    return 1;
  }
  // X.690 Definite Long Form length encoding
  // (first byte has top bit set and number of following length bytes
  //  is in lowest 7 bits; subsequent byes comprise the length, most
  //  significant byte first)
  return 1+(countSignificantBits(length)+7)/8;
}

std::vector<uint8_t>::iterator encodeLength(
  std::vector<uint8_t>::iterator const begin,
  size_t length) throw()
{
  auto at(begin);
  if (length < 128) {
    // X.690 Definite Short Form length encoding (described above)
    *at++=length;
    return at;
  }
  // X.690 Definite Long Form length encoding (described above)
  int const lengthBytes=(countSignificantBits(length)+7)/8;
  *at++=0x80|lengthBytes; // how many bytes
  at+=lengthBytes;
  std::vector<uint8_t>::reverse_iterator r(at);
  for(int i=0; i!=lengthBytes; ++i) {
    *r++=length&0xff;
    length>>8;
  }
  return at;
}

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
};

size_t intDataLength(int64_t val_) throw()
{
  int dataLength=8;
  if (val_>0) {
    // for positive numbers, remove leading byte while have 9 leading 0 bits
    int64_t mask=0xff80000000000000;
    while(dataLength>1 && ((val_ & ~mask)==val_)) {
      --dataLength;
      mask = (mask >> 8) | 0xff00000000000000;
    }
  }
  else if (val_<0)
  {
    // for negative numbers, remove leading byte while have 9 leading 1 bits
    int64_t mask=0xff80000000000000;
    while(dataLength>1 && ((val_ & mask)==mask)) {
      --dataLength;
      mask = (mask >> 8) | 0xff00000000000000;
    }
  }
  else
  {
    dataLength=1; // 0 is encoded using a single 0 byte
  }
  return dataLength;
}

size_t encodedLengthOfValue(int64_t val_) throw()
{
  int const dataLength(intDataLength(val_));
  return 1/*type*/+
    encodedLengthOfLength(dataLength)+
    dataLength;
}

size_t encodedLengthOfValue(std::string const& val_) throw()
{
  // X.690 octetstring using primitive encoding
  uint64_t const dataLength(val_.size());
  return 1/*type*/+
    encodedLengthOfLength(dataLength)+
    dataLength;
}

}

IntValue::IntValue(int64_t const& val) throw():
    Value(encodedLengthOfValue(val)),
    val_(val)
{
}

std::vector<uint8_t>::iterator IntValue::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  auto at(begin);
  *at++=0x02;
  auto const dataLength=intDataLength(val_);
  at=encodeLength(at, dataLength);
  for(int i=dataLength; i!=0; --i) {
    uint8_t b=(val_ >> ((i-1)*8)) & 0xff;
    *at++=b;
  }
  return at;
}
std::string IntValue::str() const throw() override
{
  return xju::format::int_(val_);
}


StringValue::StringValue(std::string const& val) throw():
    Value(encodedLengthOfValue(val)),
    val_(val)
{
}

std::vector<uint8_t>::iterator StringValue::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  // X.690 octetstring using primitive encoding
  auto at(begin);
  *at++=0x04;
  at=encodeLength(at,val_.size());
  std::copy(val_.begin(),val_.end(),at);
  return at+val_.size();
}
std::string StringValue::str() const throw() override
{
  return xju::format::quote(xju::format::cEscapeString(val_));
}

namespace
{
uint64_t encodedLengthOfOidComponent(uint32_t const c) throw()
{
  int bits=xju::countSignificantBits(c);
  // encoded in groups of 7 bits, but c==0 uses one byte
  // bits     length
  // 0        1
  // <8       1
  // <15      2
  if(c==0) {
    return 1;
  }
  return (bits-1)/7+1;
}

size_t oidDataLength(Oid const& oid) throw()
{
  return std::accumulate(
    oid.components().begin()+2,
    oid.components().end(),
    uint64_t{1U},/*for 1.3 encoded as 0x2B*/
    [](uint64_t t, uint32_t c)
    {
      return t+encodedLengthOfOidComponent(c);
    });
}
std::vector<uint8_t>::iterator encodeOidComponent(
  std::vector<uint8_t>::iterator const begin,
  uint32_t c) throw()
{
  auto at(begin);
  int byteCount=encodedLengthOfOidComponent(c);
  at+=byteCount;
  std::vector<uint8_t>::reverse_iterator r(at);
  for(int i=0; i != byteCount; ++i) {
    *r++=0x80|(c&0x7f);
    c=c>>7;
  }
  *(at-1) &= ~0x80; // clear top bit of last byte
  return at;
}

size_t encodedLengthOfValue(Oid const& val_) throw()
{
  xju::assert_less(1U, val_.components().size());
  xju::assert_equal(val_.components()[0],1);
  xju::assert_equal(val_.components()[1],3);
  uint64_t const dataLength{oidDataLength(val_)};
  return 1/*type*/+
    encodedLengthOfLength(dataLength)+
    dataLength;
}
}

OidValue::OidValue(Oid const& val) throw():
    Value(encodedLengthOfValue(val)),
    val_(val)
{
}

std::vector<uint8_t>::iterator OidValue::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  auto at(begin);
  *at++=0x06;
  at=encodeLength(at,oidDataLength(val_));
  *at++=0x2b; //1.3
  for(auto i=val_.components().begin()+2;
      i!=val_.components().end();
      ++i) {
    at=encodeOidComponent(at,*i);
  }
  return at;
}

std::vector<uint8_t>::iterator NullValue::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  auto at(begin);
  *at++=0x05;
  at=encodeLength(at,0);
  return at;
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

namespace
{
class DecodeIterator
{
public:
  explicit DecodeIterator(std::vector<uint8_t> const& data) throw():
      data_(data),
      at_(data.begin()) {
  }
  uint8_t const& operator*() const
  {
    if (atEnd()) {
      std::ostringstream s;
      s << "end of data at offset " << data_.size();
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    return *at_;
  }
  DecodeIterator operator++(int) throw(xju::Exception)
  {
    DecodeIterator result(*this);
    operator++();
    return result;
  }
  DecodeIterator& operator++() throw(xju::Exception)
  {
    if (atEnd()) {
      std::ostringstream s;
      s << "end of data at offset " << data_.size();
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    ++at_;
    return *this;
  }
  bool atEnd() const throw()
  {
    return at_==data_.end();
  }
  std::vector<uint8_t> const& data_;
  std::vector<uint8_t>::const_iterator at_;

  friend std::ostream& operator<<(std::ostream& s, DecodeIterator i) throw()
  {
    return s << "at offset " << (at_-data_.begin());
  }
};

// result.first is length, if not valid, length is X.690 indefinite form
// result.second is just after decoded data
std::pair<xju::Optional<size_t>,DecodeIterator> decodeLength(
  DecodeIterator const at) throw(xju::Exception)
{
  try {
    if ((*at)<0x80) {
      // X.690 definite short encoding
      return std::make_pair(xju::Optional<size_t>(*at),at+1);
    }
    else if ((*at)==0x80)
    {
      // X.690 indefinite form
      return std::make_pair(xju::Optional<size_t>(),at+1);
    }
    else
    {
      // X.690 definite long form
      uint64_t length=0;
      int const byteCount(*at);
      if (byteCount > 8) {
        std::ostringstream s;
        s << "can only handle 8-byte lengths, not " << byteCount;
        throw xju::Exception(s.str(), XJU_TRACED);
      }
      for(auto i(at+1); i!=(at+1+byteCount); ++i) {
        length|=((uint64_t)*at++)<<(8*(at+byteCount-i));
      }
      return std::make_pair(xju::Optional<size_t>(length),at+1);
    }
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "decode length at " << at;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

// result.first.first is sequence type
// result.first.second is length, if not valid, length is X.690 indefinite form
// result.second is just after decoded data
std::pair<std::pair<uint8_t,xju::Optional<size_t> >, DecodeIterator>
decodeSequenceTypeAndLength(DecodeIterator const& at) throw(
  xju::Exception)
{
  try {
    uint8_t const sequenceType=*at++;
    auto length=decodeLength(at);
    return std::make_pair(std::make_pair(sequenceType,length.first),
                          length.second);
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "decode sequence type and length at " << at;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

std::pair<IntValue,DecodeIterator> decodeIntValue(DecodeIterator const at)
  throw(xju::Exception)
{
  try {
    if ((*at) != 0x02) {
      std::ostringstream s;
      s << "type is " << xju::format::hex(at) << " not 0x02";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    auto length(decodeLength(at+1));
    if (!length.first.valid) {
      std::ostringstream s;
      s << "integers with indefinite length are illegal";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    if (length.first.value()>8) {
      std::ostringstream s;
      s << "can only handle 8-byte integers, not " << length.first.value();
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    uint64_t result=0;
    if ((*length.second)&0x80) {
      // negative number
      result=~result;
    }
    for(auto i=length.second; i!=length.second+length.first.value(); ++i) {
      result&=~(((uint64_t)0xff)<<(length.second+length.first.value()-i-1));
      result|=((uint64_t)*i)<<(length.second+length.first.value()-i-1);
    }
    return std::make_pair(result,i);
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "decode integer at " << at;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

}

  
SnmpV1Response decodeSnmpV1Response(std::vector<uint8_t> const& data) throw(
    // malformed
    xju::Exception)
{
  std::vector<std::string> ok;
  try {
    DecodeIterator const start(data);
    auto const s1(decodeSequenceTypeAndLength(start));
    if (s1.first.first!=0x30) {
      std::ostringstream s;
      s << "expected sequence type byte 0x30, got " 
        << xju::format::hex(s1.first.first)
        << " at offset 0";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    try {
      auto const snmpVersion(decodeIntValue(s1.second));
      if (snmpVersion.first!=0) {
        std::ostringstream s;
        s << "expected version 0 (SNMP V1), got " 
          << xju::format::hex(version.first)
          << " at " << s1l.second;
        throw xju::Exception(s.str(), XJU_TRACED);
      }
      try {
        auto const community(decodeStringValue(snmpVersion.second)); try {
          auto const s2(decodeSequenceTypeAndLength(community.second)); try {
            auto const id(decodeIntValue(s2.second)); try {
              auto const e(decodeIntValue(id.second)); try {
                auto const ei(decodeIntValue(e.second)); try {
                  auto const s3(decodeSequenceTypeAndLength(ei.second)); try {
                    DecodeIterator at(s3.second);
                    std::vector<
                      std::pair<Oid, std::shared_ptr<Value const> > > values;
                    auto atEnd=[&]() {
                      if (s3.first.second.valid())
                      {
                       // X.690 definite length
                       return at == s3.second.value()+s3.fist.second.value();
                      }
                      else
                      {
                       // X.690 indefinite length
                        return (((*at)==0) && ((*at+1)==0)); 
                      }
                    };
                    while(!atEnd()) {
                      try {
                        auto const s(decodeSequenceTypeAndLength(at.second));
                        auto const oid(decodeOid(s.second));
                        auto const value(decodeValue(oid.second));
                        values.emplace_back(
                          std::make_pair(oid.first,value.first));
                        at=value.second;
                      }
                      catch(xju::Exception& e) {
                        std::transform(values.begin(),values.end(),
                                       std::back_inserter(ok),
                                       [](decltype(values)::value_type x) {
                                         std::ostringstream s;
                                         s << x.first << ": " << (*x.second);
                                         return s.str();
                                       });
                        std::ostringstream s;
                        s << "decode param oid and value sequence at offset " 
                          << at;
                        e.addContext(s.str(), XJU_TRACED);
                        throw;
                      }
                    }
                    return SnmpV1Response(
                      SnmpV1Response::ResponseType(s2.first),
                      Community(community.first),
                      RequestId(id.first),
                      (ErrorStatus)e.first,
                      ErrorIndex(ei.first),
                      values);
                  }
                  catch(xju::Exception& e) {
                    std::ostringstream s;
                    s << "3rd sequence type " 
                      << xju::format::hex(s3.first.first)
                      << " and length " << s3.first.second
                      << " at " << ei.second;
                    ok.push_back(s.str());
                    throw;
                  }
                }
                catch(xju::Exception& e) {
                  std::ostringstream s;
                  s << "error index " << ei.first
                    << " at " << e.second;
                  ok.push_back(s.str());
                  throw;
                }
              }
              catch(xju::Exception& e) {
                std::ostringstream s;
                s << "error " << xju::format::hex(e.first)
                  << " at " << id.second;
                ok.push_back(s.str());
                throw;
              }
            }
            catch(xju::Exception& e) {
              std::ostringstream s;
              s << "request id " << id.first << " at " << s2.second;
              ok.push_back(s.str());
              throw;
            }
          }
          catch(xju::Exception& e) {
            std::ostringstream s;
            s << "2nd sequence type " << xju::format::hex(s2.first.first)
              << " and length " << s2.first.second
              << " bytes at " << snmpVersion.second;
            ok.push_back(s.str());
            throw;
          }
        }
        catch(xju::Exception& e) {
          std::ostringstream s;
          s << "community " << xju::format::quote(community.first)
            << " at " << snmpVersion.second;
          ok.push_back(s.str());
          throw;
        }
      }
      catch(xju::Exception& e) {
        std::ostringstream s;
        s << "snmp version 1 at " << s1.second;
        ok.push_back(s.str());
        throw;
      }
    }
    catch(xju::Exception& e) {
      std::ostringstream s;
      s << "sequence type 0x30 and length " << s1.first.second << " bytes";
      ok.push_back(s.str());
      throw;
    }
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "decode snmp v1 response from " << data.size() << " bytes of data";
    if (ok.size()) {
      s << ", having successfully decoded "
        << xju::format::join(ok.rbegin(),ok.rend(),", ");
    }
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}


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
  SnmpV1Response::ResponseType const got,
  SnmpV1Response::ResponseType const expected,
  xju::Traced const& trace) throw():
    xju::Exception("expected response of type "+xju::format::hex(expected)+
                   " but response of type "+xju::format::hex(got),
                   trace),
    got_(got),
    expected_(expected) 
{
}
ResponseIdMismatch::ResponseIdMismatch(RequestId const got,
                                       RequestId const expected,
                                       xju::Traced const& trace) throw():
    xju::Exception("expected message with id "+xju::format::str(expected)+
                   " but got message of id "+xju::format::str(got),
                   trace),
    got_(got),
    expected_(expected) 
{
}

}
}

