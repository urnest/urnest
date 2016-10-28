#ifndef _XJU_IO_HCP
#define _XJU_IO_HCP
// Copyright (c) 2015 Trevor Taylor
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


class Output
{
public:
  virtual ~Output() throw();

  // in-line human readable description
  virtual std::string str() const throw()=0;

  class Closed : public xju::Exception
  {
  public:
    Closed(Output const& x, xju::Traced trace) throw();

  };
    
protected:
  virtual int fileDescriptor() const throw() = 0;

friend std::pair<std::set<Input const*>,std::set<Output const* > > select(
  std::set<Input const* > const& inputs,
  std::set<Output const* > const& outputs,
  std::chrono::system_clock::time_point const& deadline) throw(
    std::bad_alloc);
};
std::ostream& operator<<(std::ostream& s, Output const& x) throw();


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


class IStream : public Input
{
public:
  // read bytes into buffer until deadline reached or bufferSize bytes read
  // or end of input is reached
  // - return number of bytes read
  // - reads any immediately available bytes if deadline has already passed
  size_t read(void* buffer, size_t bufferSize,
              std::chrono::system_clock::time_point deadline) throw(
                std::bad_alloc,
                // end of input before anything was read
                Input::Closed,
                // eg disk error
                xju::Exception);


  //point stdin (file descriptor 0) at this input
  void useAsStdin() throw();

  
};

class OStream : public Output
{
public:
  // write bytes from buffer until bufferSize bytes written or
  // deadline reached or output closed
  // - return number of bytes written
  // - writes what is possible immediately if deadline has already passed
  // - returns immediately if output closes before deadline
  size_t write(void const* buffer, 
               size_t bufferSize,
               std::chrono::system_clock::time_point deadline) throw(
                 std::bad_alloc,
                // eg disk error
                 xju::Exception);


  void useAsStdout() throw();

  
  void useAsStderr() throw();

  
};

}
}
#endif
