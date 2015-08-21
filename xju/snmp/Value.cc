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
  StringValue const* v(dynamic_cast<StringValue const*>(this));
  if (!v) {
    std::ostringstream s;
    s << (*this) << " is not a string";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return v->val_;
}
Value::operator int() const throw(xju::Exception)
{
  IntValue const* v(dynamic_cast<IntValue const*>(this));
  if (!v) {
    std::ostringstream s;
    s << (*this) << " is not an integer";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  if (v->val_ > INT_MAX) {
    std::ostringstream s;
    s << (*this) << " is too big to convert to an integer (INT_MAX = "
      << INT_MAX << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  if (v->val_ < INT_MIN) {
    std::ostringstream s;
    s << (*this) << " is too small to convert to an integer (INT_MIN = "
      << INT_MIN << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return v->val_;
}
Value::operator unsigned int() const throw(xju::Exception)
{
  IntValue const* v(dynamic_cast<IntValue const*>(this));
  if (!v) {
    std::ostringstream s;
    s << (*this) << " is not an integer";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  if (v->val_ > UINT_MAX) {
    std::ostringstream s;
    s << (*this) << " is too big to convert to an unsigned integer (UINT_MAX = "
      << UINT_MAX << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  if (v->val_ < 0) {
    std::ostringstream s;
    s << (*this) << " cannot be converted to an unsigned integer";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return v->val_;
}
Value::operator long() const throw(xju::Exception)
{
  IntValue const* v(dynamic_cast<IntValue const*>(this));
  if (!v) {
    std::ostringstream s;
    s << (*this) << " is not an integer";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  if (v->val_ > LONG_MAX) {
    std::ostringstream s;
    s << (*this) << " is too big to convert to a long integer (LONG_MAX = "
      << LONG_MAX << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  if (v->val_ < LONG_MIN) {
    std::ostringstream s;
    s << (*this) << " is too small to convert to a long integer (LONG_MIN = "
      << LONG_MIN << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return v->val_;
}
Value::operator unsigned long() const throw(xju::Exception)
{
  IntValue const* v(dynamic_cast<IntValue const*>(this));
  if (!v) {
    std::ostringstream s;
    s << (*this) << " is not an integer";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  if (v->val_ > ULONG_MAX) {
    std::ostringstream s;
    s << (*this) << " is too big to convert to an unsigned long (ULONG_MAX = "
      << ULONG_MAX << ")";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  if (v->val_ < 0) {
    std::ostringstream s;
    s << (*this) << " cannot be converted to an unsigned long";
    throw xju::Exception(s.str(),XJU_TRACED);
  }
  return v->val_;
}



}
}

