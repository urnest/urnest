#ifndef _XJU_IO_OSTREAM_HH
#define _XJU_IO_OSTREAM_HH
// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <xju/io/Output.hh>
#include <unistd.h>
#include <chrono>

namespace xju
{
namespace io
{
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
