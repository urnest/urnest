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
Fd::Fd(int fd) noexcept:
      fd_(fd)
  {
    xju::assert_greater_equal(fd,0);
  }
  
#line 32
Fd::~Fd() noexcept
  {
    if (fd_!=-1) {
      xju::syscall(xju::close,XJU_TRACED)(fd_);
    }
  }
  
#line 38
Fd& Fd::operator=(Fd&& x) noexcept
  {
    if (this != &x) {
      if (fd_!=-1) {
        xju::syscall(xju::close,XJU_TRACED)(fd_);
      }
      fd_=x.fd_;
      x.fd_=-1;
    }
    return *this;
  }
  
#line 49
Fd::Fd(Fd&& y) noexcept:
      fd_(std::move(y.fd_))
  {
    y.fd_=-1;
  }
  
#line 54
int Fd::fd() const noexcept
  {
    return fd_;
  }

}
}
