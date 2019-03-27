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
#include <xju/steadyNow.hh>
#include <sstream>
#include <xju/format.hh>


std::string t(std::chrono::system_clock::time_point const& t) noexcept
{
  return xju::format::localTime(
    t,
    xju::format::Year,"/",xju::format::Month,"/",xju::format::Day," ",
    xju::format::Hour,":",xju::format::Minute,":",xju::format::Minute);
}

int main(int argc, char* argv[])
{
  if (argc!=2){
    std::cerr << "usage: " << argv[0] << " <wtmp-file-name>\n";
    return 1;
  }
  try{
    auto const fileName{xju::path::split(argv[1])};
    xju::linux::wtmp::LoginLogoutMonitor m{fileName};

    auto const events{m.readEventsUntilDeadline(xju::steadyNow())};
    for (auto e: events){
      xju::linux::wtmp::Event::USER_LOGGED_IN const* ii;
      xju::linux::wtmp::Event::USER_LOGGED_OUT const* oo;
      if (ii=dynamic_cast<xju::linux::wtmp::Event::USER_LOGGED_IN const*>(&*e)){
        auto const& i{ii->userLoggedIn_};
        std::cout << t(i.at_) << ": " << i.user_ << " logged in from "
                  << i.from_ << "\n";
      }
      else if (oo=dynamic_cast<xju::linux::wtmp::Event::USER_LOGGED_OUT const*>(&*e)){
        auto const& o{oo->userLoggedOut_};
        std::cout << t(o.at_) << ": " << o.who_.user_ << " logged out\n";
      }
    };
    return 0;
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << xju::format::join(argv,argv+argc,std::string(" "));
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}
