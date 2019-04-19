// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/linux/wtmp/LoginMonitor.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/file/rm.hh>
#include <xju/steadyNow.hh>
#include <xju/io/FileWriter.hh>
#include <utmp.h>
#include <string.h>
#include <xju/linux/wtmp/UserLoggedIn.hh>
#include <xju/unix_epoch.hh>
#include <chrono>
#include <xju/file/rename.hh>
#include <xju/next.hh>
#include <thread>
#include <xju/test/call.hh>

namespace xju
{
namespace linux
{
namespace wtmp
{

namespace
{
void rmf(
  std::pair<xju::path::AbsolutePath,xju::path::FileName> const& file) noexcept
{
  try{
    xju::file::rm(file);
  }
  catch(xju::SyscallFailed const& e){
    xju::assert_equal(e._errno,ENOENT);
  }
}

utmp loggedIn(std::string const& line,
              std::string const& user,
              std::chrono::seconds at) noexcept
{
    struct utmp r;
    ::memset(&r,0,sizeof(r));
    r.ut_type=7;
    r.ut_pid=7;
    strcpy(r.ut_line,line.c_str());
    strcpy(r.ut_id,  line.c_str());
    strcpy(r.ut_user,user.c_str());
    strcpy(r.ut_host,"hh");
    r.ut_tv.tv_sec=at.count();
    r.ut_tv.tv_usec=0;
    return r;
}

}

void test1() {
  // cases to deal with:
  // - file wtmp_ has never existed (i.e. never successfully opened it)...
  //     ... and still doesn't [1]
  auto const wtmp{xju::path::split("wtmp")};
  auto const wtmp1{xju::path::split("wtmp.1")};
  rmf(wtmp);
  rmf(wtmp1);
  LoginMonitor m{wtmp,2};
  xju::assert_equal(m.readEventsUntilDeadline(xju::steadyNow()),
                    std::vector<UserLoggedIn>());

  //     ... and does now [2]
  xju::io::FileWriter writer{wtmp,xju::file::Mode(0666)};
  xju::assert_equal(m.readEventsUntilDeadline(xju::steadyNow()),
                    std::vector<UserLoggedIn>());
  
  {
    struct utmp r;
    r.ut_type=1;
    writer.write(&r,sizeof(r));
  }
  xju::assert_equal(m.readEventsUntilDeadline(xju::steadyNow()),
                    std::vector<UserLoggedIn>());
  
  {
    auto const r{loggedIn("l1","fred",std::chrono::seconds(1))};
    writer.write(&r,sizeof(r));
  }
  {
    auto const r{loggedIn("l2","u2",std::chrono::seconds(2))};
    writer.write(&r,sizeof(r));
  }
  {
    auto const events{m.readEventsUntilDeadline(xju::steadyNow())};
    xju::assert_equal(events.size(),2U);
    xju::assert_equal(*events.begin(),
                      UserLoggedIn("fred","hh",
                                   xju::unix_epoch()+
                                   std::chrono::seconds(1)));
    xju::assert_equal(*(events.begin()+1),
                      UserLoggedIn("u2","hh",
                                   xju::unix_epoch()+
                                   std::chrono::seconds(2)));
  }
  {
    // - file existed (i.e. we have it open) and has been renamed
    //     ... new file has not yet been created [3]
    auto const r{loggedIn("l1","u2",std::chrono::seconds(2))};
    writer.write(&r,sizeof(r));
    xju::file::rename(wtmp,wtmp1);
    auto const events{m.readEventsUntilDeadline(xju::steadyNow())};
    xju::assert_equal(events.size(),1U);
    xju::assert_equal(*events.begin(),
                      UserLoggedIn("u2","hh",
                                   xju::unix_epoch()+
                                   std::chrono::seconds(2)));
  }
  //     ... new file has been created but not yet written [4]
  // - also want to work where the writer continues to write to old
  //   for an arbitrary amount of time after opening the new file [6]
  xju::io::FileWriter writer2{wtmp,xju::file::Mode(0666)};
  {
    auto const r{loggedIn("l2","u3",std::chrono::seconds(3))};
    writer.write(&r,sizeof(r));
    auto const events{m.readEventsUntilDeadline(xju::steadyNow())};
    xju::assert_equal(events.size(),1U);
    xju::assert_equal(*events.begin(),
                      UserLoggedIn("u3","hh",
                                   xju::unix_epoch()+
                                   std::chrono::seconds(3)));
  }
  {
    // new file has been written
    auto const r{loggedIn("l1","u4",std::chrono::seconds(4))};
    writer2.write(&r,sizeof(r));
    auto const events{m.readEventsUntilDeadline(xju::steadyNow())};
    xju::assert_equal(events.size(),1U);
    xju::assert_equal(*events.begin(),
                      UserLoggedIn("u4","hh",
                                   xju::unix_epoch()+
                                   std::chrono::seconds(4)));
    // have read from new file, old file should no longer be monitored
    auto const r2{loggedIn("l1","u5",std::chrono::seconds(5))};
    writer.write(&r2,sizeof(r2));
    auto const events2{m.readEventsUntilDeadline(xju::steadyNow())};
    xju::assert_equal(events2.size(),0U);
  }
    
  // - open fails [5]
  {
    xju::file::rename(wtmp,wtmp1);
    {
      xju::io::FileWriter writer{wtmp,xju::file::Mode(0266)};
    }
    try{
      auto const events{m.readEventsUntilDeadline(xju::steadyNow())};
      rmf(wtmp);
      xju::assert_never_reached();
    }
    catch(xju::Exception& e){
      rmf(wtmp);
      xju::assert_equal(readableRepr(e),"Failed to open file "+xju::path::str(wtmp)+" for reading because\nopen system call failed, errno = 13.");
    }
  }
}
void test2(){
  // - also where new file is never written to before it is replaced,
  //   ... but is written to after it is replaced [7]
  auto const wtmp{xju::path::split("wtmp")};
  auto const wtmp1{xju::path::split("wtmp.1")};
  auto const wtmp2{xju::path::split("wtmp.2")};
  rmf(wtmp);
  LoginMonitor m{wtmp,2};
  xju::io::FileWriter writer{wtmp,xju::file::Mode(0666)};
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  xju::file::rename(wtmp,wtmp1);
  xju::io::FileWriter writer2{wtmp,xju::file::Mode(0666)};
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  xju::file::rename(wtmp1,wtmp2);
  xju::file::rename(wtmp,wtmp1);
  xju::io::FileWriter writer3{wtmp,xju::file::Mode(0666)};
  {
    auto const r{loggedIn("l1","u6",std::chrono::seconds(6))};
    writer.write(&r,sizeof(r));
    auto const events{m.readEventsUntilDeadline(xju::steadyNow())};
    xju::assert_equal(events.size(),1U);
    xju::assert_equal(*events.begin(),
                      UserLoggedIn("u6","hh",
                                   xju::unix_epoch()+
                                   std::chrono::seconds(6)));
  }
  {
    auto const r{loggedIn("l1","u7",std::chrono::seconds(7))};
    writer2.write(&r,sizeof(r));
    auto const events{m.readEventsUntilDeadline(xju::steadyNow())};
    xju::assert_equal(events.size(),1U);
    xju::assert_equal(*events.begin(),
                      UserLoggedIn("u7","hh",
                                   xju::unix_epoch()+
                                   std::chrono::seconds(7)));
  }
  {
    auto const r{loggedIn("l1","u8",std::chrono::seconds(8))};
    writer3.write(&r,sizeof(r));
    auto const events{m.readEventsUntilDeadline(xju::steadyNow())};
    xju::assert_equal(events.size(),1U);
    xju::assert_equal(*events.begin(),
                      UserLoggedIn("u8","hh",
                                   xju::unix_epoch()+
                                   std::chrono::seconds(8)));
  }
}

void test3(){
  // select and deadline
  auto const wtmp{xju::path::split("wtmp")};
  auto const wtmp1{xju::path::split("wtmp.1")};
  rmf(wtmp);
  LoginMonitor m{wtmp,2};
  xju::io::FileWriter writer{wtmp,xju::file::Mode(0666)};
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  xju::file::rename(wtmp,wtmp1);
  xju::io::FileWriter writer2{wtmp,xju::file::Mode(0666)};
  {
    // - never readable
    auto const t1{xju::steadyNow()};
    auto const events{m.readEventsUntilDeadline(
        xju::steadyNow()+std::chrono::milliseconds(50))};
    xju::assert_greater_equal(
      xju::steadyNow(),t1+std::chrono::milliseconds(50));
    xju::assert_less(
      xju::steadyNow(),t1+std::chrono::milliseconds(100));
  }

  {
    // - becomes readable
    auto const t1{xju::steadyNow()};
    auto c{xju::test::call(
        [&](){
          return m.readEventsUntilDeadline(
            t1+std::chrono::milliseconds(100));
            })};
    std::this_thread::sleep_until(t1+std::chrono::milliseconds(50));
    {
      auto const r{loggedIn("l1","u9",std::chrono::seconds(9))};
      writer.write(&r,sizeof(r));
    }
    std::this_thread::sleep_until(t1+std::chrono::milliseconds(80));
    {
      auto const r{loggedIn("l1","u10",std::chrono::seconds(10))};
      writer2.write(&r,sizeof(r));
    }
    auto const events{c->getResultBy(t1+std::chrono::milliseconds(160))};
    xju::assert_equal(events.size(),2U);
    xju::assert_equal(*events.begin(),
                      UserLoggedIn("u9","hh",
                                   xju::unix_epoch()+
                                   std::chrono::seconds(9)));
    xju::assert_equal(*(events.begin()+1),
                      UserLoggedIn("u10","hh",
                                   xju::unix_epoch()+
                                   std::chrono::seconds(10)));
  }
    
  // - already readable
  {
    {
      auto const r{loggedIn("l1","u11",std::chrono::seconds(11))};
      writer2.write(&r,sizeof(r));
    }
    auto const t1{xju::steadyNow()};
    auto const events{m.readEventsUntilDeadline(
        xju::steadyNow()+std::chrono::milliseconds(50))};
    xju::assert_greater_equal(
      xju::steadyNow(),t1+std::chrono::milliseconds(50));
    xju::assert_less(
      xju::steadyNow(),t1+std::chrono::milliseconds(100));
    xju::assert_equal(events.size(),1U);
    xju::assert_equal(*events.begin(),
                      UserLoggedIn("u11","hh",
                                   xju::unix_epoch()+
                                   std::chrono::seconds(11)));
  }
}

}
}
}

using namespace xju::linux::wtmp;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  test3(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

