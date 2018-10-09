// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/p9.hh>
#include <omnicxy/proto/p9.cref.hh>
#include <omnicxy/proto/p9.sref.hh>

#include <xju/Exception.hh>
#include <iostream>
#include <string>
#include <xju/format.hh>
#include <xju/stringToInt.hh>
#include <stdlib.h>
#include <xju/Lock.hh>
#include <xju/Mutex.hh>
#include <xju/Condition.hh>
#include <chrono>
#include <cxy/ORB.hh>
#include <unistd.h>

std::string makeURI(int port, std::string const& objectName) throw()
{
  if(!getenv("P9_HOST")) {
    std::ostringstream s;
    s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
    return s.str();
  }
  else
  {
    std::ostringstream s;
    s << "corbaloc:iiop:" << getenv("P9_HOST")<< ":"<< port << "/" << objectName;
    return s.str();
  }
}

class F_impl : public p9::F
{
public:
  F_impl() throw():
    changed_(guard_)
  {
  }
  
  ~F_impl() throw()
  {
  }
  
  virtual void f1(std::string const& x) throw(cxy::Exception)
  {
    std::cout << "F::f1(" << x.size() << ")" << std::endl;
    xju::Lock l(guard_);
    calls_.push_back(x);
    changed_.signal(l);
  }

  xju::Mutex guard_;
  xju::Condition changed_;
  std::vector<std::string> calls_;
};

  
int main(int argc, char* argv[])
{
  try {
    if (argc != 3 || !(std::string("client")==argv[2]||
                       std::string("client-rep")==argv[2]||
                       std::string("server")==argv[2]||
                       std::string("both")==argv[2])) {
      std::cerr << "usage:  " 
                << argv[0] << " <ip-port> [client|server|both]" << std::endl;
      return 1;
    }
    
    std::string const OBJECT_NAME("p9");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      cxy::ORB<cxy::Exception> orb("giop:tcp::");
      cxy::cref<p9::F> ref(orb, makeURI(port, OBJECT_NAME));
      ref->f1("fred");
      ref->f1(std::string(10000,'a'));
    }
    else if (argv[2]==std::string("client-rep")) {
      cxy::ORB<cxy::Exception> orb("giop:tcp::");
      cxy::cref<p9::F> ref(orb, makeURI(port, OBJECT_NAME));
      while(true) {
        ref->f1(std::string(100,'a'));
        ::sleep(2);
      }
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      F_impl x;
      
      cxy::sref<p9::F> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(std::chrono::steady_clock::now()+
                       std::chrono::seconds(30));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);
      F_impl x;
      
      cxy::sref<p9::F> const xa(orb, OBJECT_NAME, x);
      
      cxy::cref<p9::F> ref(orb, makeURI(port, OBJECT_NAME));
      ref->f1("fred");
      ref->f1(std::string(6000,'a'));
      ref->f1(std::string(10000,'a'));
      xju::Lock l(x.guard_);
      auto const onlyWaitUntil(
        std::chrono::steady_clock::now()+
        std::chrono::seconds(5));
      
      while((x.calls_.size()<3) &&
            (std::chrono::steady_clock::now()<onlyWaitUntil)) {
        x.changed_.wait(l,onlyWaitUntil);
      }
      xju::assert_equal(x.calls_.size(),3);
      xju::assert_equal(x.calls_[0],"fred");
      xju::assert_equal(x.calls_[1],std::string(6000,'a'));
      xju::assert_equal(x.calls_[2],std::string(10000,'a'));
    }

    return 0;
  }
  catch(xju::Exception& e) {
    e.addContext(xju::format::join(argv, argv+argc, " "), XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 1;
  }
  catch(cxy::Exception& e) {
    e.addContext(xju::format::join(argv, argv+argc, " "), 
                 std::make_pair(__FILE__, __LINE__));
    std::cerr << readableRepr(e, true, false) << std::endl;
    return 1;
  }
}
