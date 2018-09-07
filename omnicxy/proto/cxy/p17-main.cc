// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/p17.hh>
#include <omnicxy/proto/p17.cref.hh>
#include <omnicxy/proto/p17.sref.hh>

#include <xju/Exception.hh>
#include <iostream>
#include <string>
#include <xju/format.hh>
#include <xju/stringToInt.hh>
#include <stdlib.h>
#include <chrono>
#include <cxy/ORB.hh>
#include <xju/stringToUInt.hh>
#include <memory>

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

std::string str(cxy::optional<p17::Name> const& x) throw()
{
  if (!x.valid()) {
    return "";
  }
  return xju::format::quote(x.value()._);
}

class p17_impl : public p17
{
public:
  ~p17_impl() throw()
  {
  }
  
  virtual cxy::optional<Name> echo(cxy::optional<Name> const& s) throw(
    cxy::Exception)
  {
    std::cout << "servant: p17::echo(" << str(s) << ")" 
              << std::endl;
    calls_.push_back(std::shared_ptr<Call>(
                       new Call::f1(s)));
    return s;
  }

  struct Call
  {
    virtual ~Call() throw()
    {
    }
    struct f1;
    struct f2;
  };
  struct Call::f1 : Call
  {
    ~f1() throw()
    {
    }
    f1(cxy::optional<Name> const& x):
        x_(x) {
    }
    cxy::optional<Name> x_;
    friend bool operator==(f1 const& x, f1 const& y) throw()
    {
      return x.x_==y.x_;
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
    
    std::string const OBJECT_NAME("p17");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      cxy::ORB<cxy::Exception> orb("giop:tcp::");
      cxy::cref<p17> ref(orb, makeURI(port, OBJECT_NAME));
      std::cout << str(ref->echo(cxy::optional<p17::Name>(
                                   p17::Name("hello world"))))
                << std::endl;
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      p17_impl x;
      
      cxy::sref<p17> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(std::chrono::system_clock::now()+
                       std::chrono::seconds(30));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);
      p17_impl x;
      
      cxy::sref<p17> const xa(orb, OBJECT_NAME, x);
      
      cxy::cref<p17> ref(orb, makeURI(port, OBJECT_NAME));
      unsigned int const repeat(
        xju::stringToUInt(::getenv("CXY_REPEAT")?::getenv("CXY_REPEAT"):"1"));
      for(unsigned int i=0; i != repeat; ++i) {
        cxy::optional<p17::Name> const result(
          ref->echo(cxy::optional<p17::Name>(p17::Name("hello world"))));
        std::cout << str(result) << std::endl;
        xju::assert_equal(result,cxy::optional<p17::Name>(
                            p17::Name("hello world")));
        xju::assert_equal(x.calls_.size(),1U);
        p17_impl::Call::f1 const& c(
          dynamic_cast<p17_impl::Call::f1 const&>(*x.calls_[0]));
        xju::assert_equal(c, p17_impl::Call::f1(p17::Name("hello world")));
        x.calls_=std::vector<std::shared_ptr<p17_impl::Call> >();
      }
      cxy::optional<p17::Name> const result(
        ref->echo(cxy::optional<p17::Name>()));
      std::cout << str(result) << std::endl;
      xju::assert_equal(result,cxy::optional<p17::Name>());
      xju::assert_equal(x.calls_.size(),1U);
      p17_impl::Call::f1 const& c(
        dynamic_cast<p17_impl::Call::f1 const&>(*x.calls_[0]));
      xju::assert_equal(c, p17_impl::Call::f1(cxy::optional<p17::Name>()));
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
