#include <xju/io/IStream.hh>
#line 1 "/home/xju/urnest/xju/io/IStream.hcp"
#line 15
#include <sstream> //impl
#include "xju/format.hh" //impl
#include "xju/syscall.hh" //impl
#include "xju/unistd.hh" //impl
#include <algorithm> //impl
#include <limits> //impl
#include <xju/io/select.hh> //impl

namespace xju
{
namespace io
{
#line 35
size_t IStream::read(void* buffer, size_t bufferSize,
              std::chrono::system_clock::time_point const& deadline) throw(
                std::bad_alloc,
                // end of input before anything was read
                Input::Closed,
                // eg disk error
                xju::Exception) {
    std::chrono::duration<float> const timeout(
      deadline-std::chrono::system_clock::now());
    size_t bytesRead(0);
    try {
      uint8_t* p((uint8_t*)buffer);
      while(bytesRead<bufferSize &&
            xju::io::select({this},deadline).size()) {
        size_t const thisRead=xju::syscall(xju::read,XJU_TRACED)(
          fileDescriptor(),
          p,
          std::min(bufferSize-bytesRead,
                   (size_t)std::numeric_limits<ssize_t>::max()));
        if (thisRead==0) {
          if (bytesRead) {
            return bytesRead;
          }
          throw Input::Closed(*this,XJU_TRACED);
        }
        bytesRead+=thisRead;
      }
      return bytesRead;
    }
    catch(xju::Exception& e)
    {
      std::ostringstream s;
      s << "read bytes from " << (*this)
        << "for " << xju::format::float_(timeout.count())
        << "s or " << (bufferSize-bytesRead) 
        << " more (of " << bufferSize << " total) bytes read "
        << "or end of input";
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
  }

  //point stdin (file descriptor 0) at this input
  
#line 78
void IStream::useAsStdin() throw()
  {
    xju::syscall(xju::dup2,XJU_TRACED)(fileDescriptor(),0);
  }
  

}
}
