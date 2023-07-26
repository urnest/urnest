// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/snmp/SnmpV2cVarResponse.hh"
#include "xju/format.hh"
#include "xju/snmp/OidValue.hh"
#include "xju/snmp/encodedLengthOfLength.hh"
#include "xju/snmp/encodeLength.hh"
#include "xju/assert.hh"

namespace xju
{
namespace snmp
{
SnmpV2cVarResponse::NoSuchObject::NoSuchObject(
  Oid const& oid, const xju::Traced& trace) throw():
xju::Exception("no such object " + xju::format::str(oid), trace)
{
}
SnmpV2cVarResponse::NoSuchInstance::NoSuchInstance(
  Oid const& oid, const xju::Traced& trace) throw():
xju::Exception("no such instance " + xju::format::str(oid), trace)
{
}
SnmpV2cVarResponse::EndOfMibView::EndOfMibView(
  Oid const& oid, const xju::Traced& trace) throw():
xju::Exception("end of MIB view " + xju::format::str(oid), trace)
{
}
xju::snmp::Value const& SnmpV2cVarResponse::operator*() const /*throw(
  NoSuchObject,
  NoSuchInstance,
  EndOfMibView)*/
{
  if (e_.get()) {
    if (dynamic_cast<NoSuchObject const*>(e_.get())) {
      throw dynamic_cast<NoSuchObject const&>(*e_);
    }
    if (dynamic_cast<NoSuchInstance const*>(e_.get())) {
      throw dynamic_cast<NoSuchInstance const&>(*e_);
    }
    throw dynamic_cast<EndOfMibView const&>(*e_);
  }
  return *v_;
}

xju::snmp::Value const* SnmpV2cVarResponse::operator->() const /*throw(
  NoSuchObject,
  NoSuchInstance,
  EndOfMibView)*/
{
  if (e_.get()) {
    if (dynamic_cast<NoSuchObject const*>(e_.get())) {
      throw dynamic_cast<NoSuchObject const&>(*e_);
    }
    if (dynamic_cast<NoSuchInstance const*>(e_.get())) {
      throw dynamic_cast<NoSuchInstance const&>(*e_);
    }
    throw dynamic_cast<EndOfMibView const&>(*e_);
  }
  return v_.operator->();
}

std::shared_ptr<xju::snmp::Value const> SnmpV2cVarResponse::value() const
  /*throw(
    NoSuchObject,
    NoSuchInstance,
    EndOfMibView)*/
{
  if (e_.get()) {
    if (dynamic_cast<NoSuchObject const*>(e_.get())) {
      throw dynamic_cast<NoSuchObject const&>(*e_);
    }
    if (dynamic_cast<NoSuchInstance const*>(e_.get())) {
      throw dynamic_cast<NoSuchInstance const&>(*e_);
    }
    throw dynamic_cast<EndOfMibView const&>(*e_);
  }
  return v_;
}


size_t SnmpV2cVarResponse::encodedLength() const throw()
{
  size_t contentLength(OidValue(oid_).encodedLength()+
                       (e_.get()?
                        1+1: // type+length of encoded exception
                        v_->encodedLength()));
  return 1+//type
    encodedLengthOfLength(contentLength)+
    contentLength;
}

std::vector<uint8_t>::iterator SnmpV2cVarResponse::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  // encoding is a sequence with 2 elements:
  //   oid
  //   value
  auto at=begin;
  *at++=0x30; // sequence
  size_t contentLength(OidValue(oid_).encodedLength()+
                       (e_.get()?
                        1+1: // type+length of encoded exception
                        v_->encodedLength()));
  at=encodeLength(at,contentLength);
  at=OidValue(oid_).encodeTo(at);
  if (e_.get()) {
    if(dynamic_cast<NoSuchObject const*>(e_.get())) {
      *at++=0x80; // type
    }
    else if (dynamic_cast<NoSuchInstance const*>(e_.get()))
    {
      *at++=0x81; // type
    }
    else if (dynamic_cast<EndOfMibView const*>(e_.get()))
    {
      *at++=0x82; // type
    }
    else
    {
      // not handled
      xju::assert_not_equal(readableRepr(*e_),readableRepr(*e_));
    }
    *at++=0; // length
  }
  else
  {
    at=v_->encodeTo(at);
  }
  return at;
}


std::ostream& operator<<(std::ostream& s, 
                         SnmpV2cVarResponse const& x) throw() {
  if (x.e_.get()) {
    s << x.oid_ << ": " << readableRepr(*x.e_);
  }
  else
  {
    s << x.oid_ << ": " << (*x.v_);
  }
  return s;
}
               

}
}

