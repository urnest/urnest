// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/p27.hh>
#include <omnicxy/proto/p27.cref.hh>
#include <omnicxy/proto/p27.sref.hh>

#include <xju/Exception.hh>
#include <iostream>
#include <string>
#include <xju/format.hh>
#include <xju/stringToInt.hh>
#include <stdlib.h>
#include <chrono>
#include <cxy/ORB.hh>
#include <memory>
#include <xju/stringToUInt.hh>

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

class F_impl : public p27::I
{
public:
  ~F_impl() throw()
  {
  }
  
  virtual ::p27::I::R f(
    std::chrono::nanoseconds const& ns,
    std::chrono::microseconds const& us,
    std::chrono::milliseconds const& ms,
    std::chrono::seconds const& s,
    std::chrono::minutes const& m,
    std::chrono::hours const& h) throw(cxy::Exception) override
  {
    std::cout << "F::f(" 
              << xju::format::duration(ns) << ", "
              << xju::format::duration(us) << ", "
              << xju::format::duration(ms) << ", "
              << xju::format::duration(s) << ", "
              << xju::format::duration(m) << ", "
              << xju::format::duration(h) << ")" << std::endl;
    ::p27::I::R result{ns,us,ms,s,m,h};
    calls_.push_back(std::shared_ptr<Call>(
                       new Call::f(result)));
    return result;
  }
  struct Call
  {
    virtual ~Call() throw()
    {
    }
    struct f;
  };
  struct Call::f : Call
  {
    ~f() throw()
    {
    }
    f(::p27::I::R a) throw():
        a_(a)
    {
    }
    ::p27::I::R a_;
    
    friend bool operator==(f const& x, f const& y) throw()
    {
      return x.a_ == y.a_;
    }
  };
  
  std::vector<std::shared_ptr<Call> > calls_;
};

  
int main(int argc, char* argv[])
{
  try {
    if (argc != 3 || !(std::string("client")==argv[2]||
                       std::string("server")==argv[2]||
                       std::string("both")==argv[2])) {
      std::cerr << "usage:  " 
                << argv[0] << " <ip-port> [client|server|both]" << std::endl;
      return 1;
    }
    
    std::string const OBJECT_NAME("p27");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      cxy::ORB<cxy::Exception> orb("giop:tcp::");
      cxy::cref<p27::I> ref(orb, makeURI(port, OBJECT_NAME));
      ref->f(std::chrono::nanoseconds(1),
             std::chrono::microseconds(2),
             std::chrono::milliseconds(3),
             std::chrono::seconds(4),
             std::chrono::minutes(5),
             std::chrono::hours(6));
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      F_impl x;
      
      cxy::sref<p27::I> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(std::chrono::steady_clock::now()+
                       std::chrono::seconds(30));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);
      F_impl x;
      
      cxy::sref<p27::I> const xa(orb, OBJECT_NAME, x);

      unsigned int const repeat(
        xju::stringToUInt(::getenv("CXY_REPEAT")?::getenv("CXY_REPEAT"):"1"));
      for(unsigned int i=0; i != repeat; ++i) {
      
        cxy::cref<p27::I> ref(orb, makeURI(port, OBJECT_NAME));
        ref->f(std::chrono::nanoseconds(1),
               std::chrono::microseconds(2),
               std::chrono::milliseconds(3),
               std::chrono::seconds(4),
               std::chrono::minutes(5),
               std::chrono::hours(6));
        xju::assert_equal(x.calls_.size(),1U);
        {
          F_impl::Call::f const& c(
            dynamic_cast<F_impl::Call::f const&>(*x.calls_[0]));
          xju::assert_equal(c, F_impl::Call::f(
                              ::p27::I::R(std::chrono::nanoseconds(1),
                                std::chrono::microseconds(2),
                                std::chrono::milliseconds(3),
                                std::chrono::seconds(4),
                                std::chrono::minutes(5),
                                std::chrono::hours(6))));
        }
        x.calls_=std::vector<std::shared_ptr<F_impl::Call> >();
      }
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
