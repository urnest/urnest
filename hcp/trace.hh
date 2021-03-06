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
#ifndef HCP_TRACE_HH_
#define HCP_TRACE_HH_

#include <string>
#include "xju/Traced.hh"

namespace hcp_trace
{
class Scope
{
public:
  std::string message_;
  xju::Traced t_;
  bool failed_;
  bool cached_;
  std::string result_;
  
  void fail() throw(){ failed_=true; }
  void cached() throw(){ cached_=true; }
  void result(std::string&& r) throw(){
    result_=r;
  }
  Scope(std::string const& message, xju::Traced const& t) throw();
  
  ~Scope() throw();
};

void milestone(std::string const& message, xju::Traced const& t) throw();

}

#endif
