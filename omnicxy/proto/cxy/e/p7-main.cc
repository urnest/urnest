// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/p7.hh>
#include <omnicxy/proto/p7.cref.hh>
#include <omnicxy/proto/p7.sref.hh>

#include <omnicxy/proto/cxy/e/E.hh>
#include <iostream>
#include <string>
#include <xju/format.hh>
#include <xju/stringToInt.hh>
#include <stdlib.h>
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
  ~F_impl() throw() {}
  
  // a way to use BaseException mapping to an idl exception
  // (f2 below shows another way)
  virtual void f1() /*throw(::p7::E1)*/
  {
    try {
      throw E(
        "file not found", std::make_pair(__FILE__,__LINE__));
    }
    catch(E& e) {
      e.addContext("F_impl::f1", std::make_pair(__FILE__, __LINE__));
      exceptions_.push_back(e);
      throw ::p7::E1(e);
    }
  }
  // another way to use BaseException mapping to an idl exception
  // (see also f1 above)
  virtual void f2() /*throw(::p7::E1)*/
  {
    try {
      throw ::p7::E1("file not readable", std::make_pair(__FILE__,__LINE__));
    }
    catch(::p7::E1& e) {
      e.addContext("F_impl::f1", std::make_pair(__FILE__, __LINE__));
      exceptions_.push_back(e);
      throw;
    }
  }
  std::vector<E> exceptions_;
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
      cxy::ORB<E> orb("giop:tcp::");
      cxy::cref<p7::F> ref(orb, makeURI(port, OBJECT_NAME));
      try {
        ref->f1();
        xju::assert_abort();
      }
      catch(::p7::E1 const& e) {
        std::cout << readableRepr(e) << std::endl;
      }
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<E> orb(orbEndPoint);

      F_impl x;
      
      cxy::sref<p7::F> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(std::chrono::steady_clock::now()+
                       std::chrono::seconds(30));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<E> orb(orbEndPoint);
      F_impl x;
      
      cxy::sref<p7::F> const xa(orb, OBJECT_NAME, x);
      
      cxy::cref<p7::F> ref(orb, makeURI(port, OBJECT_NAME));
      try {
        ref->f1();
        xju::assert_abort();
      }
      catch(::p7::E1 const& e) {
        xju::assert_(e.context().size(), std::greater_equal<size_t>(),1U);
        xju::assert_equal(e.context()[0], x.exceptions_[0].context()[0]);
      }
      try {
        ref->f2();
        xju::assert_abort();
      }
      catch(::p7::E1 const& e) {
        xju::assert_(e.context().size(), std::greater_equal<size_t>(),1U);
        xju::assert_equal(e.context()[0], x.exceptions_[1].context()[0]);
      }
    }
    
    return 0;
  }
  catch(E& e) {
    e.addContext(xju::format::join(argv, argv+argc, " "), 
                 std::make_pair(__FILE__, __LINE__));
    std::cerr << readableRepr(e, true, false) << std::endl;
    return 1;
  }
}
