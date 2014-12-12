// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include "p7.hh"
#include "p7.cref.hh"
#include "p7.sref.hh"

#include <xju/Exception.hh>
#include <iostream>
#include <string>
#include <xju/format.hh>
#include <xju/stringToInt.hh>
#include <stdlib.h>
#include <xju/mt.hh>
#include <xju/Time.hh>
#include <cxy/ORB.hh>

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

class F_impl : public p7::F
{
public:
  ~F_impl() throw()
  {
  }
  
  virtual void f1() throw(::p7::E1)
  {
    try {
      throw cxy::Exception(
        "file not found", std::make_pair(__FILE__,__LINE__));
    }
    catch(cxy::Exception& e) {
      e.addContext("F_impl::f1", std::make_pair(__FILE__, __LINE__));
      exceptions_.push_back(e);
      throw ::p7::E(e);
    }
  }
  virtual void f2() throw(::p7::E2)
  {
    try {
      throw ::p7::E2(
        "file not readable", std::make_pair(__FILE__,__LINE__));
    }
    catch(::p7::E2& e) {
      e.addContext("F_impl::f1", std::make_pair(__FILE__, __LINE__));
      exceptions_.push_back(e);
      throw;
    }
  }
  std::vector<cxy::Exception> exceptions_;
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
    
    std::string const OBJECT_NAME("p7");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      cxy::ORB<cxy::Exception> orb("giop:tcp::");
      cxy::cref<p7::F> ref(orb, makeURI(port, OBJECT_NAME));
      try {
        ref->f1();
        xju::assert_abort();
      }
      catch(::p7::E const& e) {
        std::cout << readableRepr(e) << std::endl;
      }
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      F_impl x;
      
      cxy::sref<p7::F> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(xju::Time::now()+xju::MicroSeconds(30*1000000));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);
      F_impl x;
      
      cxy::sref<p7::F> const xa(orb, OBJECT_NAME, x);
      
      cxy::cref<p7::F> ref(orb, makeURI(port, OBJECT_NAME));
      try {
        ref->f1();
        xju::assert_abort();
      }
      catch(::p7::E1 const& e) {
        xju::assert_(e.context.size(), std::greater_equal<size_t>(),1U);
        xju::assert_equal(e.context[0], f1.exceptions_[0].context(0))
      }
      try {
        ref->f2();
        xju::assert_abort();
      }
      catch(::p7::E2 const& e) {
        xju::assert_(e.context.size(), std::greater_equal<size_t>(),1U);
        xju::assert_equal(e.context[0], xa.exceptions_[1].context(0))
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
