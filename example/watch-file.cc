// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include <sys/inotify.h>
#include <map>
#include <xju/path.hh>
#include <set>
#include <algorithm>
#include <xju/functional.hh>
#include <xju/syscall.hh>
#include <utility>
#include <sstream>
#include <xju/Exception.hh>
#include <xju/NonCopyable.hh>
#include <xju/io/Input.hh>
#include <xju/inotify.hh>
#include <xju/format.hh>
#include <chrono>
#include <xju/io/select.hh>
#include <xju/unistd.hh>
#include <iostream>
#include <xju/AutoFd.hh>
#include <xju/steadyEternity.hh>
#include <xju/steadyNow.hh>

namespace
{
union Buffer
{
  inotify_event event;
  char xdata[sizeof(inotify_event)+NAME_MAX+1];
};

std::map<int,xju::path::AbsolutePath> addWatches(
  int inotifyFd,
  std::set<xju::path::AbsFile > const& files) /*throw(
    xju::Exception)*/
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
          IN_ACCESS|
          IN_OPEN|
          IN_CLOSE_WRITE|
          IN_CLOSE_NOWRITE
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

class FileObserver : public xju::io::Input, xju::NonCopyable
{
public:
  // watch for modifications to files
  // - parent directories of all files must exist
  //
  // Note you can use xju::io::select() to wait for one or more of files
  // possibly modified; use read() to see which of files if any modified
  //
  explicit FileObserver(std::set<xju::path::AbsFile> const& files) /*throw(
    // - no resources (see initify_init1)
    // - missing/unreadable parent directory
    xju::Exception)*/ try:
      fd_(xju::syscall(xju::inotify_init1,XJU_TRACED)(IN_NONBLOCK|IN_CLOEXEC)),
      files_(files),
      watches_(addWatches(fd_.fd(),files))
  {
  }
  catch(xju::Exception& e) {
    std::string (*converter)(xju::path::AbsFile const& x)=xju::path::str;
    
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
  std::vector<std::pair<xju::path::AbsFile,uint32_t> > read(
    std::chrono::steady_clock::time_point deadline)
    // xju::Exception
  {
    std::vector<std::pair<xju::path::AbsFile,uint32_t> >  result;
    std::chrono::duration<float> const timeout(
      deadline-std::chrono::steady_clock::now());
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
              result.push_back({x,event->mask});
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
  
private:
  typedef xju::path::AbsFile FileId;
  typedef std::set<FileId> Files;
  
  xju::AutoFd fd_;
  Files const files_;
  std::map<int,xju::path::AbsolutePath> const watches_;

  //Input::
  int fileDescriptor() const throw()
  {
    return fd_.fd();
  }

  //Input::
  std::string str() const throw()
  {
    std::ostringstream s;
    s << (*this);
    return s.str();
  }

  friend std::ostream& operator<<(std::ostream& s, FileObserver const& x)
    noexcept
  {
    std::string (*converter)(std::pair<xju::path::AbsolutePath,
                             xju::path::FileName> const& x)=
      xju::path::str;
    
    return s << "observer of changes to files "
             << xju::format::join(
               x.files_.begin(),x.files_.end(),
               converter,
               ", ");
  }
  
};

std::string strMask(uint32_t const m){
  std::vector<std::string> result;
  if (m & IN_ACCESS) result.push_back("ACCESS");
  if (m & IN_OPEN) result.push_back("OPEN");
  if (m & IN_CLOSE_WRITE) result.push_back("CLOSE WRITABLE");
  if (m & IN_CLOSE_NOWRITE) result.push_back("CLOSE READ-ONLY");
  return xju::format::join(result.begin(),result.end(),",");
}

int main(int argc, char* argv[])
{

  FileObserver o({xju::path::split(argv[1])});
  
  while(true){
    xju::io::select(std::set<xju::io::Input const*>{&o},
                    xju::steadyEternity());
    auto const events(o.read(xju::steadyNow()));
    for(auto e: events){
      std::cout << xju::path::str(e.first) << " "
                << strMask(e.second) << std::endl;
    }
  }
  return 0;
}

