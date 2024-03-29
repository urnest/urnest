// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include "omnicxy/proto/p1.hh"
#include "omnicxy/proto/p1.sref.hh"

#include <xju/Exception.hh>
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

class F_impl : public p1::F
{
public:
  ~F_impl() throw() {}
  
  
  virtual void f1() /*throw(cxy::Exception)*/
  {
    std::cout << "F::f1()" << std::endl;
  }
};

  
int main(int argc, char* argv[])
{
  try {
    if (argc != 2) {
      std::cerr << "usage:  " 
                << argv[0] << " <ip-port>" << std::endl;
      return 1;
    }
    
    std::string const OBJECT_NAME("p1");
    
    int const port(xju::stringToInt(argv[1]));
    
    std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
    cxy::ORB<cxy::Exception> orb(orbEndPoint);
    
    F_impl x;
    
    cxy::sref<p1::F> const xa(orb, OBJECT_NAME, x);
    
    try {
      F_impl x2;
      cxy::sref<p1::F> const xb(orb, OBJECT_NAME, x2);
      xju::assert_abort();
    }
    catch(cxy::Exceptions<cxy::Exception>::DuplicateName& e) {
      std::ostringstream expect;
      expect << "Failed to create CORBA servant with corbaloc name \"p1\" in ORB at giop:tcp::" << port << " because\n\"p1\" is already in use.";
      
        xju::assert_equal(readableRepr(e), expect.str());
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
