#include "write.hh"
#include <unistd.h> //impl
#include "xju/fcntl.hh" //impl
#include "xju/AutoFd.hh" //impl
#include "xju/syscall.hh" //impl
#include <cinttypes> //impl
#include <sstream> //impl
#include "xju/unistd.hh" //impl
#include <xju/format.hh> //impl


namespace xju
{
namespace file
{
void write(
  std::pair<xju::path::AbsolutePath,xju::path::FileName> const& file,
  void const* content,
  size_t const contentSize,
  mode_t const mode) throw(
    xju::Exception)
{
  try {
    xju::AutoFd const fd(xju::syscall(xju::open,XJU_TRACED)(
                           xju::path::str(file).c_str(),
                           O_WRONLY|O_CREAT|O_TRUNC|O_CLOEXEC,
                           mode));
    uint8_t const*const p((uint8_t const*)content);
    size_t bytesWritten(0);
    do {
      bytesWritten+=xju::syscall(xju::write,XJU_TRACED)(
        fd.fd(),
        p+bytesWritten,
        contentSize-bytesWritten);
    }
    while(bytesWritten < contentSize);
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "write " << contentSize << "-byte content "
      << " to file " << xju::path::str(file)
      << ", creating file with mode "
      << xju::format::octal(mode)
      << " & ~umask & 0777 if necessary";
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

void write(
  std::pair<xju::path::AbsolutePath,xju::path::FileName> const& file,
  std::string const& content,
  mode_t const mode) throw(
    xju::Exception)
{
  return xju::file::write(file,content.c_str(),content.size(),mode);
}

}
}
