// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/p11.hh>
#include <omnicxy/proto/p11.cref.hh>
#include <omnicxy/proto/p11.sref.hh>

#include <xju/Exception.hh>
#include <iostream>
#include <string>
#include <xju/format.hh>
#include <xju/stringToInt.hh>
#include <stdlib.h>
#include <chrono>
#include <cxy/ORB.hh>

#include <xju/assert.hh>

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

std::ostream& operator<<(std::ostream& s, ::U1 const& x) throw()
{
  if (dynamic_cast< ::U1::A const*>(&x)) {
    ::U1::A const& y(dynamic_cast< ::U1::A const&>(x));
    return s << "::U1::A " << y.a_;
  }
  if (dynamic_cast< ::U1::B const*>(&x)) {
    return s << "::U1::B";
  }
  if (dynamic_cast< ::U1::C const*>(&x)) {
    ::U1::C const& y(dynamic_cast< ::U1::C const&>(x));
    return s << "::U1::C " << y.c_;
  }
  
}

class F_impl : public p11::F
{
public:
  ~F_impl() throw()
  {
  }
  
  virtual ::std::shared_ptr< ::U1 const> f1(
    ::std::shared_ptr< ::U1 const> const& a) throw(cxy::Exception)
  {
    std::cout << "F::f1(" 
              << (*a) << ")" << std::endl;
    calls_.push_back(std::shared_ptr<Call>(
                       new Call::f1(a)));
    return a;
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
    f1(::std::shared_ptr< ::U1 const> const& a) throw():
        a_(a) {
    }
    ::std::shared_ptr< ::U1 const> a_;
    
    friend bool operator==(f1 const& x, f1 const& y) throw()
    {
      ::U1 const& x_(*x.a_);
      ::U1 const& y_(*y.a_);
      
      return x_==y_;
    }
  };
  std::vector<std::shared_ptr<Call> > calls_;
};

  
int main(int argc, char* argv[])
{
  xju::assert_equal(p11_Z, "z");
  xju::assert_equal(p11_Q, 10);
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
      cxy::cref<p11::F> ref(orb, makeURI(port, OBJECT_NAME));
      std::cout << ref->f1(::std::shared_ptr< ::U1 const>(
                             new ::U1::C(10.6))) << std::endl;
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      F_impl x;
      
      cxy::sref<p11::F> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(std::chrono::steady_clock::now()+
                       std::chrono::seconds(30));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);
      F_impl x;
      
      cxy::sref<p11::F> const xa(orb, OBJECT_NAME, x);
      
      cxy::cref<p11::F> ref(orb, makeURI(port, OBJECT_NAME));
      ::std::shared_ptr< ::U1 const> r(
        ref->f1(::std::shared_ptr< ::U1 const>(new ::U1::A(20L))));
      xju::assert_equal(x.calls_.size(),1U);
      {
        F_impl::Call::f1 const& c(
          dynamic_cast<F_impl::Call::f1 const&>(*x.calls_[0]));
        xju::assert_equal(c, F_impl::Call::f1(::std::shared_ptr< ::U1 const>(
                                                new ::U1::A(20L))));
      }
      xju::assert_equal(*r, ::U1::A(20L));
      xju::assert_less(*r, ::U1::A(21L));
      xju::assert_less(*r, ::U1::B());
      xju::assert_less(*r, ::U1::C(0.0));
      xju::assert_less(*r->clone(), ::U1::C(0.0));
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
