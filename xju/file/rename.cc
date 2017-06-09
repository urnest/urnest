#include "rename.hh"
#include <stdio.h> //impl
#include "xju/syscall.hh" //impl
namespace xju
{
namespace file
{
void rename(
  std::pair<xju::path::AbsolutePath,xju::path::FileName> const& file,
  std::pair<xju::path::AbsolutePath,xju::path::FileName> const& newName) throw(
    xju::Exception)
{
  try {
    xju::syscall(xju::rename,XJU_TRACED)(xju::path::str(file).c_str(),
                                      xju::path::str(newName).c_str());
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "rename " << xju::path::str(file) << " to "
      << xju::path::str(newName);
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

// rename directory to newName
void rename(
  std::pair<xju::path::AbsolutePath,xju::path::DirName> const& directory,
  std::pair<xju::path::AbsolutePath,xju::path::DirName> const& newName) throw(
    xju::Exception)
{
  try {
    xju::syscall(xju::rename,XJU_TRACED)(xju::path::str(directory).c_str(),
                                      xju::path::str(newName).c_str());
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "rename " << xju::path::str(directory) << " to "
      << xju::path::str(newName);
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

}
}
