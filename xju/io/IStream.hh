#ifndef _XJU_IO_ISTREAM_HCP
#define _XJU_IO_ISTREAM_HCP
// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <xju/io/Input.hh>
#include <unistd.h>
#include <chrono>

namespace xju
{
namespace io
{

class IStream : public Input
{
public:
  // read bytes into buffer until deadline reached or bufferSize bytes read
  // or end of input is reached
  // - return number of bytes read
  // - reads any immediately available bytes if deadline has already passed
  size_t read(void* buffer, size_t bufferSize,
              std::chrono::system_clock::time_point const& deadline) throw(
                std::bad_alloc,
                // end of input before anything was read
                Input::Closed,
                // eg disk error
                xju::Exception);


  //point stdin (file descriptor 0) at this input
  void useAsStdin() throw();

  
};


}
}


#endif
