//     -*- mode: c++ ; c-file-style: "xju" ; -*-
//
// Copyright (c) 1997-2007 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/format.hh>
#include <sstream>
#include <xju/isnan.hh>
#include <ctype.h>
#include <iomanip>
#include <iterator>
#include <algorithm>

namespace xju
{
namespace format
{
std::string char_(const char c) throw()
{
  std::ostringstream s;
  s << int_(c);
  if (isalnum(c))
  {
    s << "('" << c << "')";
  }
  return s.str();
}

std::string float_(const float x, 
                   const ios_base::fmtflags format,
                   const int precision) throw()
{
  if (xju_isnan(x))
  {
    return "nan";
  }
  std::ostringstream s;
  s.setf(format, std::ios::floatfield);
  s.precision(precision);
  s << x;
  return s.str();
}

std::string float_(const double x, 
                   const ios_base::fmtflags format,
                   const int precision) throw()
{
  if (xju_isnan(x))
  {
    return "nan";
  }
  std::ostringstream s;
  s.setf(format, std::ios::floatfield);
  s.precision(precision);
  s << x;
  return s.str();
}

std::string quote(const std::string& x) throw()
{
  return quote("\"", x);
}

std::string quote(const std::string& quote,
                  const std::string& x) throw()
{
  return quote + x + quote;
}

std::string quote(const std::string& pre,
                  const std::string& post,
                  const std::string& x) throw()
{
  return pre + x + post;
}
std::string hex(char x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << (unsigned short)(unsigned char)x; //avoid printing as character
  return s.str();
}
std::string hex(signed char x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << (unsigned short)(unsigned char)x; //avoid printing as character
  return s.str();
}

std::string hex(unsigned char x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << (unsigned short)x; //avoid printing as character
  return s.str();
}
std::string hex(short x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << (unsigned short)x; //avoid printing a sign
  return s.str();
}
std::string hex(unsigned short x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << x;
  return s.str();
}
std::string hex(int x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << (unsigned int)x; //avoid printing a sign
  return s.str();
}
std::string hex(unsigned int x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << x;
  return s.str();
}
std::string hex(long x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << (unsigned long)x; //avoid printing a sign
  return s.str();
}
std::string hex(unsigned long x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << x;
  return s.str();
}
std::string hex(long long x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << (unsigned long long)x; //avoid printing a sign
  return s.str();
}
std::string hex(unsigned long long x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0')
    << x;
  return s.str();
}

// see format.hh for explanation
#define XJU__IS_AN_ABOVE_TYPE(UX_MAX) (\
    ((UX_MAX) - UCHAR_MAX == 0) ||     \
    ((UX_MAX) - USHRT_MAX == 0) ||     \
    ((UX_MAX) - UINT_MAX == 0) ||      \
    ((UX_MAX) - ULONG_MAX == 0) ||     \
    ((UX_MAX) - ULLONG_MAX == 0))

#if !XJU__IS_AN_ABOVE_TYPE(UINT16_MAX)
std::string hex(int16_t x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << (uint16_t)x; //avoid printing a sign
  return s.str();
}
std::string hex(uint16_t x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << x;
  return s.str();
}
#endif

#if !XJU__IS_AN_ABOVE_TYPE(UINT32_MAX)
std::string hex(int32_t x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << (uint32_t)x; //avoid printing a sign
  return s.str();
}
std::string hex(uint32_t x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << x;
  return s.str();
}
#endif

#if !XJU__IS_AN_ABOVE_TYPE(UINT64_MAX)
std::string hex(int64_t x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << (uint64_t)x; //avoid printing a sign
  return s.str();
}
std::string hex(uint64_t x, const std::string& leader) 
  throw(std::bad_alloc)
{
  std::ostringstream s;
  s << leader
    << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') 
    << x;
  return s.str();
}
#endif

#undef XJU__IS_AN_ABOVE_TYPE


std::string cEscapeChar(char const c) throw()
{
  switch(c) {
  case '\'':
    return "\\'";
  case '"':
    return "\\\"";
  case '\\':
    return "\\\\";
  case '\a':
    return "\\a";
  case '\b':
    return "\\b";
  case '\f':
    return "\\f";
  case '\n':
    return "\\n";
  case '\r':
    return "\\r";
  case '\t':
    return "\\t";
  case '\v':
    return "\\v";
  default:
    if (::isprint(c))
    {
      return str(c);
    }
    return hex(c);
  }
}

std::string cEscapeString(std::string const& s) throw()
{
  std::ostringstream result;
  std::transform(s.begin(), s.end(),
                 std::ostream_iterator<std::string>(result),
                 cEscapeChar);
  return result.str();
}

namespace
{
class IndentAfterNewline
{
public:
  explicit IndentAfterNewline(std::string const& indent) throw():
      indent_(indent)
  {
  }
  std::string const indent_;
  std::string operator()(char const c) throw()
  {
    switch(c) {
    case '\n':
      return "\n"+indent_;
    }
    return str(c);
  }
};
}

std::string indent(std::string const& s, std::string const& prefix) throw()
{
  std::ostringstream result;
  std::transform(s.begin(), s.end(),
                 std::ostream_iterator<std::string>(result),
                 IndentAfterNewline(prefix));
  return result.str();
}


}
}
