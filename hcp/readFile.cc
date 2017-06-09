// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/readFile.hh>

#include "xju/AutoFd.hh" //impl
#include "xju/syscall.hh" //impl
#include <fcntl.h> //impl
#include <sstream> //impl
#include <unistd.h> //impl
#include <string> //impl
#include "xju/fcntl.hh" //impl
#include "xju/unistd.hh" //impl

namespace hcp
{
std::string readFile(
  std::pair<xju::path::AbsolutePath,xju::path::FileName> const& file) throw(
    xju::Exception)
{
  try {
    xju::AutoFd fd(xju::syscall(xju::open, XJU_TRACED)(
                     xju::path::str(file).c_str(), O_RDONLY|O_CLOEXEC, 0777));
    std::ostringstream result;
    char buffer[1024];
    while(1)
    {
      size_t const x(
        xju::syscall(xju::read, XJU_TRACED)(
          fd.fd(), buffer, 1024));
      if (x == 0)
      {
        return result.str();
      }
      result << std::string(buffer, buffer+x);
    }
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "read file " << xju::path::str(file);
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}


}
