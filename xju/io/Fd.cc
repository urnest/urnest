#include <xju/io/Fd.hh>
#line 1 "/home/xju/urnest/xju/io/Fd.hcp"
#line 10
#include "xju/assert.hh" //impl
#include "xju/syscall.hh" //impl
#include "xju/unistd.hh" //impl
namespace xju
{
namespace io
{
#line 27
Fd::Fd(int fd) throw():
      fd_(fd)
  {
    xju::assert_greater_equal(fd,0);
  }
  
#line 32
Fd::~Fd() throw()
  {
    if (fd_!=-1) {
      xju::syscall(xju::close,XJU_TRACED)(fd_);
    }
  }
  
Fd::Fd(Fd&& y) throw():
      fd_(std::move(y.fd_))
  {
    y.fd_=-1;
  }
  
#line 44
int Fd::fd() const noexcept
  {
    return fd_;
  }

}
}
