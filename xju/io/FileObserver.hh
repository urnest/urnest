#ifndef _XJU_IO_FILEOBSERVER_HCP
#define _XJU_IO_FILEOBSERVER_HCP
// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//

#include "xju/io/Input.hh"
#include "xju/NonCopyable.hh"
#include <set>
#include <utility>
#include <xju/path.hh>
#include "xju/Exception.hh"
#include <xju/format.hh>
#include <chrono>
#include <map>
#include "xju/AutoFd.hh"
#include <iosfwd>

namespace xju
{
namespace io
{

class FileObserver : public xju::io::Input, xju::NonCopyable
{
public:
  // watch for modifications to files
  // - parent directories of all files must exist
  // - use xju::io::select() to wait for one or more of files possibly
  //   modified; use read() to see which of files if any modified
  //
  FileObserver(std::set<std::pair<xju::path::AbsolutePath,xju::path::FileName> > const& files) throw(
    // - no resources (see initify_init1)
    // - missing/unreadable parent directory
    xju::Exception);


  // read file changes until deadline
  // - if deadline has passed, gets any past, unread changes
  // - only returns names originally asked for
  std::set<std::pair<xju::path::AbsolutePath,xju::path::FileName> > read(
    std::chrono::system_clock::time_point deadline) throw();

  
private:
  typedef std::pair<xju::path::AbsolutePath,xju::path::FileName> FileId;
  typedef std::set<FileId> Files;
  
  xju::AutoFd fd_;
  Files const files_;
  std::map<int,xju::path::AbsolutePath> const watches_;

  //Input::
  int fileDescriptor() const throw();


  //Input::
  std::string str() const throw();


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

  
}
}
#endif
