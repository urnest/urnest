// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include "p14.hh"
#include "p14.cref.hh"
#include "p14.sref.hh"

#include <xju/Exception.hh>
#include <iostream>
#include <string>
#include <xju/format.hh>
#include <xju/stringToInt.hh>
#include <stdlib.h>
#include <xju/mt.hh>
#include <xju/Time.hh>
#include <cxy/ORB.hh>
#include "xju/Shared.hh"

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

class F_impl : public nb::B1
{
public:
  ~F_impl() throw()
  {
  }
  
  int32_t f1(
    int32_t const& x) throw(cxy::Exception)
  {
    std::cout << "F::f1(" 
              << x << ")" << std::endl;
    calls_.push_back(xju::Shared<Call>(
                       new Call::f1(x)));
    return x;
  }
  int32_t f2(
    int32_t const& x) throw(cxy::Exception)
  {
    std::cout << "F::f2(" 
              << x << ")" << std::endl;
    calls_.push_back(xju::Shared<Call>(
                       new Call::f2(x)));
    return x;
  }
  struct Call
  {
    virtual ~Call() throw()
    {
    }
    struct f1;
    struct f2;
  };
  struct Call::f1 : Call
  {
    ~f1() throw()
    {
    }
    f1(::int32_t const& a) throw():
        a_(a) {
    }
    ::int32_t a_;
    
    friend bool operator==(f1 const& x, f1 const& y) throw()
    {
      return x.a_==y.a_;
    }
  };
  struct Call::f2 : Call
  {
    ~f2() throw()
    {
    }
    f2(::int32_t const& a) throw():
        a_(a) {
    }
    ::int32_t a_;
    
    friend bool operator==(f2 const& x, f2 const& y) throw()
    {
      return x.a_==y.a_;
    }
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
    
    std::string const OBJECT_NAME("p14");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      {
        cxy::ORB<cxy::Exception> orb("giop:tcp::");
        cxy::cref<nb::B1> ref(orb, makeURI(port, OBJECT_NAME));
        ref->f1(11);
        ref->f1(31);
      }
      {
        cxy::ORB<cxy::Exception> orb("giop:tcp::");
        cxy::cref<na::A1> ref(orb, makeURI(port, OBJECT_NAME));
        ref->f1(11);
      }
      
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      F_impl x;
      
      cxy::sref<nb::B1> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(xju::Time::now()+xju::MicroSeconds(30*1000000));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);
      F_impl x;
      
      cxy::sref<nb::B1> const xa(orb, OBJECT_NAME, x);
      {
        cxy::cref<nb::B1> ref(orb, makeURI(port, OBJECT_NAME));
        ref->f1(17);
        xju::assert_equal(x.calls_.size(),1U);
        {
          F_impl::Call::f1 const& c(
            dynamic_cast<F_impl::Call::f1 const&>(*x.calls_[0]));
          xju::assert_equal(c, F_impl::Call::f1(17));
        }
        ref->f2(31);
        xju::assert_equal(x.calls_.size(),2U);
        {
          F_impl::Call::f2 const& c(
            dynamic_cast<F_impl::Call::f2 const&>(*x.calls_[1]));
          xju::assert_equal(c, F_impl::Call::f2(31));
        }
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
