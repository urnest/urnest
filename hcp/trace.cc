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
static const char rcsid[] = "$RCSfile: twt_cxx.el,v $ $Revision: 1.2 $";
#include <hcp/trace.hh>

#include <iostream>
#include <exception>

namespace hcp_trace
{
Scope::Scope(std::string const& message, xju::Traced const& t) throw():
  message_(message),
  t_(t),
  failed_(false)
{
  std::cout << "[ " << message_ << " " << t_.file() << ":" << t_.line()
            << std::endl;
}

Scope::~Scope() throw()
{
  std::cout << "]" << ((std::uncaught_exception()||failed_)?"*":" ")
            << message_ << " " << t_.file() << ":" << t_.line()
            << std::endl;
}

void milestone(std::string const& message, xju::Traced const& t) throw()
{
  std::cout << "= " << message << " " << t.file() << ":" << t.line()
            << std::endl;
}


}
