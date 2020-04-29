// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/p15.hh>
#include <omnicxy/proto/p15.cref.hh>
#include <omnicxy/proto/p15.sref.hh>

#include <xju/Exception.hh>
#include <iostream>
#include <string>
#include <xju/format.hh>
#include <xju/stringToInt.hh>
#include <stdlib.h>
#include <chrono>
#include <cxy/ORB.hh>
#include <memory>
#include <mcheck.h>

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

class AAA_impl : public p15::AAA
{
public:
  ~AAA_impl() throw()
  {
  }

  // AAA::
  int32_t f(
    int32_t const& x) /*throw(cxy::Exception)*/
  {
    std::cout << "AAA::f(" 
              << x << ")" << std::endl;
    calls_.push_back(std::shared_ptr<Call>(
                       new Call::f1(x)));
    return x;
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
    f1(::int32_t const& a) throw():
        a_(a) {
    }
    ::int32_t a_;
    
    friend bool operator==(f1 const& x, f1 const& y) throw()
    {
      return x.a_==y.a_;
    }
  };
  std::vector<std::shared_ptr<Call> > calls_;
};

  
class BBB_impl : public p15::BBB
{
public:
  BBB_impl(::cxy::IOR< ::p15::AAA > const& aaa) throw():
      aaa_(aaa)
  {
  }
  
  ~BBB_impl() throw()
  {
  }

  ::cxy::IOR< ::p15::AAA const> aaa_;
  
  // BBB::
  virtual ::cxy::IOR< ::p15::AAA > getA() throw()
  {
    return aaa_;
  }
  // BBB::
  virtual ::cxy::IOR< ::p15::AAA > echoA(::cxy::IOR< ::p15::AAA >const& a) 
    throw()
  {
    return a;
  }
};

  
int main(int argc, char* argv[])
{
  try {
    mtrace();
    if (argc != 3 || !(std::string("client")==argv[2]||
                       std::string("server")==argv[2]||
                       std::string("both")==argv[2])) {
      std::cerr << "usage:  " 
                << argv[0] << " <ip-port> [client|server|both]" << std::endl;
      return 1;
    }
    
    std::string const OBJECT_NAME("p15");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      {
        cxy::ORB<cxy::Exception> orb("giop:tcp::");
        cxy::cref<p15::BBB> ref(orb, makeURI(port, OBJECT_NAME));
        cxy::cref<p15::AAA> a(orb, ref->getA());
        cxy::cref<p15::AAA> aa(orb, ref->echoA(a));
        aa->f(11);
      }
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      AAA_impl x;
      cxy::sref<p15::AAA> const xa(orb, OBJECT_NAME+"aaa", x);
      
      BBB_impl y(xa.ior());
      cxy::sref<p15::BBB> const xb(orb, OBJECT_NAME, y);
      
      orb.monitorUntil(std::chrono::steady_clock::now()+
                       std::chrono::seconds(30));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      AAA_impl x;
      cxy::sref<p15::AAA> const xa(orb, OBJECT_NAME+"aaa", x);
      
      BBB_impl y(xa.ior());
      cxy::sref<p15::BBB> const xb(orb, OBJECT_NAME, y);
      
      cxy::cref<p15::BBB> ref(orb, makeURI(port, OBJECT_NAME));
      cxy::cref<p15::AAA> a(orb, ref->getA());
      a->f(11);
      xju::assert_equal(x.calls_.size(),1U);
      {
        AAA_impl::Call::f1 const& c(
          dynamic_cast<AAA_impl::Call::f1 const&>(*x.calls_[0]));
        xju::assert_equal(c, AAA_impl::Call::f1(11));
      }
    }
    muntrace();
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
