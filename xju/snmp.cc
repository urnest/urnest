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
#include "xju/format.hh"
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

class Sequence : public Value
{
public:
  Sequence(std::vector<std::shared_ptr<Value const> > const& items,
           uint8_t sequenceType) throw():
      items_(items),
      sequenceType_(sequenceType),
      dataLength_(encodedLengthOfItems(items)),
      encodedLength_(1/*type*/+
                     encodedLengthOfLength(dataLength_)+
                     dataLength_) {
  }
  uint8_t sequenceType_;
  std::vector<std::shared_ptr<Value const> > const items_;

  // Value::
  size_t encodedLength() const throw() override
  {
    return encodedLength_;
  }
  
  // Value::
  std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw() override
  {
    auto at=begin;
    *at++=sequenceType_;
    at=encodeLength(at,encodedLength_);
    for(std::shared_ptr<Value const> x: items_) {
      at=x->encodeTo(at);
    }
    xju::assert_equal(at-begin, encodedLength_);
    return at;
  }

private:
  size_t const dataLength_;
  size_t const encodedLength_;
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

}

size_t IntValue::encodedLength() const throw()
{
  int const dataLength(intDataLength(val_));
  return 1/*type*/+
    encodedLengthOfLength(dataLength)+
    dataLength;
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

size_t StringValue::encodedLength() const throw()
{
  // X.690 octetstring using primitive encoding
  uint64_t const dataLength(val_.size());
  return 1/*type*/+
    encodedLengthOfLength(dataLength)+
    dataLength;
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

size_t oidDataLength(OidValue const& oid) throw()
{
  return std::accumulate(
    oid.val_.components().begin()+2,
    oid.val_.components().end(),
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

}

size_t OidValue::encodedLength() const throw()
{
  xju::assert_less(1U, val_.components().size());
  xju::assert_equal(val_.components()[0],1);
  xju::assert_equal(val_.components()[1],3);
  uint64_t const dataLength{oidDataLength(*this)};
  return 1/*type*/+
    encodedLengthOfLength(dataLength)+
    dataLength;
}
std::vector<uint8_t>::iterator OidValue::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  auto at(begin);
  *at++=0x06;
  at=encodeLength(at,oidDataLength(*this));
  *at++=0x2b; //1.3
  for(auto i=val_.components().begin()+2;
      i!=val_.components().end();
      ++i) {
    at=encodeOidComponent(at,*i);
  }
  return at;
}

size_t NullValue::encodedLength() const throw()
{
  uint64_t const dataLength(0);
  return 1/*type*/+
    encodedLengthOfLength(dataLength)+
    dataLength;
}
std::vector<uint8_t>::iterator NullValue::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  auto at(begin);
  *at++=0x05;
  at=encodeLength(at,0);
  return at;
}

std::vector<uint8_t> encode(SnmpV1GetRequest const& request) throw();


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

