#include <xju/file/mkdir.hh>
#line 1 "/home/xju/urnest/xju/file/mkdir.hcp"
#line 13
#include "xju/syscall.hh" //impl
#include <sys/stat.h> //impl
#include <sys/types.h> //impl
#include "xju/unistd.hh" //impl

namespace xju
{
namespace file
{
void mkdir(std::pair<xju::path::AbsolutePath,xju::path::FileName> const& d,
           mode_t mode)
  throw(xju::Exception)
{
  xju::syscall(xju::mkdir,XJU_TRACED)(xju::path::str(d).c_str(),mode);
}

}
}
