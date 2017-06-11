#ifndef _XJU_IO_SELECT_HH
#define _XJU_IO_SELECT_HH
// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <utility>
#include <set>
#include <chrono>
#include <new>

#include "xju/syscall.hh"

namespace xju
{
namespace io
{
class Input;
class Output;

// wait at most until deadline for one of the specified inputs or outputs
// to be readable/writable; return as soon as one is, returning all that are
// - returns empty sets when deadline is reached
// - returns those inputs/outputs that are already readable/writable if
//   deadline has already passed
// post: result.first.size()||result.second.size()||deadline reached
std::pair<std::set<Input const*>,std::set<Output const*> > select(
  std::set<Input const*> const& inputs,
  std::set<Output const*> const& outputs,
  std::chrono::system_clock::time_point const& deadline) throw(
    std::bad_alloc);


// as above but without any outputs
// 
std::set<Input const* > select(
  std::set<Input const* > const& inputs,
  std::chrono::system_clock::time_point const& deadline) throw(
    xju::SyscallFailed);


// as above but without any inputs
// 
std::set<Output const* > select(
  std::set<Output const* > const& outputs,
  std::chrono::system_clock::time_point const& deadline) throw(
    std::bad_alloc);



}
}
#endif
