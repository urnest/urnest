// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_INVALIDPARAM_H
#define XJU_SNMP_INVALIDPARAM_H
#include "xju/Exception.hh"
#include <string>
#include "xju/snmp/Oid.hh"

namespace xju
{
namespace snmp
{

// exception corresponding to several RFC 1157 error-statuses
class InvalidParam : public xju::Exception
{
public:
  InvalidParam(std::string const& cause, Oid const& param, const xju::Traced& trace) throw();
  Oid param_;
};

}
}

#endif

