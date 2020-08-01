//     -*- mode: c++ ; c-file-style: "xju" ; -*-
//
// Copyright (c) 2008
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/trace.hh>

#include <iostream>
#include <exception>
#include <xju/format.hh>

namespace hcp_trace
{
Scope::Scope(std::string const& message, xju::Traced const& t) throw():
  message_(message),
  t_(t),
  failed_(false),
  cached_(false)
{
  std::cout << "[  " << message_ << " " << t_.file() << ":" << t_.line()
            << std::endl;
}

Scope::~Scope() throw()
{
  std::cout << "]"
            << (cached_?"C":" ")
    //g++ bug: c++-14 mode should have both std::uncaught_exception
    //and uncaught_exceptions but only has the former (c++-17 only has latter)
#if __cplusplus > 201402L
            << ((std::uncaught_exceptions()||failed_)?"*":" ")
#else
            << ((std::uncaught_exception()||failed_)?"*":" ")
#endif
            << message_ << " "
            << (failed_?"":xju::format::quote(
                  xju::format::cEscapeString(result_))+" ")
            << t_.file() << ":" << t_.line()
            << std::endl;
}

void milestone(std::string const& message, xju::Traced const& t) throw()
{
  std::cout << "= " << message << " " << t.file() << ":" << t.line()
            << std::endl;
}


}
