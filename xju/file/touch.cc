#include <xju/file/touch.hh>
#line 1 "/home/xju/urnest/xju/file/touch.hcp"
#line 13
#include <fcntl.h> //impl
#include <unistd.h> //impl
#include "xju/unistd.hh" //impl
#include "xju/fcntl.hh" //impl


namespace xju
{
namespace file
{
void touch(std::pair<xju::path::AbsolutePath,xju::path::FileName> const& d,
           mode_t mode)
  throw(xju::Exception)
{
  xju::syscall(xju::close,XJU_TRACED)(
    xju::syscall(xju::open,XJU_TRACED)(xju::path::str(d).c_str(),
                                       O_CREAT|O_APPEND|O_CLOEXEC,
                                       mode));
}

}
}
