#include <xju/io/FileObserver.hh>
#include <xju/inotify.hh> //impl
#include <limits.h> //impl
#include "xju/syscall.hh" //impl
#include <algorithm> //impl
#include "xju/functional.hh" //impl
#include <sstream> //impl
#include "xju/format.hh" //impl
#include <iostream> //impl
#include <sys/inotify.h> //impl
#include <xju/io/select.hh> //impl
#include "xju/unistd.hh" //impl

namespace xju
{
namespace io
{
namespace
{
struct Buffer
{
  struct inotify_event event;
  char data[NAME_MAX+1];
};
std::map<int,xju::path::AbsolutePath> addWatches(
  int inotifyFd,
  std::set<std::pair<xju::path::AbsolutePath,xju::path::FileName> > const& files) throw(
    xju::Exception)
{
  std::map<int,xju::path::AbsolutePath> result;

  std::set<xju::path::AbsolutePath> dirs;
  std::transform(files.begin(),files.end(),
                 std::inserter(dirs,dirs.end()),
                 xju::functional::First());
  for(auto x:dirs) {
    try {
      int const watchId(
        xju::syscall(xju::inotify_add_watch,XJU_TRACED)(
          inotifyFd,str(x).c_str(),
          // covers touch, rm, mv, and write
          IN_CREATE| //touch, write-non-existent
          IN_DELETE| //rm
          IN_MOVED_TO|IN_MOVED_FROM| //mv
          IN_MODIFY //write
          ));
      result.insert(std::make_pair(watchId,x));
    }
    catch(xju::Exception& e) {
      std::ostringstream s;
      s << "add watch for directory " << xju::path::str(x);
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
  }
  return result;
}

}

FileObserver::FileObserver(std::set<std::pair<xju::path::AbsolutePath,xju::path::FileName> > const& files) throw(
    // - no resources (see initify_init1)
    // - missing/unreadable parent directory
    xju::Exception) try:
      fd_(xju::syscall(xju::inotify_init1,XJU_TRACED)(IN_NONBLOCK|IN_CLOEXEC)),
      files_(files),
      watches_(addWatches(fd_.fd(),files))
  {
  }
  catch(xju::Exception& e) {
    std::string (*converter)(std::pair<xju::path::AbsolutePath,xju::path::FileName> const& x)=xju::path::str;
    
    std::ostringstream s;
    s << "start watching for changes to files "
      << xju::format::join(files.begin(),files.end(),
                           converter,
                           ", ");
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }

  // read file changes until deadline
  // - if deadline has passed, gets any past, unread changes
  // - only returns names originally asked for
  
std::set<std::pair<xju::path::AbsolutePath,xju::path::FileName> > FileObserver::read(
    std::chrono::system_clock::time_point deadline) throw()
  {
    Files result;
    std::chrono::duration<float> const timeout(
      deadline-std::chrono::system_clock::now());
    size_t bytesRead(0);
    try {
      Buffer buffer;
      size_t const bufferSize(sizeof(buffer));
      while(xju::io::select({this},deadline).size()) {
        uint8_t* p((uint8_t*)&buffer);
        size_t const thisRead=xju::syscall(xju::read,XJU_TRACED)(
          fileDescriptor(),
          p,
          bufferSize);
        if (thisRead==0) {
          if (result.size()) {
            return result;
          }
          throw Input::Closed(*this,XJU_TRACED);
        }
        uint8_t* const end(p+thisRead);
        while((end-p)>=sizeof(inotify_event)) {
          auto event((inotify_event const*)p);
          xju::path::FileName const f(event->name);
          auto i(watches_.find(event->wd));
          if (i!=watches_.end()) {
            FileId const x((*i).second,f);
            if (files_.find(x)!=files_.end()) {
              result.insert(x);
            }
          }
          p+=sizeof(inotify_event)+event->len;
        }
      }
      return result;
    }
    catch(xju::Exception& e)
    {
      std::ostringstream s;
      s << "read file changes from " << (*this)
        << "until " << xju::format::float_(timeout.count())
        << "s elapsed or end of input";
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
  }
  

int FileObserver::fileDescriptor() const throw()
  {
    return fd_.fd();
  }

  //Input::
  
std::string FileObserver::str() const throw()
  {
    std::ostringstream s;
    s << (*this);
    return s.str();
  }

  
}
}
