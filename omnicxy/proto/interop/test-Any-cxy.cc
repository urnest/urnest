// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/a.hh>
#include <omnicxy/proto/a.cref.hh>
#include <omnicxy/proto/a.sref.hh>
#include <omnicxy/proto/a.any.hh>


std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

class server_impl : public a::server
{
public:
  explicit server_impl(cxy::ORB<cxy::Exception>& orb) throw():
      orb_(orb)
  {
  }
  cxy::ORB<cxy::Exception>& orb_;
  
  ~server_impl() throw()
  {
  }
  
  //a::server::
  virtual void run(cxy::IOR<a::reflect> const& f_) throw()
  {
    cxy::cref<a::reflect> f(orb_,f_);

    cxy::Any<> const y(f->f(cxy::Any<>((int16_t)3)));
    int16_t const z(y.get<int16_t>());
    xju::assert_equal(z,(int16_t)3);
  }
};

int main(uint16_t const port, std::string const& testAnyCxxExe) throw(
  cxy::Exception)
{
  try {
    std::string const OBJECT_NAME("client");

    std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
    cxy::ORB<cxy::Exception> orb(orbEndPoint);
    
    server_impl x;
    
    cxy::sref<a::server> const xa(orb, OBJECT_NAME, x);
    
    //REVISIT: run testAnyCxxExe
    return 0;
  }
  catch(cxy::Exception& e) {
    std::ostringstream s;
    s << "run using port " << port;
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

int main(int argc, char* argv[])
{
  try {
    if (argc != 2) {
      std::cerr << "usage:  " 
                << argv[0] << " <test-Any-cxx-exe>" << std::endl;
      return 1;
    }
    
    for(uint16_t port=3387; port!=4387; ++port) {
      try {
        return main(port);
      }
      catch(cxy::PortInUse&) {
      }
    }
    
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
