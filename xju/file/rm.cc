#include <xju/file/rm.hh>
#include "xju/unistd.hh" //impl

namespace xju
{
namespace file
{
void rm(std::pair<xju::path::AbsolutePath,xju::path::FileName> const& file)
  throw(xju::Exception)
{
  xju::syscall(xju::unlink,XJU_TRACED)(xju::path::str(file).c_str());
}

}
}
