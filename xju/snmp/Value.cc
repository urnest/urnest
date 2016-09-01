// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "Value.hh"

#include <sstream>
#include <limits.h>

namespace xju
{
namespace snmp
{


Value::operator std::string() const throw(xju::Exception)
{
  std::ostringstream s;
  s << (*this) << " is not a string";
  throw xju::Exception(s.str(),XJU_TRACED);
}
Value::operator int() const throw(xju::Exception)
{
  std::ostringstream s;
  s << (*this) << " is not an integer";
  throw xju::Exception(s.str(),XJU_TRACED);
}
Value::operator unsigned int() const throw(xju::Exception)
{
  std::ostringstream s;
  s << (*this) << " is not an integer";
  throw xju::Exception(s.str(),XJU_TRACED);
}
Value::operator long() const throw(xju::Exception)
{
  std::ostringstream s;
  s << (*this) << " is not an integer";
  throw xju::Exception(s.str(),XJU_TRACED);
}
Value::operator unsigned long() const throw(xju::Exception)
{
  std::ostringstream s;
  s << (*this) << " is not an integer";
  throw xju::Exception(s.str(),XJU_TRACED);
}
Value::operator Oid() const throw(xju::Exception)
{
  std::ostringstream s;
  s << (*this) << " is not an Oid";
  throw xju::Exception(s.str(),XJU_TRACED);
}



}
}

