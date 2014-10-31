// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include "p1.cref.hh"

#include <xju/Exception.hh>
#include <iostream>
#include <string>
#include "xju/format.hh"
#include "xju/stringToInt.hh"
#include <stdlib.h>
#include <cxy/ORB.hh>

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

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

    try {
      cxy::cref<p1::F> ref(orb, makeURI(port, OBJECT_NAME));
      xju::assert_abort();
    }
    catch(cxy::Exceptions<cxy::Exception>::NoSuchObject& e) {
      return 0;
    }
  }
  catch(cxy::Exception& e) {
    e.addContext(xju::format::join(argv, argv+argc, " "), 
                 std::make_pair(__FILE__, __LINE__));
    std::cerr << readableRepr(e, true, false) << std::endl;
    return 1;
  }
}
