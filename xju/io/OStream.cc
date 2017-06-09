#include <xju/io/OStream.hh>
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
size_t OStream::write(void const* buffer, 
               size_t bufferSize,
               std::chrono::system_clock::time_point deadline) throw(
                 std::bad_alloc,
                // eg disk error
                 xju::Exception) {
    std::chrono::duration<float> const timeout(
      deadline-std::chrono::system_clock::now());
    size_t bytesWrote(0);
    try {
      uint8_t const* p((uint8_t const*)buffer);
      while(bytesWrote<bufferSize &&
            xju::io::select({this},deadline).size()) {
        size_t const thisWrite=xju::syscall(xju::write,XJU_TRACED)(
          fileDescriptor(),
          p,
          std::min(bufferSize-bytesWrote,
                   (size_t)std::numeric_limits<ssize_t>::max()));
        if (thisWrite==0) {
          if (bytesWrote) {
            return bytesWrote;
          }
          throw Output::Closed(*this,XJU_TRACED);
        }
        bytesWrote+=thisWrite;
      }
      return bytesWrote;
    }
    catch(xju::Exception& e)
    {
      std::ostringstream s;
      s << "write bytes to " << (*this)
        << "for " << xju::format::float_(timeout.count()) 
        << "s or " << (bufferSize-bytesWrote) 
        << " more (of " << bufferSize << " total) bytes written "
        << "or output closed";
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
  }

  
void OStream::useAsStdout() throw()
  {
    xju::syscall(xju::dup2,XJU_TRACED)(fileDescriptor(),1);
  }
  
  
void OStream::useAsStderr() throw()
  {
    xju::syscall(xju::dup2,XJU_TRACED)(fileDescriptor(),2);
  }
  

}
}
