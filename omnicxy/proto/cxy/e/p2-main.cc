// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include "p2.hh"
#include "p2.cref.hh"
#include "p2.sref.hh"

#include "E.hh"
#include <iostream>
#include <string>
#include<xju/format.hh>
#include<xju/stringToInt.hh>
#include <stdlib.h>
#include<xju/mt.hh>
#include <cxy/ORB.hh>
#include "xju/Shared.hh"

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

class F_impl : public p2::F
{
public:
  ~F_impl() throw()
  {
  }
  
  virtual void f1(
    int16_t const& a, 
    int32_t const& b, 
    double const& c, 
    std::string const& d) throw(E)
  {
    std::cout << "F::f1(" 
              << a << ", "
              << b << ", "
              << c << ", "
              << d << ")" << std::endl;
    calls_.push_back(xju::Shared<Call>(
                       new Call::f1(a,b,c,d)));
  }
  virtual int16_t f2() throw(E)
  {
    std::cout << "F::f2(" << std::endl;
    calls_.push_back(xju::Shared<Call>(
                       new Call::f2));
    return 22;
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
    f1(int16_t const& a, 
       int32_t const& b, 
       double const& c, 
       std::string const& d) throw():
        a_(a),
        b_(b),
        c_(c),
        d_(d) {
    }
    int16_t a_;
    int32_t b_;
    double  c_;
    std::string  d_;
    
    friend bool operator==(f1 const& x, f1 const& y) throw()
    {
      return std::make_pair(std::make_pair(x.a_,x.b_),
                            std::make_pair(x.c_,x.d_))==
        std::make_pair(std::make_pair(y.a_,y.b_),
                       std::make_pair(y.c_,y.d_));
    }
  };
  struct Call::f2 : Call
  {
    ~f2() throw()
    {
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
    
    std::string const OBJECT_NAME("p2");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      cxy::ORB<E> orb("giop:tcp::");
      cxy::cref<p2::F> ref(orb, makeURI(port, OBJECT_NAME));
      ref->f1(1, 2, 3.4, "fred");
      std::cout << ref->f2() << std::endl;
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<E> orb(orbEndPoint);

      F_impl x;
      
      cxy::sref<p2::F> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(xju::Time::now()+xju::MicroSeconds(30*1000000));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<E> orb(orbEndPoint);

      F_impl x;
      
      cxy::sref<p2::F> const xa(orb, OBJECT_NAME, x);
      
      cxy::cref<p2::F> ref(orb, makeURI(port, OBJECT_NAME));
      ref->f1(1, 2, 3.4, "fred");
      xju::assert_equal(x.calls_.size(),1U);
      {
        F_impl::Call::f1 const& c(
          dynamic_cast<F_impl::Call::f1 const&>(*x.calls_[0]));
        xju::assert_equal(c, F_impl::Call::f1(1,2,3.4,"fred"));
      }
      xju::assert_equal(ref->f2(),22);
      xju::assert_equal(x.calls_.size(),2U);
      {
        F_impl::Call::f2 const& c(
          dynamic_cast<F_impl::Call::f2 const&>(*x.calls_[1]));
      }
    }
    
    return 0;
  }
  catch(xju::Exception& e) {
    e.addContext(xju::format::join(argv, argv+argc, " "), XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 1;
  }
}
