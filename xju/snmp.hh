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

namespace xju
{
namespace snmp
{

// The intended use of this module for snmp-get is:
//
//   SnmpV1GetRequest request(...);
//   std::vector<uint8_t> requestData(encode(request));
//   ... send requestData to server
//   ... receive responseData as std::vector<uint8_t>
//   std::map<Oid, std::shared_ptr<Value> > values(
//     validateResponse(request,decodeSnmpV1Response(responseData)));
//   ... use values
//
// ... and for snmp-set is:
//
//   SnmpV1SetRequest request(...);
//   std::vector<uint8_t> requestData(encode(request));
//   ... send requestData to server
//   ... receive responseData as std::vector<uint8_t>
//   validateResponse(request,decodeSnmpV1Response(responseData));
//


// RFC 1157 OID
class Oid
{
public:
  // construct from dotted, eg .1.3.6.1.4.364
  explicit Oid(std::string const& dotted) throw(
    xju::Exception);
  explicit Oid(std::vector<uint32_t> const& components) throw():
      components_(components)
  {
  }
  std::string toString() const throw();
  std::vector<uint32_t> const& components() const throw()
  {
    return components_;
  }
  
private:
  std::vector<uint32_t> components_;

  friend bool operator==(Oid const& x, Oid const& y) throw()
  {
    return x.components_ == y.components_;
  }
  friend bool operator!=(Oid const& x, Oid const& y) throw()
  {
    return x.components_ != y.components_;
  }
  friend bool operator<(Oid const& x, Oid const& y) throw()
  {
    return x.components_ < y.components_;
  }
  friend Oid operator+(Oid const& a, Oid const& b) throw();
};

// RFC 1157 request-id
class RequestIdTag{};
typedef xju::Int<uint64_t> RequestId;

// RFC 1157 error-index
// note error-index of first oid is 1, not 0
class ErrorIndexTag{};
typedef xju::Int<uint64_t> ErrorIndex;

// RFC 1157 community
class CommunityTag{};
typedef xju::Tagged<std::string,CommunityTag> Community;
  
struct SnmpV1GetRequest
{
  SnmpV1GetRequest(Community const& community,
                   RequestId const id,
                   std::set<Oid> const& oids) throw():
      community_(community),
      id_(id),
      oids_(oids) {
  }
  Community community_;
  RequestId id_;
  std::set<Oid> oids_;
};
  
std::vector<uint8_t> encode(SnmpV1GetRequest const& request) throw();

class Value
{
public:
  virtual ~Value() throw(){}

  explicit Value(size_t encodedLength) throw():
      encodedLength_(encodedLength)
  {
  }
  
  // encodeTo(x)-x
  size_t const encodedLength_;
  
  // convenience functions that do type and range checking
  operator std::string() const throw(xju::Exception);
  operator int() const throw(xju::Exception);
  operator unsigned int() const throw(xju::Exception);
  operator long() const throw(xju::Exception);
  operator unsigned long() const throw(xju::Exception);

  // return length of encoded value
  // ie return encodeTo(x)-x
  size_t encodedLength() const throw()
  {
    return encodedLength_;
  }

  // encode at begin, returning end of encoding
  virtual std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw()=0;

  // human readable
  virtual std::string str() const throw()=0;
  
  friend std::ostream& operator<<(std::ostream& s, Value const& x) throw()
  {
    return s << x.str();
  }
};

class IntValue : public Value
{
public:
  ~IntValue() throw(){}
  
  explicit IntValue(int64_t const& val) throw();
  
  int64_t const val_;

  // Value::
  std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw() override;

  // Value::
  virtual std::string str() const throw() override;
};

class StringValue : public Value
{
public:
  ~StringValue() throw(){}
  
  explicit StringValue(std::string const& val) throw();
  std::string const val_;

  // Value::
  std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw() override;

  // Value::
  virtual std::string str() const throw() override;
};

class OidValue : public Value
{
public:
  ~OidValue() throw(){}
  
  explicit OidValue(Oid const& val) throw();
  Oid const val_;

  // Value::
  std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw() override;

  // Value::
  virtual std::string str() const throw() override
  {
    return val_.toString();
  }
};

class NullValue : public Value
{
public:
  ~NullValue() throw(){}
  
  explicit NullValue() throw():
      Value(2)
  {
  }

  // Value::
  std::vector<uint8_t>::iterator encodeTo(
    std::vector<uint8_t>::iterator begin) const throw() override;

  // Value::
  virtual std::string str() const throw() override
  {
    return "null";
  }
};

struct SnmpV1Response
{
  // RFC 1157 error-status
  enum ErrorStatus
  {
    NO_ERROR,
    TOO_BIG,
    NO_SUCH_NAME,
    BAD_VALUE,
    READ_ONLY,
    GEN_ERR
  };
  
  SnmpV1Response(
    uint8_t responseType,
    Community community,
    RequestId id,
    ErrorStatus error,
    ErrorIndex errorIndex,
    std::vector<std::pair<Oid, std::shared_ptr<Value const> > > values)
      throw():
      responseType_(responseType),
      community_(community),
      id_(id),
      error_(error),
      errorIndex_(errorIndex),
      values_(values) {
  }
  uint8_t responseType_;
  Community community_;
  RequestId id_;
  ErrorStatus error_;
  // 1-based ie bad oid is values_[errorIndex_-1]; 
  // (0 if error_ is a non-param-specific error)
  ErrorIndex errorIndex_;
  
  std::vector<std::pair<Oid, std::shared_ptr<Value const> > > values_;
};

SnmpV1Response decodeSnmpV1Response(std::vector<uint8_t> const& data) throw(
    // malformed
    xju::Exception);

// exceptions corresponding to RFC 1157 error-status
class GenErr : public xju::Exception
{
public:
  explicit GenErr(const xju::Traced& trace) throw();
};
class TooBig : public xju::Exception
{
public:
  explicit TooBig(const xju::Traced& trace) throw();
};
class InvalidParam : public xju::Exception
{
public:
  InvalidParam(std::string const& cause, Oid const& param, const xju::Traced& trace) throw();
  Oid param_;
};
class NoSuchName : public InvalidParam
{
public:
  NoSuchName(Oid const& param, const xju::Traced& trace) throw();
};
class BadValue : public InvalidParam
{
public:
  BadValue(Oid const& param, const xju::Traced& trace) throw();
};
class ReadOnly : public InvalidParam
{
public:
  ReadOnly(Oid const& param, const xju::Traced& trace) throw();
};

// other exceptions
class ResponseTypeMismatch : public xju::Exception
{
public:
  ResponseTypeMismatch(uint8_t const got,
                       uint8_t const expected,
                       xju::Traced const& trace) throw();
  uint8_t got_;
  uint8_t expected_;
};
class ResponseIdMismatch : public xju::Exception
{
public:
  ResponseIdMismatch(RequestId const got,
                     RequestId const expected,
                     xju::Traced const& trace) throw();

  RequestId got_;
  RequestId expected_;
};
  

// validate reponse to specified request
// - returns the requested values
std::map<Oid, std::shared_ptr<Value const> > validateResponse(
  SnmpV1GetRequest const& request,
  SnmpV1Response const& response) throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    NoSuchName,
    TooBig,
    GenErr);

struct SnmpV1SetRequest
{
  Community community_;
  RequestId id_;
  std::map<Oid, std::shared_ptr<Value const> > values_;
};

std::vector<uint8_t> encode(SnmpV1SetRequest const& request) throw();

// validate reponse to specified request
void validateResponse(
  SnmpV1SetRequest const& request, 
  SnmpV1Response const& response) throw(
    ResponseTypeMismatch,
    ResponseIdMismatch,
    NoSuchName,
    BadValue,
    ReadOnly,
    TooBig,
    GenErr);

}
}

#endif
