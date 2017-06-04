#include <xju/AutoFd.hh>
#line 1 "/home/xju/urnest/xju/AutoFd.hcp"
#line 12
#include "xju/assert.hh" //impl
#include "xju/syscall.hh" //impl
#include "xju/unistd.hh" //impl


namespace xju
{
#line 24
AutoFd::AutoFd(int fd) noexcept:
      fd_(fd)
  {
    xju::assert_greater_equal(fd,0);
  }
  
#line 29
AutoFd::~AutoFd() noexcept
  {
    if (fd_!=-1) {
      xju::syscall(xju::close,XJU_TRACED)(fd_);
    }
  }
  
#line 35
AutoFd& AutoFd::operator=(AutoFd&& x) noexcept
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
  
#line 46
AutoFd::AutoFd(AutoFd&& y) noexcept:
      fd_(std::move(y.fd_))
  {
    y.fd_=-1;
  }
  
#line 51
int AutoFd::fd() const noexcept
  {
    return fd_;
  }

}
