// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "SnmpV1Trap.hh"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <xju/format.hh>

namespace xju
{
namespace snmp
{

namespace
{
std::string formatVar(std::pair<Oid,std::shared_ptr<Value const> > const& v)
  throw()
{
  std::ostringstream s;
  s << v.first << "=" << (*v.second);
  return s.str();
}

}

std::ostream& operator<<(std::ostream& s, SnmpV1Trap const& x) 
  throw()
{
  std::vector<std::string> vars;
  std::transform(x.vars_.begin(),x.vars_.end(),
                 std::back_inserter(vars),
                 formatVar);
  s << "community " << xju::format::quote(x.community_.value())
    << ", trap type " << x.trapType_
    << ", origin " << x.origin_
    << ", generic type " << (int)x.genericType_
    << ", specific type " << x.specificType_
    << ", up time " << x.timestamp_.count()/10
    << ", vars: " << xju::format::join(vars.begin(),vars.end(),", ");
  return s;
}

}
}
