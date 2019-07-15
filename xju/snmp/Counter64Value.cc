// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "Counter64Value.hh"

#include "xju/snmp/encodedLengthOfValue.hh"
#include "xju/snmp/encodeInt.hh"
#include "xju/format.hh"

namespace xju
{
namespace snmp
{

Counter64Value::Counter64Value(uint64_t val) throw():
    Value(encodedLengthOfValue(val)),
    val_(val) {
  }

int Counter64Value::intValue() const throw(xju::Exception)
{
  if (val_ > INT_MAX) {
    std::ostringstream s;
    s << (*this) << " is too big to convert to an integer (INT_MAX = "
      << INT_MAX << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return val_;
}

unsigned int Counter64Value::uintValue() const throw(xju::Exception)
{
  if (val_ > UINT_MAX) {
    std::ostringstream s;
    s << (*this)
      << " is too big to convert to an unsigned integer (UINT_MAX = "
      << UINT_MAX << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return val_;
}

long Counter64Value::longValue() const throw(xju::Exception)
{
  if (val_ > LONG_MAX) {
    std::ostringstream s;
    s << (*this) << " is too big to convert to a long integer (LONG_MAX = "
      << LONG_MAX << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return val_;
}

unsigned long Counter64Value::ulongValue() const throw(xju::Exception)
{
  if (val_ > ULONG_MAX) {
    std::ostringstream s;
    s << (*this)
      << " is too big to convert to an unsigned long integer (ULONG_MAX = "
      << ULONG_MAX << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return val_;
}



std::vector<uint8_t>::iterator Counter64Value::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  return encodeInt(begin,0x46,val_);
}

std::string Counter64Value::str() const throw()
{
  return xju::format::str(val_);
}

bool Counter64Value::less(Value const& y) const throw()
{
  return val_ < dynamic_cast<Counter64Value const&>(y).val_;
}

}
}

