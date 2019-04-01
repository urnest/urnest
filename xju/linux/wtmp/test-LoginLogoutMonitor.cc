// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/linux/wtmp/LoginLogoutMonitor.hh>

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
#include <xju/linux/wtmp/Event.hh>

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

}

void test1() {
  // cases to deal with:
  // - file wtmp_ has never existed (i.e. never successfully opened it)...
  //     ... and still doesn't [1]
  auto const wtmp{xju::path::split("wtmp")};
  rmf(wtmp);
  LoginLogoutMonitor m{wtmp};
  xju::assert_equal(m.readEventsUntilDeadline(xju::steadyNow()),
                    std::vector<std::shared_ptr<Event const> >());

  //     ... and does now [2]
  xju::io::FileWriter writer{wtmp,xju::file::Mode(0777)};
  xju::assert_equal(m.readEventsUntilDeadline(xju::steadyNow()),
                    std::vector<std::shared_ptr<Event const> >());
  
  {
    struct utmp r;
    r.ut_type=1;
    writer.write(&r,sizeof(r));
  }
  xju::assert_equal(m.readEventsUntilDeadline(xju::steadyNow()),
                    std::vector<std::shared_ptr<Event const> >());
  
  {
    struct utmp r;
    ::memset(&r,0,sizeof(r));
    r.ut_type=7;
    r.ut_pid=7;
    strcpy(r.ut_line,"l1");
    strcpy(r.ut_id,"l1");
    strcpy(r.ut_user,"fred");
    strcpy(r.ut_host,"hh");
    r.ut_tv.tv_sec=1;
    r.ut_tv.tv_usec=0;
    writer.write(&r,sizeof(r));
  }
  {
    auto const events{m.readEventsUntilDeadline(xju::steadyNow())};
    xju::assert_equal(events.size(),1U);
    xju::assert_equal(dynamic_cast<Event::USER_LOGGED_IN const&>(
                        **events.begin()),
                      Event::USER_LOGGED_IN(
                        UserLoggedIn("fred","hh",
                                     xju::unix_epoch()+
                                     std::chrono::seconds(1))));
  }
      // - file existed (i.e. we have it open) and has been renamed
      //     ... new file has not yet been created [3]
      //     ... new file has been created [4]
      // - open fails [5]
      // - also want to work where the writer continues to write to old
      //   for an arbitrary amount of time after opening the new file [6]
      // - also where new file is never written to before it is replaced,
      //   ... but is written to after it is replaced [7]
  // select and deadline
  // - already readable
  // - becomes readable
  // - never readable
}

}
}
}

using namespace xju::linux::wtmp;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

