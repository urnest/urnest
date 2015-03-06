// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include "p12.hh"
#include "p12.cref.hh"
#include "p12.sref.hh"

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

#include <xju/assert.hh>

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

std::ostream& operator<<(std::ostream& s, ::p12::U1 const& x) throw()
{
  if (dynamic_cast< ::p12::U1::A const*>(&x)) {
    ::p12::U1::A const& y(dynamic_cast< ::p12::U1::A const&>(x));
    return s << "::p12::U1::A " << y.a_;
  }
  if (dynamic_cast< ::p12::U1::B const*>(&x)) {
    return s << "::p12::U1::B";
  }
  if (dynamic_cast< ::p12::U1::C const*>(&x)) {
    ::p12::U1::C const& y(dynamic_cast< ::p12::U1::C const&>(x));
    return s << "::p12::U1::C " << y.c_;
  }
  
}

class F_impl : public p12::F
{
public:
  ~F_impl() throw()
  {
  }
  
  virtual void f1(
    ::xju::Shared< ::p12::U1 const> const& a) throw(cxy::Exception)
  {
    std::cout << "F::f1(" 
              << (*a) << ")" << std::endl;
    calls_.push_back(xju::Shared<Call>(
                       new Call::f1(a)));
  }
  struct Call
  {
    virtual ~Call() throw()
    {
    }
    struct f1;
  };
  struct Call::f1 : Call
  {
    ~f1() throw()
    {
    }
    f1(::xju::Shared< ::p12::U1 const> const& a) throw():
        a_(a) {
    }
    ::xju::Shared< ::p12::U1 const> a_;
    
    friend bool operator==(f1 const& x, f1 const& y) throw()
    {
      ::p12::U1 const& x_(*x.a_);
      ::p12::U1 const& y_(*y.a_);
      
      return x_==y_;
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
    
    std::string const OBJECT_NAME("p10");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      cxy::ORB<cxy::Exception> orb("giop:tcp::");
      cxy::cref<p12::F> ref(orb, makeURI(port, OBJECT_NAME));
      ref->f1(::xju::Shared< ::p12::U1 const>(
                new ::p12::U1::C(10.6)));
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      F_impl x;
      
      cxy::sref<p12::F> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(xju::Time::now()+xju::MicroSeconds(30*1000000));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);
      F_impl x;
      
      cxy::sref<p12::F> const xa(orb, OBJECT_NAME, x);
      
      cxy::cref<p12::F> ref(orb, makeURI(port, OBJECT_NAME));
      ref->f1(::xju::Shared< ::p12::U1 const>(new ::p12::U1::A(20L)));
      ref->f1(::xju::Shared< ::p12::U1 const>(new ::p12::U1::B("fred")));
      xju::assert_equal(x.calls_.size(),2U);
      {
        F_impl::Call::f1 const& c(
          dynamic_cast<F_impl::Call::f1 const&>(*x.calls_[0]));
        xju::assert_equal(c, F_impl::Call::f1(::xju::Shared< ::p12::U1 const>(
                                                new ::p12::U1::A(20L))));
      }
      {
        F_impl::Call::f1 const& c(
          dynamic_cast<F_impl::Call::f1 const&>(*x.calls_[1]));
        xju::assert_equal(c, F_impl::Call::f1(::xju::Shared< ::p12::U1 const>(
                                                new ::p12::U1::B("fred"))));
        xju::assert_equal(discriminator(*c.a_), ::X::B);
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
