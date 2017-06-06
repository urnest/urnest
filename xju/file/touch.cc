#include <xju/file/touch.hh>
#line 1 "/home/xju/urnest/xju/file/touch.hcp"
#line 13
#include <fcntl.h> //impl
#include <unistd.h> //impl
#include "xju/unistd.hh" //impl
#include "xju/fcntl.hh" //impl
#include <sys/time.h> //impl
#include "xju/AutoFd.hh" //impl
#include <sstream> //impl


namespace xju
{
namespace file
{
namespace
{
xju::SyscallF2<int,int,timeval const*> fut("::futimes",::futimes);
}

void touch(std::pair<xju::path::AbsolutePath,xju::path::FileName> const& f,
           mode_t mode)
  throw(xju::Exception)
{
  try {
    struct timeval n[2];
    gettimeofday(&n[0],0);
    n[1]=n[0];
    xju::AutoFd const fd(
      xju::syscall(xju::open,XJU_TRACED)(xju::path::str(f).c_str(),
                                         O_CREAT|O_APPEND|O_CLOEXEC,
                                         mode));
    xju::syscall(fut,XJU_TRACED)(fd.fd(),n);
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "touch file " << xju::path::str(f);
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

}
}
