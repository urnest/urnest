// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/p22.hh>
#include <omnicxy/proto/p22.cref.hh>
#include <omnicxy/proto/p22.sref.hh>

#include <xju/Exception.hh>
#include <iostream>
#include <string>
#include <xju/format.hh>
#include <xju/stringToInt.hh>
#include <stdlib.h>
#include <chrono>
#include <cxy/ORB.hh>
#include <memory>

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

class F_impl : public p22
{
public:
  ~F_impl() throw() {}
  
  
  virtual ::cxy::UnixTimeMilli f(
    ::cxy::UnixTimeMilli const& a) /*throw(cxy::Exception)*/
  {
    std::cout << "::p22::f(" 
              << a << ") -> UnixTimeMilli(33)" << std::endl;
    calls_.push_back(std::shared_ptr<Call>(
                       new Call::f(a)));
    return ::cxy::UnixTimeMilli(std::chrono::milliseconds(33));
  }
  struct Call
  {
    virtual ~Call() throw() {}
    
    struct f;
  };
  struct Call::f : Call
  {
    ~f() throw() {}
    
    f(::cxy::UnixTimeMilli const& a) throw():
        a_(a) {
    }
    ::cxy::UnixTimeMilli a_;
    
    friend bool operator==(f const& x, f const& y) throw()
    {
      return x.a_ == y.a_;
    }
  };
  std::vector<std::shared_ptr<Call> > calls_;
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
    
    std::string const OBJECT_NAME("p22");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      cxy::ORB<cxy::Exception> orb("giop:tcp::");
      cxy::cref<p22> ref(orb, makeURI(port, OBJECT_NAME));
      std::cout << ref->f(::cxy::UnixTimeMilli(std::chrono::milliseconds(8)))
                << std::endl;
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      F_impl x;
      
      cxy::sref<p22> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(std::chrono::steady_clock::now()+
                       std::chrono::seconds(30));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);
      F_impl x;
      
      cxy::sref<p22> const xa(orb, OBJECT_NAME, x);
      
      cxy::cref<p22> ref(orb, makeURI(port, OBJECT_NAME));
      xju::assert_equal(ref->f(::cxy::UnixTimeMilli(std::chrono::milliseconds(1))),::cxy::UnixTimeMilli(std::chrono::milliseconds(33)));
      {
        F_impl::Call::f const& c(
          dynamic_cast<F_impl::Call::f const&>(*x.calls_[0]));
        xju::assert_equal(c, F_impl::Call::f(::cxy::UnixTimeMilli(std::chrono::milliseconds(1))));
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
