// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "Counter32Value.hh"

#include "xju/snmp/encodedLengthOfValue.hh"
#include "xju/snmp/encodeInt.hh"
#include "xju/format.hh"

namespace xju
{
namespace snmp
{

Counter32Value::Counter32Value(uint32_t val) throw():
    Value(encodedLengthOfValue((int64_t)val)),
    val_(val) {
  }

int Counter32Value::intValue() const throw(xju::Exception)
{
  if (val_ > INT_MAX) {
    std::ostringstream s;
    s << (*this) << " is too big to convert to an integer (INT_MAX = "
      << INT_MAX << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return val_;
}

unsigned int Counter32Value::uintValue() const throw()
{
  return val_;
}

long Counter32Value::longValue() const throw(xju::Exception)
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

unsigned long Counter32Value::ulongValue() const throw()
{
  return val_;
}



std::vector<uint8_t>::iterator Counter32Value::encodeTo(
  std::vector<uint8_t>::iterator begin) const throw()
{
  return encodeInt(begin,0x41,val_);
}

std::string Counter32Value::str() const throw()
{
  return xju::format::str(val_);
}

bool Counter32Value::less(Value const& y) const throw()
{
  return val_ < dynamic_cast<Counter32Value const&>(y).val_;
}

}
}

