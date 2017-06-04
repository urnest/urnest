//     -*- mode: c++ ; c-file-style: "xju" ; -*-
//
// Copyright (c) 2008
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/readFile.hh>
#include "xju/AutoFd.hh"
#include "xju/syscall.hh"
#include <sstream>
#include <xju/fcntl.hh>
#include <xju/unistd.hh>

namespace xju
{
std::string readFile(std::string fileName) throw(
  xju::Exception)
{
  try
  {
    xju::AutoFd fd(xju::syscall(xju::open, XJU_TRACED)(
      fileName.c_str(), O_RDONLY|O_CLOEXEC, 0777));
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
  catch(xju::Exception& e)
  {
    std::ostringstream s;
    s << "read contents of file " << fileName;
    e.addContext(s, XJU_TRACED);
    throw;
  }
}

}
