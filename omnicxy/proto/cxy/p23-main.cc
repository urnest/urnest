// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/p23.hh>
#include <omnicxy/proto/p23.cref.hh>
#include <omnicxy/proto/p23.sref.hh>

#include <xju/Exception.hh>
#include <iostream>
#include <string>
#include <xju/format.hh>
#include <xju/stringToInt.hh>
#include <stdlib.h>
#include <chrono>
#include <cxy/ORB.hh>
#include <xju/stringToUInt.hh>

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

class D_impl : public D
{
public:
  ~D_impl() throw() {}
  
  
  virtual std::string fa() /*throw(cxy::Exception)*/
  {
    return "fa()";
  }
  virtual std::string fb() /*throw(cxy::Exception)*/
  {
    return "fb()";
  }
  virtual std::string fc() /*throw(cxy::Exception)*/
  {
    return "fc()";
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
    
    std::string const OBJECT_NAME("p23");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      cxy::ORB<cxy::Exception> orb("giop:tcp::");
      {
        cxy::cref<D> ref(orb, makeURI(port, OBJECT_NAME));
        ref->fa();
        ref->fb();
        ref->fc();
      }
      {
        cxy::cref<B> ref(orb, makeURI(port, OBJECT_NAME));
        ref->fa();
        ref->fb();
      }
      {
        cxy::cref<C> ref(orb, makeURI(port, OBJECT_NAME));
        ref->fa();
        ref->fc();
      }
      {
        cxy::cref<A> ref(orb, makeURI(port, OBJECT_NAME));
        ref->fa();
      }
      
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      D_impl x;
      
      cxy::sref<D> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(std::chrono::steady_clock::now()+
                       std::chrono::seconds(30));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);
      D_impl x;
      
      cxy::sref<D> const xa(orb, OBJECT_NAME, x);
      
      cxy::cref<D> ref(orb, makeURI(port, OBJECT_NAME));
      unsigned int const repeat(
        xju::stringToUInt(::getenv("CXY_REPEAT")?::getenv("CXY_REPEAT"):"1"));
      for(unsigned int i=0; i != repeat; ++i) {
        ref->fa();
        ref->fb();
        ref->fc();
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
