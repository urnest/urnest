//     -*- c-file-style: "xju" ; -*-
//
// Copyright (c) 1997-2008 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    Functions to format various types of objects into 
//    human-readable strings.
//
//  RATIONALE
//
//    It is frequently necessary to format object values and sequence
//    values as human-readable text strings (for example when 
//    generating log messages). The standard library provides minimal
//    assistance, and experience has shown that the utilities in
//    this module simplify such formatting.
//
//    The utilities do not necessarily produce unambiguous representations
//    and so must be clearly identified as formatting text
//    for human consumption, so they are in a separate namespace
//    (xju::format).
//
//    Sets of values are very common, so we provide a convenient
//    way to format them (see set()).
//
//    Formatting of arbitrary types is based on the stream-output operator
//    (i.e. operator<<(ostream&, T)) since that is the C++ way, and
//    such operators are supplied by the compiler for the basic types.
//
//    We avoid the default operator<< for bool, which typically produces
//    "0" and "1", we produce "false" and "true" which are clearer.
//
//    The formatting utilities never raise exceptions, reflecting that
//    all valid values should have a human readable representation (if
//    any do). This also simplifies use.
//
// HOW TO...
//
//    ... format an integer? See int_(), class Int and class IntU
//    ...                    hex()
//    ... format a float?    See float_(), double_(), Float and Double
//    ... format your type T? See Str<T>, str<T>()
//    ... quote a string? See quote(), class Quote
//    ... format a set of values? See set(), join()
// 
//    Also see test-format.cc for various working examples.
//
// NOTES
//
//    [1] ios_base::scientific and ios_base::fixed are defined but
//        there's no specific value for "neither of these" (which is
//        the default). Alas I doubt that ios_base:fmtflags(0) is
//        portable.
//
#ifndef _XJU_FORMAT_HH_
#define _XJU_FORMAT_HH_

#include <cstdint>
#include <limits.h>

#include <iostream>
#include <functional>
#include <algorithm>
#include <xju/JoiningIterator.hh>
#include <string>
#include <sstream>
#include <xju/ios_base.hh>
#include <cinttypes>
#include <climits>
#include <chrono>
#include <time.h>

namespace xju
{
namespace format
{
//
// Format a character, giving the ascii code and if c is
// alphanumeric the print representation of c.
//
// post: result contains no newlines.
//
std::string char_(const char c) throw();


//
// Format an integer.
//
// These functions produce frequently used formats
// (e.g. fixed width, zero padded), typically with
// simpler code than using an ostream.
//
// Example:
//
//    xju::assert_equal(xju::format::int_(24, width=4), "0024")
//
template<class I>
std::string int_(I const x,
                 int const width = 0,
                 char const fill = '0',
                 ios_base::fmtflags align = std::ios::right,
                 ios_base::fmtflags base = std::ios::dec) throw();

template<>
inline std::string int_(char const x,
                        int const width,
                        char const fill,
                        ios_base::fmtflags align,
                        ios_base::fmtflags base) throw()
{
  return int_((int)x, width, fill, align, base);
}

template<>
inline std::string int_(unsigned char const x,
                        int const width,
                        char const fill,
                        ios_base::fmtflags align,
                        ios_base::fmtflags base) throw()
{
  return int_((unsigned int)x, width, fill, align, base);
}


template<>
inline std::string int_(signed char const x,
                        int const width,
                        char const fill,
                        ios_base::fmtflags align,
                        ios_base::fmtflags base) throw()
{
  return int_((int)x, width, fill, align, base);
}


//
// Function object to call int_.
//
// Example:
//
//    vector<int> values;
//    xju::format::set(values.begin(), 
//                     values.end(),
//                     xju::format::Int(/*width*/4));
//
struct Int
{
public:
  explicit Int(int width = 0, 
               char fill = '0',
               ios_base::fmtflags align = std::ios::right,
               ios_base::fmtflags base = std::ios::dec) throw():
    _width(width),
    _fill(fill),
    _align(align),
    _base(base)
  {
  }
  std::string operator()(const long long x) const throw()
  {
    return int_(x, _width, _fill, _align, _base);
  }
private:
  int _width;
  int _fill;
  ios_base::fmtflags _align;
  ios_base::fmtflags _base;
};

//
// function object to call int_ (for unsigned value type).
//
struct IntU
{
public:
  explicit IntU(
    int width = 0, 
    char fill = '0',
    ios_base::fmtflags align = std::ios::right,
    ios_base::fmtflags base = std::ios::dec) throw():
    
    _width(width),
    _fill(fill),
    _align(align),
    _base(base)
  {
  }
  std::string operator()(const unsigned long long x) const throw()
  {
    return int_(x, _width, _fill, _align, _base);
  }
private:
  int _width;
  int _fill;
  ios_base::fmtflags _align;
  ios_base::fmtflags _base;
};


//
// Format a real number.
//
// These functions produce frequently used formats
// (e.g. fixed with 3 decimal places), typically with
// simpler code than using an ostream.
//
// Example:
//
//    xju::assert_equal(xju::format::float_(12.34), "12.34");
//    xju::assert_equal(xju::format::float_(12.34, 0L, 3), "12.3");
//    xju::assert_equal(xju::format::float_(12.34, ios::scientific),
//                      "1.234000e+01");
//    xju::assert_equal(xju::format::float_(12.34, ios::fixed, 4),
//                      "12.3400");
//    xju::assert_equal(xju::format::float_(sqrt(-1.0), "nan");
//
std::string float_(
  const float x, 
  const ios_base::fmtflags format = ios_base::fmtflags(0), // [1]
  const int precision = 6) throw();
std::string float_(
  const double x, 
  const ios_base::fmtflags format = ios_base::fmtflags(0), // [1]
  const int precision = 6) throw();

//
// Function objects that call float_ (for floats and
// doubles respectively).
//
class Float
{
public:
  explicit Float(
    const ios_base::fmtflags format = ios_base::fmtflags(0), // [1]
    const int precision = 6) throw():
    _format(format),
    _precision(precision)
  {
  }
  std::string operator()(const float x) const throw()
  {
    return float_(x, _format, _precision);
  }
private:
  ios_base::fmtflags _format;
  int _precision;
};
class Double
{
public:
  explicit Double(const ios_base::fmtflags format = ios_base::fmtflags(0), // [1]
                  const int precision = 6) throw():
    _format(format),
    _precision(precision)
  {
  }
  std::string operator()(const double x) const throw()
  {
    return float_(x, _format, _precision);
  }
private:
  ios_base::fmtflags _format;
  int _precision;
};


//
// Conversion to std::string.
//
// Use:
//
//    For user-defined class T, either have
//    to implement ostream& ::operator<<(ostream&, T) or
//    provide a specialisation of 
//    string Str<T>::operator(const T&) const throw()
//
//    Note that classes with virtual destructors (i.e.
//    designed to be extended), should implement their
//    operator<<(ostream&, T) by calling a virtual function,
//    virtual string T::format() const throw() for example,
//    to allow sub-classes to supply their own implementations.
//
template<class T>
class Str
{
public:
  std::string operator()(const T& x) const throw()
  {
    std::ostringstream s;
    s << x;
    return s.str();
  }
};
//
// Convenience function that calls Str<T>()(x).
//
template<class T>
std::string str(const T& x) throw();

//
// C char-literal string of c
// eg cEscapeChar('\n') == std::string("\\n")
//
std::string cEscapeChar(char const c) throw();

//
// C string-literal string of s
// eg cEscapeString("fred\njock")==std::string("fred\\njock")
//
std::string cEscapeString(std::string const& s) throw();

//
// Add specified prefix to each line of x (but not at start)
//
std::string indent(std::string const& s, std::string const& prefix) throw();

// time as seconds.usecs since unix epoch
std::string time(std::chrono::system_clock::time_point const& t) throw();

// format like as 0.000s
std::string duration(std::chrono::milliseconds const& d) noexcept;

// format like as 0.000000s
std::string duration(std::chrono::microseconds const& d) noexcept;

// format like as 0.000000000s
std::string duration(std::chrono::nanoseconds const& d) noexcept;

// format like as 0s
std::string duration(std::chrono::seconds const& d) noexcept;

// format like as 0m
std::string duration(std::chrono::minutes const& d) noexcept;

// format like as 0h
std::string duration(std::chrono::hours const& d) noexcept;

// format time t as specified
// e.g.
//   cout << localTime(now,Hour,':',Minute,':',Second);
//
template<class Formatter,class ... Formatters>
std::string localTime(
  std::chrono::system_clock::time_point const& t,
  Formatter a,
  Formatters... b) throw();
template<class Formatter,class ... Formatters>
std::string gmTime(
  std::chrono::system_clock::time_point const& t,
  Formatter a,
  Formatters... b) throw();

class YYYY_{};class MM_{};class DD_{};
class Year_{};class Month_{};class Day_{};
class DayName_{};class DayName3_{};
class Hour_{};class Hour12_{};class Minute_{};class Second_{};
class ampm_{};class AMPM_{};
class Millisecond_{};class Microsecond_{};class Nanosecond_{};

// time format selectors for use with localtime, gmtime above
extern YYYY_ const YYYY;
extern MM_ const MM;
extern DD_ const DD;
extern Year_ const Year;
extern Month_ const Month;
extern Day_ const Day;
extern DayName_ const DayName;
extern DayName3_ const DayName3;
extern Hour_ const Hour;
extern Minute_ const Minute;
extern Second_ const Second;
extern Hour12_ const Hour12;
extern ampm_ const ampm;
extern AMPM_ const AMPM;
extern Millisecond_ const Millisecond;
extern Microsecond_ const Microsecond;
extern Nanosecond_ const Nanosecond;
}
}

namespace xju
{
namespace format
{
// default operator<<() for bool prints 1 or 0, we want true or false
template<>
inline std::string Str<bool>::operator()(const bool& x) const throw()
{
  if (x)
  {
    return "true";
  }
  else
  {
    return "false";
  }
}

// put double quotes around x
std::string quote(const std::string& x) throw();

// put specified quote std::string before and after x
std::string quote(const std::string& quote,
                  const std::string& x) throw();

// put quotes around x. Default double quotes.
std::string quote(const std::string& pre,
                  const std::string& post,
                  const std::string& x) throw();

class Quote
{
public:
  Quote():	_pre("\""),	_post("\"") {}
  Quote(const std::string& quote): _pre(quote), _post(quote) {}
  Quote(const std::string& pre, const std::string& post):
    _pre(pre), _post(post) {}
  
  std::string operator()(const std::string& x) const throw()
  {
    return quote(_pre, _post, x);
  }
  
private:
  std::string _pre;
  std::string _post;
};



//
// Convert objects to strings, using xju::format::str() by
// default, and join those string by placing a specified 
// joiner between each.
//
// for one object, the result is the object converted to a string
// for no input strings, the result is the empty string
//
// Examples:
//
//   vector<int> x;
//   assert_equal(join(x.begin(), x.end(), itoa, ", "), "")
//   
//   x.push_back(2);
//   assert_equal(join(x.begin(), x.end(), itoa, ", "), "2")
//   
//   x.push_back(5);
//   assert_equal(join(x.begin(), x.end(), itoa, ", "), "2, 5")
//   
template<class ConstIterator, class ConvertFunction>
std::string join(const ConstIterator begin, 
                 const ConstIterator end,
                 ConvertFunction converter,
                 const std::string& joiner) throw();

//
// as above, but use Str<T> as converter.
//
// Example:
//
//    class X;
//    ostream& operator<<(ostream& s, const X& x) throw();
//    vector<X> values = ...;
//    std::ostringstream s;
//    s << xju::format::join(values.begin(),
//                           values.end(),
//                           ", ");
//                          
//    vector<xju::Time> times;
//    xju::assert_equal(join(times.begin(), times.end(), ", "), "");
//
//    times.push_back(xju::Time(1, 4));
//    xju::assert_equal(join(times.begin(), times.end(), ", "),
//                      "0.000004");
//          
//    times.push_back(xju::Time(93, 76));
//    xju::assert_equal(join(times.begin(), times.end(), ", "),
//                      "0.000004, 93.000076");
//          
template<class ConstIterator>
std::string join(const ConstIterator begin, 
                 const ConstIterator end,
                 const std::string& joiner) throw();

//
// Produce a human readable, single line set representation, using
// c to format each element, e.g.
//
//    {  }    -- the empty set
//    { a, b, c } -- set contains a, b and c
//
template<class ConstIterator, class ConvertFunction>
std::string set(const ConstIterator begin,
                const ConstIterator end,
                ConvertFunction c) throw();
//
// as above, using xju::format::str() as the conversion function
//
template<class ConstIterator>
std::string set(const ConstIterator begin,
                const ConstIterator end) throw();

}
}


// implementation of template classes and functions

namespace xju
{
namespace format
{
template<class T>
std::string str(const T& x) throw()
{
  return Str<T>()(x);
}
// REVISIT
// 	template<class T, class U>
//         std::string Str<std::pair<T, U> >::operator()(
//             const std::pair<T, U>& x) const throw();

// 	template<class T, class U>
//         std::string Str<std::pair<T, U> >::operator()(
//             const std::pair<T, U>& x) const throw()
//         {
//             return Str<T>()(x.first) + ", " + Str<U>()(x.second);
//         }

//
// Return a formatter for type T.
//
template<class T>
xju::format::Str<T> makeConverter(const T&) throw()
{
  return xju::format::Str<T>();
}


template<class ConstIterator, class ConvertFunction>
std::string join(const ConstIterator begin, 
                 const ConstIterator end,
                 ConvertFunction c,
                 const std::string& joiner) throw()
{
  std::ostringstream s;
  std::transform(
    begin,
    end,
    xju::JoiningIterator<std::string, std::string>(s, joiner),
    c);
  return s.str();
}

template<class ConstIterator>
std::string join(const ConstIterator begin, 
                 const ConstIterator end,
                 const std::string& joiner) throw()
{
  if (begin == end)
  {
    return std::string();
  }
  return join(begin,
              end,
              makeConverter(*begin),
              joiner);
}


template<class ConstIterator, class ConvertFunction>
std::string set(const ConstIterator begin,
                const ConstIterator end,
                ConvertFunction c) throw()
{
  std::ostringstream s;
  s << "{ " << join(begin, end, c, ", ") << " }";
  return s.str();
}

template<class ConstIterator>
std::string set(const ConstIterator begin,
                const ConstIterator end) throw()
{
  if (begin == end)
  {
    return std::string("{  }");
  }
  return xju::format::set(begin,
                          end,
                          makeConverter(*begin));
}

// format as fixed width hexadecimal with leading '0x'
// post: result.size()==leader.size()+sizeof(x)*2
std::string hex(char x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string hex(signed char x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string hex(unsigned char x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string hex(short x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string hex(unsigned short x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string hex(int x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string hex(unsigned int x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string hex(long x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string hex(unsigned long x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string hex(long long x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string hex(unsigned long long x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;

// convenient for use in join()
struct Hex{
  std::string leader_;
  explicit Hex(std::string leader="0x") noexcept:leader_(std::move(leader)){}
  template<class T> std::string operator()(T const& x){return hex(x,leader_);}
};

// format as fixed width octal with leading "0"
std::string octal(char x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
std::string octal(signed char x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
std::string octal(unsigned char x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
std::string octal(short x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
std::string octal(unsigned short x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
std::string octal(int x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
std::string octal(unsigned int x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
std::string octal(long x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
std::string octal(unsigned long x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
std::string octal(long long x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
std::string octal(unsigned long long x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;

// convenient for use in join()
struct Octal{
  std::string leader_;
  explicit Octal(std::string leader="0") noexcept:leader_(std::move(leader)){}
  template<class T> std::string operator()(T const& x){return octal(x,leader_);}
};

// we can't define a hex function where the type of x is a typedef
// of a type that we've done above, but the above does not necessarily
// cover all int types (eg a pure 32bit compiler v int64_t)
#define XJU__IS_AN_ABOVE_TYPE(UX_MAX) (\
    ((UX_MAX) - UCHAR_MAX == 0) ||     \
    ((UX_MAX) - USHRT_MAX == 0) ||     \
    ((UX_MAX) - UINT_MAX == 0) ||      \
    ((UX_MAX) - ULONG_MAX == 0) ||     \
    ((UX_MAX) - ULLONG_MAX == 0))

#if !XJU__IS_AN_ABOVE_TYPE(UINT16_MAX)
std::string hex(int16_t x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string hex(uint16_t x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string octal(int16_t x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
std::string octal(uint16_t x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
#endif

#if !XJU__IS_AN_ABOVE_TYPE(UINT32_MAX)
std::string hex(int32_t x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string hex(uint32_t x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string octal(int32_t x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
std::string octal(uint32_t x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
#endif

#if !XJU__IS_AN_ABOVE_TYPE(UINT64_MAX)
std::string hex(int64_t x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string hex(uint64_t x, const std::string& leader = "0x") 
  /*throw(std::bad_alloc)*/;
std::string octal(int64_t x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
std::string octal(uint64_t x, const std::string& leader = "0") 
  /*throw(std::bad_alloc)*/;
#endif

#undef XJU__IS_AN_ABOVE_TYPE

}
}

// implementations
namespace xju
{
namespace format
{
template<class I>
std::string int_(I const x, 
                 int width, 
                 char fill,
                 ios_base::fmtflags align,
                 ios_base::fmtflags base) throw()
{
  std::ostringstream s;
  s.setf(align, ios_base::adjustfield);
  s.fill(fill);
  s.width(width);
  s.setf(base, ios_base::basefield);
  s << x;
  return s.str();
}

template<class ...Formatters>
std::string formatTm(struct tm const& x,std::chrono::nanoseconds) throw(){
  return std::string();
}
template<class ... Formatters>
std::string formatTm(struct tm const& x,std::chrono::nanoseconds n,
                     YYYY_, Formatters... bs) throw(){
  return format::int_(x.tm_year+1900,4)+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     MM_, Formatters... bs) throw(){
  return format::int_(x.tm_mon+1,2)+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     DD_, Formatters... bs) throw(){
  return format::int_(x.tm_mday,2)+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x,std::chrono::nanoseconds n,
                     Year_, Formatters... bs) throw(){
  return format::int_(x.tm_year+1900,4)+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     Month_, Formatters... bs) throw(){
  return format::int_(x.tm_mon+1)+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     Day_, Formatters... bs) throw(){
  return format::int_(x.tm_mday)+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     DayName_, Formatters... bs) throw(){
  static const char* dayNames[]={
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"};
  return std::string(
    (x.tm_wday>=0&&x.tm_wday<=6)?dayNames[x.tm_wday]:"???")+
    formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     DayName3_, Formatters... bs) throw(){
  static const char* dayNames[]={
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat"};
  return std::string(
    (x.tm_wday>=0&&x.tm_wday<=6)?dayNames[x.tm_wday]:"???")+
    formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     Hour_, Formatters... bs) throw(){
  return format::int_(x.tm_hour,2)+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     Hour12_, Formatters... bs) throw(){
  int h;
  switch(x.tm_hour){
  case 0: h=12; break;
  case 12: h=12; break;
  default:
    h=(x.tm_hour%12);
  }
  return format::int_(h)+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     ampm_, Formatters... bs) throw(){
  return std::string(x.tm_hour>=12?"pm":"am")+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     AMPM_, Formatters... bs) throw(){
  return std::string(x.tm_hour>=12?"PM":"AM")+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     Minute_, Formatters... bs) throw(){
  return format::int_(x.tm_min,2)+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     Second_, Formatters... bs) throw(){
  return format::int_(x.tm_sec,2)+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     Millisecond_, Formatters... bs) throw(){
  return format::int_((n.count()/1000000)%1000,3)+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     Microsecond_, Formatters... bs) throw(){
  return format::int_((n.count()/1000)%1000000,6)+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     Nanosecond_, Formatters... bs) throw(){
  return format::int_(n.count(),9)+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x, std::chrono::nanoseconds n,
                     char c, Formatters... bs) throw(){
  return std::string(1U,c)+formatTm(x,n,bs...);
}
template<class ... Formatters>
std::string formatTm(struct tm const& x,
                     std::chrono::nanoseconds n,
                     std::string const& s,
                     Formatters... bs) throw(){
  return s+formatTm(x,n,bs...);
}
template<class Formatter,class ... Formatters>
std::string localTime(
  std::chrono::system_clock::time_point const& x,
  Formatter a,
  Formatters... b) throw()
{
  time_t xt(std::chrono::system_clock::to_time_t(x));
  struct tm xx;
  ::localtime_r(&xt,&xx);
  auto const seconds(std::chrono::duration_cast<std::chrono::seconds>(
                       x.time_since_epoch()));
  auto const nanoseconds(std::chrono::duration_cast<std::chrono::nanoseconds>(
                           x.time_since_epoch()-seconds));
  return formatTm(xx,nanoseconds,a,b...);
}
template<class Formatter,class ... Formatters>
std::string gmTime(
  std::chrono::system_clock::time_point const& x,
  Formatter a,
  Formatters... b) throw()
{
  time_t xt(std::chrono::system_clock::to_time_t(x));
  struct tm xx;
  ::gmtime_r(&xt,&xx);
  auto const seconds(std::chrono::duration_cast<std::chrono::seconds>(
                       x.time_since_epoch()));
  auto const nanoseconds(std::chrono::duration_cast<std::chrono::nanoseconds>(
                           x.time_since_epoch()-seconds));
  return formatTm(xx,nanoseconds,a,b...);
}

}
}

#endif
