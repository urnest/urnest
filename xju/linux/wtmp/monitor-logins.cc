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
#include <xju/steadyNow.hh>
#include <sstream>
#include <xju/format.hh>
#include <chrono>


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
    xju::linux::wtmp::LoginMonitor m{fileName,0};

    // skip existing
    auto const events{m.readEvents(xju::steadyNow())};

    while(true){
      auto const events{m.readEvents(xju::steadyNow()+
                                     std::chrono::seconds(3))};
      for (auto i: events){
        std::cout << i.user_ << " logged in from "
                  << i.from_ << " at " << t(i.at_) << "\n";
      };
      std::cout.flush();
    }
    return 0;
  }
  catch(xju::Exception& e){
    std::ostringstream s;
    s << xju::format::join(argv,argv+argc,std::string(" "));
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}
