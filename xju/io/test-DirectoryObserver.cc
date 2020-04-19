// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/io/DirectoryObserver.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/file/Mode.hh>
#include <xju/file/mkdir.hh>
#include <xju/file/touch.hh>
#include <xju/steadyNow.hh>
#include <xju/file/write.hh>
#include <xju/file/chmod.hh>
#include <xju/file/rm.hh>

namespace xju
{
namespace io
{

std::pair<xju::path::AbsFile,xju::io::DirectoryEntryEvent> e(
  xju::path::AbsFile f, xju::io::DirectoryEntryEvent event){
  return std::make_pair(f,event);
}
std::pair<xju::path::AbsFile,xju::io::DirectoryEntryEvent> e(
  xju::path::AbsolutePath d, xju::io::DirectoryEntryEvent event){
  return std::make_pair(xju::path::split(xju::path::str(d)),event);
}
std::set<std::pair<xju::path::AbsFile,xju::io::DirectoryEntryEvent> > es(
  std::set<std::pair<xju::path::AbsFile,xju::io::DirectoryEntryEvent> > const& x){
  return x;
}

void test1() {
  xju::file::Mode const mode(0777);
  
  auto const d(xju::path::splitdir("x"));
  xju::file::mkdir(d,mode); //assume does not exist

  auto const a(xju::path::split("x/a"));
  auto const b(xju::path::split("x/b"));
  
  // in dir, each event, verify only that event
  {
    xju::io::DirectoryObserver x(
      d,{xju::io::DirectoryEntryEvent::ENTRY_ADDED});
    xju::file::touch(a,mode);
    xju::assert_equal(x.read(xju::steadyNow()),
                      es({e(a,xju::io::DirectoryEntryEvent::ENTRY_ADDED)}));
  }
  {
    xju::io::DirectoryObserver x(
      d,{xju::io::DirectoryEntryEvent::METADATA_CHANGED});
    xju::file::chmod(a,xju::file::Mode(0776));
    xju::assert_equal(x.read(xju::steadyNow()),
                      es({e(a,xju::io::DirectoryEntryEvent::METADATA_CHANGED)}));
    xju::assert_equal(x.read(xju::steadyNow()),
                      es({}));
  }
  {
    xju::io::DirectoryObserver x(
      d,{xju::io::DirectoryEntryEvent::WRITER_CLOSED});
    xju::file::write(a,"fred",4,mode);
    xju::assert_equal(x.read(xju::steadyNow()),
                      es({e(a,xju::io::DirectoryEntryEvent::WRITER_CLOSED)}));
    xju::assert_equal(x.read(xju::steadyNow()),
                      es({}));
  }
  {
    xju::io::DirectoryObserver x(
      d,{xju::io::DirectoryEntryEvent::ENTRY_REMOVED});
    xju::file::rm(a);
    xju::assert_equal(x.read(xju::steadyNow()),
                      es({e(a,xju::io::DirectoryEntryEvent::ENTRY_REMOVED)}));
    xju::assert_equal(x.read(xju::steadyNow()),
                      es({}));
  }

  // exclude self change
  {
    xju::io::DirectoryObserver x(
      d,{xju::io::DirectoryEntryEvent::METADATA_CHANGED});
    xju::file::chmod(d,xju::file::Mode(0776));
    xju::assert_equal(x.read(xju::steadyNow()),
                      es({}));
  }
  
  // timeout
  {
    auto const deadline(xju::steadyNow()+std::chrono::milliseconds(20));
    xju::io::DirectoryObserver x(
      d,{xju::io::DirectoryEntryEvent::METADATA_CHANGED});
    xju::assert_equal(x.read(deadline),
                      es({}));
    xju::assert_greater_equal(xju::steadyNow(),deadline);
  }
  // non existent
  try{
    xju::io::DirectoryObserver x(
      xju::path::splitdir("x/a"),{xju::io::DirectoryEntryEvent::METADATA_CHANGED});
    xju::assert_never_reached();
  }
  catch(xju::SyscallFailed const& e){
    xju::assert_equal(e._errno,ENOENT);
  }
  // file not dir
  try{
    xju::file::touch(b,mode);
    xju::io::DirectoryObserver x(
      xju::path::splitdir("x/b"),{xju::io::DirectoryEntryEvent::METADATA_CHANGED});
    xju::assert_never_reached();
  }
  catch(xju::SyscallFailed const& e){
    xju::assert_equal(e._errno,ENOTDIR);
  }
}

}
}

using namespace xju::io;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

