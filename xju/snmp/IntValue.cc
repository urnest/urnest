// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "xju/snmp/IntValue.hh"

#include "xju/format.hh"
#include "xju/snmp/encodeInt.hh"
#include "xju/snmp/encodedLengthOfValue.hh"
#include <limits.h>
#include <sstream>

namespace xju
{
namespace snmp
{

IntValue::IntValue(int64_t const& val) throw():
    Value(encodedLengthOfValue(val)),
    val_(val)
{
}

IntValue::operator int() const throw(xju::Exception)
{
  if (val_ > INT_MAX) {
    std::ostringstream s;
    s << (*this) << " is too big to convert to an integer (INT_MAX = "
      << INT_MAX << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  if (val_ < INT_MIN) {
    std::ostringstream s;
    s << (*this) << " is too small to convert to an integer (INT_MIN = "
      << INT_MIN << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return val_;
}

IntValue::operator unsigned int() const throw(xju::Exception)
{
  if (val_ > UINT_MAX) {
    std::ostringstream s;
    s << (*this) << " is too big to convert to an unsigned integer (UINT_MAX = "
      << UINT_MAX << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  if (val_ < 0) {
    std::ostringstream s;
    s << (*this) << " cannot be converted to an unsigned integer";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return val_;
}

IntValue::operator long() const throw(xju::Exception)
{
  if (val_ > LONG_MAX) {
    std::ostringstream s;
    s << (*this) << " is too big to convert to a long integer (LONG_MAX = "
      << LONG_MAX << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  if (val_ < LONG_MIN) {
    std::ostringstream s;
    s << (*this) << " is too small to convert to a long integer (LONG_MIN = "
      << LONG_MIN << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return val_;
}

IntValue::operator unsigned long() const throw(xju::Exception)
{
  if (val_ > ULONG_MAX) {
    std::ostringstream s;
    s << (*this) << " is too big to convert to an unsigned long (ULONG_MAX = "
      << ULONG_MAX << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  if (val_ < 0) {
    std::ostringstream s;
    s << (*this) << " cannot be converted to an unsigned long";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return val_;
}


std::vector<uint8_t>::iterator IntValue::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  return encodeInt(begin,0x02,val_);
}
std::string IntValue::str() const throw()
{
  return xju::format::int_(val_);
}


}
}

