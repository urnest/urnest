// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/p25.hh>
#include <omnicxy/proto/p25.cref.hh>
#include <omnicxy/proto/p25.sref.hh>

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

class I25_impl : public p25::I25
{
public:
  ~I25_impl() throw() {}
  
  virtual p25::ThreeTwos f25(p25::ThreeTwos const& a) noexcept
  {
    return a;
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
    
    std::string const OBJECT_NAME("p25");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      cxy::ORB<cxy::Exception> orb("giop:tcp::");

      cxy::cref<p25::I25> ref(orb, makeURI(port, OBJECT_NAME));
      auto const x(ref->f25({p25::Two(1,2),
              p25::Two(3,4),
              p25::Two(5,6)}));
      std::cout << xju::format::join(
        x.begin(),
        x.end(),
        [](p25::Two const& t){
          return xju::format::str(t.a_)+","+xju::format::str(t.b_);
        },
        ",") << std::endl;
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      I25_impl x;
      
      cxy::sref<p25::I25> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(std::chrono::steady_clock::now()+
                       std::chrono::seconds(30));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);
      I25_impl x;
      
      cxy::sref<p25::I25> const xa(orb, OBJECT_NAME, x);
      
      cxy::cref<p25::I25> ref(orb, makeURI(port, OBJECT_NAME));
      unsigned int const repeat(
        xju::stringToUInt(::getenv("CXY_REPEAT")?::getenv("CXY_REPEAT"):"1"));
      for(unsigned int i=0; i != repeat; ++i) {
        auto const x(ref->f25(
                       p25::ThreeTwos{
                         p25::Two(1,2),
                         p25::Two(3,4),
                         p25::Two(5,6)}));
        xju::assert_equal(x,
                          p25::ThreeTwos{
                            p25::Two(1,2),
                            p25::Two(3,4),
                            p25::Two(5,6)});
        
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
