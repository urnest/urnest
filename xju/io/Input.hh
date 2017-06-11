#ifndef _XJU_IO_INPUT_HH
#define _XJU_IO_INPUT_HH
// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <set>
#include <chrono>
#include <string>
#include "xju/Exception.hh"
#include "xju/Traced.hh"
#include <utility>

#include <iosfwd>

namespace xju
{
namespace io
{
class Output;
class Input
{
public:
  virtual ~Input() throw();

  // in-line human readable description
  virtual std::string str() const throw()=0;

  class Closed : public xju::Exception
  {
  public:
    Closed(Input const& x, xju::Traced trace) throw();

  };
    
protected:
  virtual int fileDescriptor() const throw() = 0;

  friend std::pair<std::set<Input const* >,std::set<Output const* > > select(
  std::set<Input const* > const& inputs,
  std::set<Output const* > const& outputs,
  std::chrono::system_clock::time_point const& deadline) throw(
    std::bad_alloc);

};
std::ostream& operator<<(std::ostream& s, Input const& x) throw();


}
}
#endif
