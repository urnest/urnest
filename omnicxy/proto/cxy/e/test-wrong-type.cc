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
#include "p1.cref.hh"

#include "E.hh"
#include <iostream>
#include <string>
#include<xju/format.hh>
#include<xju/stringToInt.hh>
#include <stdlib.h>
#include<xju/mt.hh>
#include <cxy/ORB.hh>

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
  }
  virtual int16_t f2() throw(E)
  {
    return 22;
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
    
    std::string const OBJECT_NAME("p2");
    
    int const port(xju::stringToInt(argv[1]));
    
    std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
    cxy::ORB<E> orb(orbEndPoint);
    
    F_impl x;
    
    cxy::sref<p2::F> const xa(orb, OBJECT_NAME, x);

    try {
      cxy::cref<p1::F> ref(orb, makeURI(port, OBJECT_NAME));
      xju::assert_abort();
    }
    catch(cxy::Exceptions<E>::WrongType& e) {
      E& f(e);
    }
    return 0;
  }
  catch(xju::Exception& e) {
    e.addContext(xju::format::join(argv, argv+argc, " "), XJU_TRACED);
    std::cerr << readableRepr(e) << std::endl;
    return 1;
  }
}
