// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include <omnicxy/proto/p5.hh>
#include <omnicxy/proto/p5.cref.hh>
#include <omnicxy/proto/p5.sref.hh>

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

class F_impl : public p5::F
{
public:
  ~F_impl() throw()
  {
  }

  // p5::F::
  virtual ::p5::JW2s f1(
    ::p5::JW2s const& p1) throw(
      // ipc failure
      // - note servant may not throw
      cxy::Exception)
  {
    std::cout << "::p5::f1([";
    for(::p5::JW2s::const_iterator i=p1.begin();i!=p1.end();++i) {
      std::cout << "\n  ::p4::XS1("
                << (*i).a_ << ", "
                << (*i).b_ << ")";
    }
    std::cout << "])-> ::p5::f1([::p4::XS1(" 
              << ::p3::MyInt(62) << ", "
              << std::string("jock") << ")])" << std::endl;
    calls_.push_back(std::shared_ptr<Call>(
                       new Call::f1(p1)));
    return ::p5::JW2s(1U, ::p4::XS1(::p3::MyInt(62), std::string("jock")));
  }
  struct Call
  {
    virtual ~Call() throw()
    {
    }
    struct f1;
  };
  struct Call::f1 : Call
  {
    ~f1() throw()
    {
    }
    f1(::p5::JW2s const& a) throw():
        a_(a) {
    }
    ::p5::JW2s a_;
    
    friend bool operator==(f1 const& x, f1 const& y) throw()
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
    
    std::string const OBJECT_NAME("p5");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      cxy::ORB<cxy::Exception> orb("giop:tcp::");
      cxy::cref<p5::F> ref(orb, makeURI(port, OBJECT_NAME));
      ::p5::JW2s const r(
        ref->f1(
          ::p5::JW2s(1U,::p4::XS1(::p3::MyInt(77),std::string("argy")))));
      std::cout << "[";
      for(::p5::JW2s::const_iterator i=r.begin();i!=r.end();++i) {
        std::cout << "\n  ::p4::XS1("
                  << (*i).a_ << ", "
                  << (*i).b_ << ")";
      }
      std::cout << "]" << std::endl;
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);

      F_impl x;
      
      cxy::sref<p5::F> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(std::chrono::system_clock::now()+
                       std::chrono::seconds(30));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<cxy::Exception> orb(orbEndPoint);
      F_impl x;
      
      cxy::sref<p5::F> const xa(orb, OBJECT_NAME, x);
      
      cxy::cref<p5::F> ref(orb, makeURI(port, OBJECT_NAME));
      xju::assert_equal(
        ref->f1(::p5::JW2s(1U,::p4::XS1(::p3::MyInt(19),std::string("al")))),
        ::p5::JW2s(1U,::p4::XS1(::p3::MyInt(62),std::string("jock"))));
      xju::assert_equal(x.calls_.size(),1U);
      {
        F_impl::Call::f1 const& c(
          dynamic_cast<F_impl::Call::f1 const&>(*x.calls_[0]));
        xju::assert_equal(
          c,
          F_impl::Call::f1(
            ::p5::JW2s(1U,::p4::XS1(::p3::MyInt(19),std::string("al")))));
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
