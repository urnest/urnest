#include <xju/io.hh>
#line 1 "/home/xju/urnest/xju/io.hcp"
#line 14
#include <xju/syscall.hh> //impl
#include <xju/unistd.hh> //impl
#include <xju/format.hh> //impl
#include <sys/select.h> //impl
namespace xju
{
namespace io
{
#line 28
Input::~Input() throw()
  {
  }
  // in-line human readable description
  
#line 37
Input::Closed::Closed(Input const& x, xju::Traced trace) throw():
        xju::Exception(x.str()+" closed",trace)
    {
    }
  
#line 53
std::ostream& operator<<(std::ostream& s, Input const& x) throw()
{
  return s << x.str();
}

#line 61
Output::~Output() throw()
  {
  }
  // in-line human readable description
  
#line 70
Output::Closed::Closed(Output const& x, xju::Traced trace) throw():
        xju::Exception(x.str()+" closed",trace)
    {
    }
  
#line 85
std::ostream& operator<<(std::ostream& s, Output const& x) throw()
{
  return s << x.str();
}

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
    std::bad_alloc)
{
  std::pair<std::set<Input const* >,std::set<Output const* > > result;
  int n;
  do {
    fd_set r;
    FD_ZERO(&r);
    for(auto x : inputs) {
      FD_SET(x->fileDescriptor(),&r);
    }
    fd_set w;
    FD_ZERO(&w);
    for(auto x : outputs) {
      FD_SET(x->fileDescriptor(),&w);
    }
    std::chrono::system_clock::duration const timeout(
      std::max(
        std::chrono::system_clock::duration(
          std::chrono::seconds(0)),
        std::min(deadline-std::chrono::system_clock::now(),
                 std::chrono::system_clock::duration(
                   std::chrono::seconds(std::numeric_limits<long>::max())))));
    std::chrono::seconds const tv_sec(
      std::chrono::duration_cast<std::chrono::seconds>(timeout));
    std::chrono::microseconds const tv_usec(
      std::chrono::duration_cast<std::chrono::microseconds>(timeout-tv_sec));
    struct timeval timeout_;
    timeout_.tv_sec=tv_sec.count();
    timeout_.tv_usec=tv_usec.count();
    n=::select(FD_SETSIZE,&r,&w,0,&timeout_);
    switch(n) {
    case 0:
      break;
    case -1:
      if (errno != EINTR)
      {
        xju::assert_equal(errno,ENOMEM);
        throw std::bad_alloc();
      }
      break;
    default:
      for(auto x : inputs) {
        if (FD_ISSET(x->fileDescriptor(),&r)) {
          result.first.insert(x);
        }
      }
      for(auto x : outputs) {
        if (FD_ISSET(x->fileDescriptor(),&w)) {
          result.second.insert(x);
        }
      }
    }
  }
  while(result.first.size()==0 &&
        result.second.size()==0 &&
        std::chrono::system_clock::now()<deadline);
  return result;
}

// as above but without any outputs
// 
std::set<Input const* > select(
  std::set<Input const* > const& inputs,
  std::chrono::system_clock::time_point const& deadline) throw(
    xju::SyscallFailed)
{
  return select(inputs,std::set<Output const* >(),deadline).first;
}

// as above but without any inputs
// 
std::set<Output const* > select(
  std::set<Output const* > const& outputs,
  std::chrono::system_clock::time_point const& deadline) throw(
    std::bad_alloc)
{
  return select(std::set<Input const*>(),outputs,deadline).second;
}

#line 186
size_t IStream::read(void* buffer, size_t bufferSize,
              std::chrono::system_clock::time_point deadline) throw(
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
            select({this},deadline).size()) {
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

#line 237
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
            select({this},deadline).size()) {
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

}
}
