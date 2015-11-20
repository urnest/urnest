// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "SnmpVersionMismatch.hh"
#include <xju/format.hh>

namespace xju
{
namespace snmp
{

namespace
{
std::string versionString(uint8_t const x) throw()
{
  switch(x) {
  case 0: return "0 (SNMP V1)";
  case 1: return "1 (SNMP V2)";
  }
  return xju::format::hex(x);
}
}

SnmpVersionMismatch::SnmpVersionMismatch(
  uint8_t const got,
  uint8_t const expected,
  xju::Traced const& trace) throw():
    xju::Exception("expected snmp version "+versionString(expected)+
                   " but got "+versionString(got),
                   trace)
{
}

}
}
