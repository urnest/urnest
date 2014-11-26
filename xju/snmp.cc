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

namespace xju
{
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

}

