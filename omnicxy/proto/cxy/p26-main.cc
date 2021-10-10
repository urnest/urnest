// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/p26.hh>
#include <omnicxy/proto/p26.cref.hh>
#include <omnicxy/proto/p26.sref.hh>

#include <xju/Exception.hh>
#include <iostream>
#include <string>
#include <xju/format.hh>
#include <xju/stringToInt.hh>
#include <stdlib.h>
#include <chrono>
#include <cxy/ORB.hh>
#include <xju/stringToUInt.hh>
#include <xju/assert.hh>

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

class I26_impl : public p26::I26
{
public:
  ~I26_impl() throw() {}
  
  virtual int32_t e() throw() {
    return (int32_t)'e';
  }
  virtual int32_t f() throw() {
    return (int32_t)'f';
  }
  virtual std::tuple< std::string,int32_t > g() throw() {
    return std::make_tuple(std::string("g"),(int32_t)'g');
  }
  virtual std::tuple< int32_t,std::string > h(
    int32_t const& x) throw() {
    return std::make_tuple(x,std::string("h"));
  }
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
    
    std::string const OBJECT_NAME("p26");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      cxy::ORB<cxy::Exception> orb("giop:tcp::");

      cxy::cref<p26::I26> ref(orb, makeURI(port, OBJECT_NAME));
      std::cout << ref->e() << std::endl
                << ref->f() << std::endl;
      auto const x{ref->g()};
      std::cout << std::get<0>(x) << ", "
                << std::get<1>(x) << std::endl;
      auto const y{ref->h(3)};
      std::cout << std::get<0>(y) << ", "
                << std::get<1>(y) << std::endl;
      
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      I26_impl x;
      
      cxy::sref<p26::I26> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(std::chrono::steady_clock::now()+
                       std::chrono::seconds(30));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);
      I26_impl x;
      
      cxy::sref<p26::I26> const xa(orb, OBJECT_NAME, x);
      
      cxy::cref<p26::I26> ref(orb, makeURI(port, OBJECT_NAME));
      unsigned int const repeat(
        xju::stringToUInt(::getenv("CXY_REPEAT")?::getenv("CXY_REPEAT"):"1"));
      for(unsigned int i=0; i != repeat; ++i) {
        xju::assert_equal(ref->e(),(int32_t)'e');
        xju::assert_equal(ref->f(),(int32_t)'f');
        xju::assert_equal(ref->g(),std::make_tuple(std::string("g"),
                                                   (int32_t)'g'));
        xju::assert_equal(ref->h(3),std::make_tuple(3,std::string("h")));
        
        
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
