// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/p24.hh>
#include <omnicxy/proto/p24.cref.hh>
#include <omnicxy/proto/p24.sref.hh>

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
#include <xju/format.hh>

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

std::string str(cxy::BoundedSequence<int16_t,4> const& x) throw()
{
  std::vector<std::string> y;
  for(auto i : x) {
    y.push_back(xju::format::int_(i));
  }
  return "["+xju::format::join(y.begin(),y.end(),",")+"]";
}

class p24_impl : public p24::i24
{
public:
  ~p24_impl() throw() {}
  
  
  virtual ::cxy::TypeCode f(
    ::cxy::TypeCode const& x) /*throw(
    cxy::Exception)*/ override
  {
    std::cout << "servant: p24::i24::f()"
              << std::endl;
    calls_.push_back(std::shared_ptr<Call>(
                       new Call::f1(x)));
    return x;
  }

  struct Call
  {
    virtual ~Call() throw() {}
    
    struct f1;
  };
  struct Call::f1 : Call
  {
    ~f1() throw() {}
    
    f1(::cxy::TypeCode const& x):
        x_(x) {
    }
    ::cxy::TypeCode const x_;
    friend bool operator==(f1 const& x, f1 const& y) throw()
    {
      return *x.x_==*y.x_;
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
    
    std::string const OBJECT_NAME("p24");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      cxy::ORB<cxy::Exception> orb("giop:tcp::");
      cxy::cref<p24::i24> ref(orb, makeURI(port, OBJECT_NAME));
      auto const result(ref->f(::cxy::createTypeCodeOf<uint32_t>()));
      std::cout << "x" << std::endl;
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      p24_impl x;
      
      cxy::sref<p24::i24> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(std::chrono::steady_clock::now()+
                       std::chrono::seconds(30));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);
      p24_impl x;
      
      cxy::sref<p24::i24> const xa(orb, OBJECT_NAME, x);
      
      cxy::cref<p24::i24> ref(orb, makeURI(port, OBJECT_NAME));
      unsigned int const repeat(
        xju::stringToUInt(::getenv("CXY_REPEAT")?::getenv("CXY_REPEAT"):"1"));
      for(unsigned int i=0; i != repeat; ++i) {
        auto const result(ref->f(::cxy::createTypeCodeOf<uint32_t>()));
        xju::assert_equal(*result, *::cxy::createTypeCodeOf<uint32_t>());
        xju::assert_equal(x.calls_.size(),1U);
        p24_impl::Call::f1 const& c(
          dynamic_cast<p24_impl::Call::f1 const&>(*x.calls_[0]));
        xju::assert_equal(c, p24_impl::Call::f1(
                            ::cxy::createTypeCodeOf<uint32_t>()));
        x.calls_=std::vector<std::shared_ptr<p24_impl::Call> >();
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
