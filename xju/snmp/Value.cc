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


std::vector<uint8_t>const& Value::stringValue() const /*throw(xju::Exception)*/
{
  std::ostringstream s;
  s << (*this) << " is not a string";
  throw xju::Exception(s.str(),XJU_TRACED);
}
std::vector<uint8_t>const& Value::opaqueValue() const /*throw(xju::Exception)*/
{
  std::ostringstream s;
  s << (*this) << " is not a opaque value";
  throw xju::Exception(s.str(),XJU_TRACED);
}
int Value::intValue() const /*throw(xju::Exception)*/
{
  std::ostringstream s;
  s << (*this) << " is not an integer";
  throw xju::Exception(s.str(),XJU_TRACED);
}
unsigned int Value::uintValue() const /*throw(xju::Exception)*/
{
  std::ostringstream s;
  s << (*this) << " is not an integer";
  throw xju::Exception(s.str(),XJU_TRACED);
}
long Value::longValue() const /*throw(xju::Exception)*/
{
  std::ostringstream s;
  s << (*this) << " is not an integer";
  throw xju::Exception(s.str(),XJU_TRACED);
}
unsigned long Value::ulongValue() const /*throw(xju::Exception)*/
{
  std::ostringstream s;
  s << (*this) << " is not an integer";
  throw xju::Exception(s.str(),XJU_TRACED);
}
Oid Value::oidValue() const /*throw(xju::Exception)*/
{
  std::ostringstream s;
  s << (*this) << " is not an Oid";
  throw xju::Exception(s.str(),XJU_TRACED);
}
std::chrono::milliseconds Value::timeTicksValue() const /*throw(
    xju::Exception)*/
{
  std::ostringstream s;
  s << (*this) << " is not an TimeTicks";
  throw xju::Exception(s.str(),XJU_TRACED);
}



}
}

