#include <xju/pipe.hh>
#line 1 "/home/xju/urnest/xju/pipe.hcp"
#line 15
#include <xju/unistd.hh> //impl
#include "xju/NonCopyable.hh" //impl
#include "xju/assert.hh" //impl
#line 19
#include <xju/AutoFd.hh> //impl

namespace xju
{
#line 24
namespace
{
class I : public xju::io::IStream
  {
  public:
    //Input::
    std::string str() const throw()
    {
      std::ostringstream s;
      s << "readable end of pipe (file descriptor " << fileDescriptor() <<")";
      return s.str();
    }
    explicit I(xju::AutoFd&& fd) throw():
        fd_(std::move(fd))
    {
    }
  private:
    //Input::
    int fileDescriptor() const throw()
    {
      return fd_.fd();
    }
    xju::AutoFd fd_;
  };
  class O : public xju::io::OStream, xju::NonCopyable
  {
  public:
    //Output::
    std::string str() const throw()
    {
      std::ostringstream s;
      s << "writeable end of pipe (file descriptor " << fileDescriptor() <<")";
      return s.str();
    }
    explicit O(xju::AutoFd&& fd) throw():
        fd_(std::move(fd))
    {
    }
  private:
    //Output::
    int fileDescriptor() const throw()
    {
      return fd_.fd();
    }
    xju::AutoFd fd_;
  };
}

std::pair<std::unique_ptr<xju::io::IStream>, std::unique_ptr<xju::io::OStream> > pipe(
  bool closeReadEndOnExec,
  bool closeWriteEndOnExec) throw(std::bad_alloc,
                                  SyscallFailed) {
  std::pair<int,int> fds(xju::pipe_());
  xju::AutoFd fdr(fds.first); //ensure close on exception
  xju::AutoFd fdw(fds.second); //ensure close on exception
  if (!closeReadEndOnExec) {
    ::fcntl(fdr.fd(),F_SETFD,::fcntl(fdw.fd(),F_GETFD)&~FD_CLOEXEC);
  }
  if (!closeWriteEndOnExec) {
    ::fcntl(fdw.fd(),F_SETFD,::fcntl(fdw.fd(),F_GETFD)&~FD_CLOEXEC);
  }
  return std::make_pair(
    std::unique_ptr<xju::io::IStream>(new I(std::move(fdr))),
    std::unique_ptr<xju::io::OStream>(new O(std::move(fdw))));
}

}
