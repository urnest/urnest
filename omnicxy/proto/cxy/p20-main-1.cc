// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/p20.hh>
#include <omnicxy/proto/p20.cref.hh>
#include <omnicxy/proto/p20.sref.hh>

#include <xju/Exception.hh>
#include <iostream>
#include <string>
#include <xju/format.hh>
#include <xju/stringToInt.hh>
#include <stdlib.h>
#include <chrono>
#include <cxy/ORB.hh>
#include "xju/Shared.hh"
#include <xju/stringToUInt.hh>

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

class F_impl : public p20::F
{
public:
  ~F_impl() throw()
  {
  }
  
  //p20::F::
  virtual cxy::Any<> f1(cxy::Any<> const& x) throw()
  {
    calls_.push_back(xju::Shared<Call>(new Call::f1(x)));
    return x;
  }
  //p20::F::
  virtual int16_t f2(const int16_t& x) throw()
  {
    calls_.push_back(xju::Shared<Call>(new Call::f2(x)));
    return x;
  }
  //p20::F::
  virtual p20::B f3(const p20::B& x) throw()
  {
    calls_.push_back(xju::Shared<Call>(new Call::f3(x)));
    return x;
  }
  
  struct Call
  {
    virtual ~Call() throw()
    {
    }
    struct f1;
    struct f2;
    struct f3;
  };
  struct Call::f1 : Call
  {
    ~f1() throw()
    {
    }
    f1(cxy::Any<> a) throw():
        a_(a) {
    }
    cxy::Any<> a_;
  };
  struct Call::f2 : Call
  {
    ~f2() throw()
    {
    }
    f2(int16_t a) throw():
        a_(a) {
    }
    int16_t a_;
  };
  struct Call::f3 : Call
  {
    ~f3() throw()
    {
    }
    f3(p20::B const& a) throw():
        a_(a) {
    }
    p20::B a_;
    
  };
  std::vector<xju::Shared<Call> > calls_;
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
    
    std::string const OBJECT_NAME("p2");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      cxy::ORB<cxy::Exception> orb("giop:tcp::");
      cxy::cref<p20::F> ref(orb, makeURI(port, OBJECT_NAME));
      int16_t const y(ref->f2((int16_t)3));
      std::cout << y << std::endl;
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      F_impl x;
      
      cxy::sref<p20::F> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(std::chrono::system_clock::now()+
                       std::chrono::seconds(30));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);
      F_impl x;
      
      cxy::sref<p20::F> const xa(orb, OBJECT_NAME, x);

      unsigned int const repeat(
        xju::stringToUInt(::getenv("CXY_REPEAT")?::getenv("CXY_REPEAT"):"1"));
      for(unsigned int i=0; i != repeat; ++i) {
      
        cxy::cref<p20::F> ref(orb, makeURI(port, OBJECT_NAME));
        int16_t const y(ref->f2((int16_t)3));
        xju::assert_equal(y,(int16_t)3);
        xju::assert_equal(x.calls_.size(),1U);
        {
          F_impl::Call::f2 const& c(
            dynamic_cast<F_impl::Call::f2 const&>(*x.calls_[0]));
          xju::assert_equal(c.a_, (int16_t)3);
        }
        x.calls_=std::vector<xju::Shared<F_impl::Call> >();
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
